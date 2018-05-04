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
#include <math.h>
#include <fstream>
#include <cstring>
#include <ctime>
#include <limits>

#include "Exceptions.h"
#include "StringUtil.h"
#include "ValueProvider.h"


namespace utils {


ValueProvider::ValueProvider(int _min, int _max, unsigned int _interval)
  : lastValueTime(0)
  , lastValue(0)
  , interval(_interval)
  , min(_min)
  , max(_max)
{
    if (max < min)
        throw std::logic_error("max < min in ValueProvider definition");
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
        int duration = p.getInt("duration");
        return new OnOffValueProvider(interval, duration);
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
    srand(time(NULL));
    lastValue = calcValue();
}

/**
 * Calculate a new value in the given range.
 */
int RandomValueProvider::calcValue() const
{
    long v = rand();
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
        throw std::logic_error("randomTime not yet supported");
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
 * The internal for Sinus defines the interval for one "wave".
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

    // printf("Delta %6lu %4u -> part =%.4f, sinus = %.4f, result = %.1f\n", timeDelta, interval, part, sinus, res);
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
  , valueCount(0)
  , latestTime(0)
  , timeOffset(0)
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
    unsigned lineNo = 0;
    while (std::getline(input, line))
    {
        ++lineNo;
        parseLine(line, lineNo);
    }
    input.close();

    if (checkSequence() == 0)
        throw Exception(std::string("No values defined in ") + filename);
}


/**
 * Init with given value range and update frequency.
 */
StoredValueProvider::StoredValueProvider(const Properties &props, const std::string &key)
  : ValueProvider(0, 0, 0)
  , values()
  , current(values.begin())
  , valueCount(0)
  , latestTime(0)
  , timeOffset(0)
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

        std::string line(value);
        parseLine(line, row);
    }

    if (checkSequence() == 0)
        throw Exception(std::string("No values defined in properties with key-prefix ") + key);
}

/**
 * Check consistency return number of value items (no random).
 */
unsigned StoredValueProvider::checkSequence()
{
    unsigned act = 0;
    unsigned row = 0;

    for (auto it : values)
    {
        ++row;
        if (it.isRandom)
            continue;

        if (it.timeOffset < act)
        {
            char error[128];
            sprintf(error, "Relative time in row %u is lower than previous one!", row);
            throw Exception(error);
        }
        act = it.timeOffset;
    }

    // first value
    current = values.begin();
    if (current != values.end())
        checkApplyRandom();
    return valueCount;
}

/**
 * Check if the current item is a random marker and set a new random
 * time offset if it is a random marker.
 */
void StoredValueProvider::checkApplyRandom()
{
    if (current->isRandom)
    {
        // determine new random offset
        unsigned newRandom = rand() % current->addOffset;
        std::list<TValueItem>::iterator next = current;

        do {
            if (++next == values.end())
                next = values.begin();

            // apply only to value items, stop at next random item
            if (next->isRandom)
                break;
            next->addOffset = newRandom;
            // printf("***** set offset to %u, time = %u *****\n", newRandom, next->timeOffset);
        } while (true);

        if (++current == values.end())
            current = values.begin();

        // printf("***** CURRENT is time = %u %d *****\n", current->timeOffset, current->value);
        if (current->isRandom)
            throw Exception("Random markers must have at least one normal item in between, found two in a sequence!");
    }
}

/**
 * Get the actual value at the given relative time.
 */
int StoredValueProvider::getValue(uint64_t relativeTimeMs)
{
    TValueItem item = *current;
    unsigned relTime = item.getOffset();

    if (lastValueTime + relTime < relativeTimeMs)
    {
        bool done = false;
        do {
            if (++current == values.end())
            {
                // start at the beginning again
                lastValueTime = relativeTimeMs;
                current = values.begin();
                checkApplyRandom();

                item = *current;
                done = true;
            }
            else {
                checkApplyRandom();
                item = *current;
                relTime = item.getOffset();
                if (lastValueTime + relTime >= relativeTimeMs)
                    done = true;
            }
        } while (!done);
    }

    int result = item.value;
    return result;
}


/**
 * Init with given value range and update frequency.
 */
void StoredValueProvider::parseLine(std::string &line, unsigned lineNo)
{
    size_t l = line.length();
    if (l == 0 || line[0] == '#')
        // comment
        return;

    // trim key
    size_t start = 0;
    while (start < l && line[start] == ' ')
        ++start;
    size_t end = start+1;
    while (end < l && line[end] != ' ')
        ++end;
    std::string val = line.substr(start, end-start);

    // next item has zero time value again ...
    bool isOffset = val.compare("offset") == 0;
    bool isRandom = val.compare("random") == 0;

    // first value is the time
    unsigned time = isRandom || isOffset ? 0 : atol(val.c_str());
    int value;
    int base;

    // trim values
    start = end + 1;
    while (start < l && line[start] == ' ')
        ++start;

    // determine value base
    val = line.substr(start, l);
    if (utils::strings::startsWith(val, "0x")) {
        base = 16;
        val = val.substr(2);
    }
    else if (utils::strings::startsWith(val, "0b")) {
        base = 2;
        val = val.substr(2);
    }
    else
        base = 10;

    char *endPtr = NULL;
    value = strtol(val.c_str(), &endPtr, base);
    if (endPtr == val.c_str())
    {
        char error[128];
        sprintf(error, "Invalid line format in value of line %u", lineNo);
        throw Exception(error);
    }

    // add more offset to all following values?
    if (isOffset)
    {
        timeOffset = latestTime+value;
        return;
    }

    // mark random delay time entry
    if (isRandom) {
        values.push_back(TValueItem(value));
        return;
    }

    // normal entry: time + value
    if (value < min)
        min = value;
    if (value > max)
        max = value;

    latestTime = time + timeOffset;
    values.push_back(TValueItem(latestTime, value));
    ++valueCount;
}

/**
 * Mark a random entry
 */
StoredValueProvider::TValueItem::TValueItem(unsigned r)
  : timeOffset(0), addOffset(r), value(0), isRandom(true) { }

/**
 * A normal value entry
 */
StoredValueProvider::TValueItem::TValueItem(unsigned t, int v)
  : timeOffset(t), addOffset(0), value(v), isRandom(false) { }

}
