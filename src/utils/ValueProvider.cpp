/*
 * ValueProvider.cpp
 *
 * Copyright (C) 2013 Holger Grosenick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <cstring>
#include <limits>

#include "Exceptions.h"
#include "StringUtil.h"
#include "ValueProvider.h"


namespace utils {


ValueProvider::ValueProvider(int _min, int _max, unsigned int _interval)
  : min(_min)
  , max(_max)
  , lastValue(0)
  , lastValueTime(0)
  , interval(_interval)
{
    if (max < min)
        throw LogicError("max < min in ValueProvider definition");
}

ValueProvider::~ValueProvider() {
}

/**
 * dynamically create a known value provider from some options which have the
 * following format:<br>
 * "linear <min> <max> <interval>" <br>
 * "random <min> <max> <interval>" <br>
 * "stored <filename>" <br>
 */
ValueProvider* ValueProvider::buildFrom(const std::string& options)
{
    size_t l = options.length();
    if (l < 10)
        return NULL;

    size_t first = 0;
    while (first < l && options[first] == ' ')
        ++first;

    // find end of first word
    size_t last = first + 1;
    while (last < l && options[last] != ' ')
        ++last;
    if (last - first < 5)
        return NULL;

    // extract type
    std::string type = options.substr(first, last-first);

    if (type.compare("stored") == 0)
    {
        while (last < l && options[last] == ' ')
            ++last;

        // load a text-file here, not a properties file!
        StoredValueProvider *sp = new StoredValueProvider(options.substr(last).c_str());
        return sp;
    }

    // parse the rest of the string
    Properties p(options.substr(last), ',');
    if (type.compare("const") == 0)
    {
        int value = p.getInt("value");
        return new ConstantValueProvider(value);
    }

    int interval = p.getInt("interval");
    if (type.compare("onOff") == 0)
    {
        return new OnOffValueProvider(interval);
    }

    int min = p.getInt("min");
    int max = p.getInt("max");
    if (type.compare("linear") == 0)
    {
        int step = p.getInt("step");
        return new LinearValueProvider(min, max, step, interval);
    }
    if (type.compare("sinus") == 0)
    {
        return new SinusValueProvider(min, max, interval);
    }
    if (type.compare("random") == 0)
    {
        return new RandomValueProvider(min, max, interval);
    }
    return NULL;
}

/**
 * Init with given value range and update frequency.
 */
RandomValueProvider::RandomValueProvider(int _min, int _max, unsigned int _interval)
  : ValueProvider(_min, _max, _interval)
{
    srandom(time(NULL));
    lastValue = calcValue();
}

/**
 * Calculate a new value in the given range.
 */
int RandomValueProvider::calcValue() const
{
    long v = random();
    int result = (v % (max - min + 1)) + min;
    return result;
}

/**
 * Get the actual value at the given relative time.
 */
int RandomValueProvider::getValue(uint64_t relativeTimeMs)
{
    if (relativeTimeMs - lastValueTime < interval)
        return lastValue;

    lastValueTime = relativeTimeMs;
    lastValue = calcValue();
    return lastValue;
}


/**
 * Init with given value range and update frequency.
 */
LinearValueProvider::LinearValueProvider(int _min, int _max, int _step, unsigned int _interval)
  : ValueProvider(_min, _max, _interval)
  , step(_step)
{
    lastValue = _min;
}


/**
 * Get the actual value at the given relative time.
 */
int LinearValueProvider::getValue(uint64_t relativeTimeMs)
{
    if (relativeTimeMs - lastValueTime < interval)
        return lastValue;

    lastValueTime = relativeTimeMs;
    int next = lastValue + step;
    if (step > 0 && next >= max)
    {
        lastValue = max;
        step = -step;
    }
    else if (step < 0 && next <= min)
    {
        lastValue = min;
        step = -step;
    }
    else
        lastValue = next;
    return lastValue;
}



/**
 * Init with given value.
 */
ConstantValueProvider::ConstantValueProvider(int v)
  : ValueProvider(v, v, 1)
{
    lastValue = v;
}

/**
 * Returns a constant value.
 */
int ConstantValueProvider::getValue(uint64_t relativeTimeMs)
{
    return lastValue;
}


OnOffValueProvider::OnOffValueProvider(unsigned intervalOn, unsigned _durationOn, unsigned _randomTime)
   : ValueProvider(0, 1, intervalOn)
   , durationOn(_durationOn)
   , randomTime(_randomTime)
   , isOn(false)
{
    if (randomTime > 0)
        throw LogicError("randomTime not yet supported");
}

/**
 * Get the actual value at the given relative time.
 */
int OnOffValueProvider::getValue(uint64_t relativeTimeMs)
{
    if (isOn) {
        if (relativeTimeMs - lastValueTime < durationOn)
            return 1;
        isOn = false;
        lastValueTime = relativeTimeMs;
        return 0;
    }

    if (relativeTimeMs - lastValueTime < interval)
        return 0;
    isOn = true;
    lastValueTime = relativeTimeMs;
    return 1;
}


/**
 * Init with given value range and sinus cycle time.
 */
SinusValueProvider::SinusValueProvider(int _min, int _max, unsigned int _interval)
  : ValueProvider(_min, _max, _interval)
  , callcount(0)
{
    lastValue = _min;
}


/**
 * Get the actual value at the given relative time.
 */
int SinusValueProvider::getValue(uint64_t relativeTimeMs)
{
    uint64_t timeDelta = (relativeTimeMs - lastValueTime);
    if (timeDelta > interval)
    {
        lastValueTime = relativeTimeMs;
        timeDelta -= interval;
    }
    else {
        // don't calculate 1000 times per second, only 250 times
        if (++callcount & 3)
            return lastValue;
    }

    double part = 360.0 * 3.14159265 / 180.0 * timeDelta / interval;

    // sin() return -1 .. 1  => keep in range 0..1
    double sinus = (sin(part) + 1) * 0.5;
    double res = (max - min) * sinus + min;

    //printf("Delta %6lu -> part =%.4f, sinus = %.4f, result = %.1f\n", timeDelta, part, sinus, res);
    lastValue = res;
    return lastValue;
}


/**
 * Init with given value range and update frequency.
 */
StoredValueProvider::StoredValueProvider(const char *filename)
  : ValueProvider(0, 0, 0)
  , values()
  , current(values.begin())
{
    std::ifstream input;
    input.open(filename);

    // open file
    if (!input.is_open()) {
        throw FileOpenError(true, filename);
    }

    min = std::numeric_limits<int>::max();
    max = std::numeric_limits<int>::min();

    // now parse text
    std::string line;
    int lineNo = 0;
    while (std::getline(input, line))
    {
        ++lineNo;
        size_t l = line.length();
        if (l == 0 || line[0] == '#')
            // comment
            continue;

        // trim key
        size_t start = 0;
        while (start < l && line[start] == ' ')
            ++start;
        size_t end = start+1;
        while (end < l && line[end] != ' ')
            ++end;
        std::string val = line.substr(start, end-start);

        // first value is the time
        unsigned time = atol(val.c_str());
        int value;
        int base;

        // trim values
        start = end + 1;
        while (start < l && line[start] == ' ')
            ++start;

        // determine value base
        val = line.substr(start, l);
        if (utils::StringUtil::startsWith(val, "0x")) {
            base = 16;
            val = val.substr(2);
        }
        else if (utils::StringUtil::startsWith(val, "0b")) {
            base = 2;
            val = val.substr(2);
        }
        else
            base = 10;

        char *endPtr = NULL;
        value = strtol(val.c_str(), &endPtr, base);

        if (value < min)
            min = value;
        if (value > max)
            max = value;
        values.push_back(TValueItem(time, value));
    }
    input.close();

    if (values.empty())
        throw Exception(std::string("No values defined in ") + filename);

    // set-up again
    current = values.cbegin();
}


/**
 * Init with given value range and update frequency.
 */
StoredValueProvider::StoredValueProvider(const Properties &props, const std::string &key)
  : ValueProvider(0, 0, 0)
  , values()
  , current(values.begin())
{
    unsigned row = 0;
    char buffer[512];

    if (key.length() > 500)
        throw Exception("Key too long: max 500 chars supported!");

    while (row < 999999999)
    {
        sprintf(buffer, "%s%u", key.c_str(), ++row);
        const char *value = props.get(buffer);

        // not found ...
        if (value == NULL || *value == 0)
            break;

        if (strlen(value) > 30)
            throw Exception(std::string("Strange value in properties for key ") + buffer);

        // trim value
        while (*value == ' ')
            ++value;
        int end = 0;
        while (value[end] != ' ')
        {
            if (value[end] == 0)
                throw Exception(std::string("Value needs a blank between two integers, but hasn't, key: ") + buffer);
            ++end;
        }
        memcpy(buffer, value, end);
        buffer[end] = 0;

        unsigned time = atol(buffer);

        // trim values
        value = value + end;
        while (*value == ' ')
            ++value;

        // determine value base
        int base;
        if (*value == '0' && value[1] == 'x') {
            base = 16;
            value += 2;
        }
        else if (*value == '0' && value[1] == 'b') {
            base = 2;
            value += 2;
        }
        else
            base = 10;

        char *endPtr = NULL;
        int v = strtol(value, &endPtr, base);

        if (v < min)
            min = v;
        if (v > max)
            max = v;
        values.push_back(TValueItem(time, v));
    }

    if (values.empty())
        throw Exception(std::string("No values defined in properties with key-prefix") + key);

    // set-up again
    current = values.cbegin();
}


/**
 * Get the actual value at the given relative time.
 */
int StoredValueProvider::getValue(uint64_t relativeTimeMs)
{
    TValueItem value = *current;
    unsigned relTime = std::get<0>(value);

    if (lastValueTime + relTime < relativeTimeMs)
    {
        bool done = false;
        do {
            if (++current == values.cend())
            {
                // start at the beginning again
                lastValueTime = relativeTimeMs;
                current = values.begin();
                value = *current;
                done = true;
            }
            else {
                value = *current;
                relTime = std::get<0>(value);
                if (lastValueTime + relTime >= relativeTimeMs)
                    done = true;
            }
        } while (!done);
    }

    int result = std::get<1>(value);
    return result;
}

}
