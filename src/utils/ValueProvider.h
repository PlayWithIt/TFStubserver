/*
 * ValueProvider.h
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

#ifndef VALUEPROVIDER_H_
#define VALUEPROVIDER_H_

#include <stdint.h>
#include <list>
#include <tuple>
#include <string>

#include "Properties.h"



namespace utils {

/**
 * @defgroup ValueProvider Classes that generate values that maybe consumed somewhere else.
 */

/**
 * @ingroup ValueProvider
 *
 * Interface for value providers that provide a value at a relative time.
 * Derived classes might use constant values or calculate 'real' values.
 */
class ValueProvider {
protected:
    int min, max;
    int lastValue;

    uint64_t lastValueTime;
    unsigned interval;

    ValueProvider(int _min, int _max, unsigned int interval = 0);

public:
    virtual ~ValueProvider();

    /**
     * Dynamically create a known value provider from some options which have the
     * following format:<br>
     * "linear <min> <max> <step> <interval>" <br>
     * "random <min> <max> <interval>" <br>
     * "sinus  <min> <max> <interval>" <br>
     * "stored <filename>" <br>
     */
    static ValueProvider* buildFrom(const std::string &options);

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs) = 0;

    /**
     * Returns the minimum of values which will be returned by this ValueProvider.
     * This is not the minimum of values returned so far, but the general minimum
     * of expected values.
     */
    int getMin() const {
        return min;
    }

    /**
     * Returns the maximum of values which will be returned by this ValueProvider.
     * This is not the maximum of values returned so far, but the general maximum
     * of expected values.
     */
    int getMax() const {
        return max;
    }

    /**
     * Returns the last value that was returned by the last call to {@link getValue()}.
     */
    int getLastValue() const {
        return lastValue;
    }
};


/**
 * @ingroup ValueProvider
 *
 * Random value provider which will change after the given interval
 * to a new random value in the given range min..max (included).
 */
class RandomValueProvider : public ValueProvider
{
    // calculate a new random value
    int calcValue() const;

public:
    RandomValueProvider(int min, int max, unsigned int interval);

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs);
};


/**
 * @ingroup ValueProvider
 *
 * A linear value provider increments the current value from min to
 * max with the given step range. If max is reached, the value is
 * reduced to min with the negative step.
 */
class LinearValueProvider : public ValueProvider
{
    int step;

public:
    LinearValueProvider(int min, int max, int step, unsigned int interval);

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs);
};


/**
 * @ingroup ValueProvider
 *
 * Always returns the same value at any time: constant value.
 */
class ConstantValueProvider : public ValueProvider
{
public:
    ConstantValueProvider(int v);

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs);
};


/**
 * @ingroup ValueProvider
 *
 * A value provider which returns only two different values: 0 or 1.
 * The state is switched to ON (1) after the 'intervalOn' and turned
 * off again after the 'durationOn'.
 */
class OnOffValueProvider : public ValueProvider
{
    unsigned durationOn;
    unsigned randomTime;
    bool     isOn;

public:
    OnOffValueProvider(unsigned intervalOn, unsigned durationOn = 5, unsigned randomTime = 0);

    /**
     * Get the actual value at the given relative time: this implementation
     * will return only 0 (switch off) or 1 (switch on).
     */
    virtual int getValue(uint64_t relativeTimeMs);
};


/**
 * @ingroup ValueProvider
 *
 * A value provider that creates curves and can calculate a value for any
 * millisecond. The given interval defines the time in milliseconds after
 * that the curve is back at the beginning.
 */
class SinusValueProvider : public ValueProvider
{
    unsigned callcount;

public:
    SinusValueProvider(int min, int max, unsigned int interval);

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs);
};


/**
 * @ingroup ValueProvider
 *
 * A value provider which reads time and return values out of a file:
 * one line per value with two integers separated by space.
 * If the end of the value list is reached, it is started at the beginning again.
 */
class StoredValueProvider : public ValueProvider
{
    typedef std::tuple<unsigned,int> TValueItem;

    std::list<TValueItem> values;
    std::list<TValueItem>::const_iterator current;

public:
    /**
     * Reads the values from the given file, throws an exception if the file
     * does not exist or cannot be read.
     */
    StoredValueProvider(const char *filename);

    /**
     * Reads the values from properties with a given prefix: the prefix will
     * be extended with a line number in order to extract the values.
     * If the prefix is "data", "data1", "data2", ... are read until a
     * key was not found.
     * <P>
     * Each property value must be a tuple of two blank separated integers.
     */
    StoredValueProvider(const Properties &props, const std::string &prefix);

    /**
     * Returns the number of values defined in the input file.
     */
    size_t numValues() const {
        return values.size();
    }

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs);
};

}

#endif /* VALUEPROVIDER_H_ */
