/*
 * ValueProvider.h
 *
 * Copyright (C) 2013-2021 Holger Grosenick
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
#include <vector>
#include <string>

#include "File.h"
#include "Properties.h"



namespace utils {

/**
 * @defgroup ValueProvider Classes that generate values that maybe consumed somewhere else.
 */

/**
 * @ingroup ValueProvider
 *
 * Interface for value providers that provide a value at a relative time (measured in milliseconds).
 * Derived classes might use constant values or calculate 'real' values.
 */
class ValueProvider {
protected:
    uint64_t lastValueTime;
    int      lastValue;
    unsigned interval;
    int      min, max;

    ValueProvider(int _min, int _max, unsigned int intervalMs = 0);

public:
    virtual ~ValueProvider();

    /**
     * Dynamically create a known value provider from some options which have the
     * following format:<br>
     * "const  value=<value>" <br>
     * "linear min=<min>,max=<max>,step=<step>,interval=<interval>" <br>
     * "random min=<min>,max=<max>,interval=<interval>" <br>
     * "sinus  min=<min>,max=<max>,interval=<interval>" <br>
     * "onOff  duration=<durationOn>,interval=<intervalOn>" <br>
     * "stored <filename>" <br>
     * "csv    <filename>" <br>
     *
     * @param options options string as described above
     * @param file base directory where to search files if they are not absolute, if NULL: working directory
     */
    static ValueProvider* buildFrom(const std::string &options, const utils::File *basePath = nullptr);

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
    RandomValueProvider(int min, int max, unsigned int intervalMs);

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs);
};


/**
 * @ingroup ValueProvider
 *
 * A linear value provider increments the current value from min to
 * max with the given step range where the interval is in milliseconds.
 * If max is reached, the value is reduced to min with the negative step.
 */
class LinearValueProvider : public ValueProvider
{
    int step;

public:
    LinearValueProvider(int min, int max, int step, unsigned int intervalMs);

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
    SinusValueProvider(int min, int max, unsigned int intervalMs);

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
    struct TValueItem
    {
        unsigned timeOffset;
        unsigned addOffset;
        int      value;
        bool     isRandom;

        /**
         * Mark a random entry
         */
        TValueItem(unsigned);
        /**
         * A normal value entry
         */
        TValueItem(unsigned,int);

        /**
         * Returns the effective time offset in ms.
         */
        unsigned getOffset() const {
            return timeOffset + addOffset;
        }
    };

    std::list<TValueItem> values;
    std::list<TValueItem>::iterator current;
    size_t   valueCount;
    unsigned latestTime;
    unsigned timeOffset;

    // parse a value out of the input line
    void parseLine(std::string &line, unsigned lineNo);

    /**
     * Check consistency return number of value items (no random).
     */
    unsigned checkSequence();
    void checkApplyRandom();

public:
    /**
     * Reads the values from the given file, throws an exception if the file
     * does not exist or cannot be read.
     */
    StoredValueProvider(const std::string &filename);

    /**
     * Reads the values from the given file, throws an exception if the file
     * does not exist or cannot be read.
     */
    StoredValueProvider(const File &file);

    /**
     * Reads the values from properties with a given prefix: the prefix will
     * be extended with a line number in order to extract the values.
     * If the prefix is "data" then "data1", "data2", ... are read until a
     * key was not found.
     * <P>
     * Each property value must be a tuple of two blank separated integers.
     */
    StoredValueProvider(const Properties &props, const std::string &prefix);

    /**
     * Returns the number of values defined in the input file.
     */
    size_t size() const {
        return valueCount;
    }

    /**
     * Get the actual value at the given relative time.
     */
    virtual int getValue(uint64_t relativeTimeMs);
};


/**
 * @ingroup ValueProvider
 *
 * Reads values from a CSV file: one line represents one set of values.
 * The first column is the time offset in milliseconds, the other columns are data values.
 * <P>
 * The first column can also contain a log timestamp like "2020-01-01 11:04:37.888365":
 * in this case the first line defines the initial timestamp which defines the offset for all
 * following line.
 * <br>
 * BE AWARE: if this feature is used, the first line should just contain the timestamp, no
 * additional data as this will be ignored. (First line would be offset 0 which is never active).
 * <P>
 * The CSV can contain '#' in first column to mark a comment (or header)
 */
class CSVValueProvider : public ValueProvider
{
    typedef std::vector<int> TCsvRow;

    std::list<TCsvRow> values;
    std::list<TCsvRow>::const_iterator current;

    /**
     * Check consistency return number of value items (no random).
     */
    unsigned checkSequence();

    // For test purpose
    void dump() const;

public:
    /**
     * Reads the values from the given file, throws an exception if the file
     * does not exist or cannot be read.
     */
    CSVValueProvider(const std::string &filename);

    /**
     * Reads the values from the given file, throws an exception if the file
     * does not exist or cannot be read.
     */
    CSVValueProvider(const File &file);

    /**
     * Returns the number of rows defined in the input file.
     */
    size_t size() const {
        return values.size();
    }

    /**
     * Get the actual value at the given relative time: this is only the first column of the CSV
     */
    virtual int getValue(uint64_t relativeTimeMs);

    /**
     * Get all values at the given relative time: the whole data row with all columns
     */
    int getValues(uint64_t relativeTimeMs, std::vector<int> &value);

    /**
     * Parses a timestamp like "2020-01-01 11:04:37.888365" and return the time in milli-seconds
     * returned by 'mktime' multiplied by 1000 plus the milliseconds of the log timestamp.
     * <P>
     * If the result is 0, a parse error has occurred.
     */
    uint64_t parseTimestamp(const char *ts);
};

typedef ValueProvider *PValueProvider;

}

#endif /* VALUEPROVIDER_H_ */
