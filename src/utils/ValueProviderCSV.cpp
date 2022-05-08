/*
 * ValueProviderCSV.cpp
 *
 * Copyright (C) 2020-2022 Holger Grosenick
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
#include <time.h>
#include <string.h>

#include <fstream>
#include <cstring>
#include <ctime>
#include <limits>

#include "Exceptions.h"
#include "Log.h"
#include "StringUtil.h"
#include "ValueProvider.h"
#include "SimpleCsv.h"


namespace utils {


/**
 * Parses a timestamp like "2020-01-01 11:04:37.888365" and return the time in milli-seconds
 * returned by 'mktime' multiplied by 1000 plus the milliseconds of the log timestamp.
 * <P>
 * If the result is 0, a parse error has occurred.
 */
uint64_t CSVValueProvider::parseTimestamp(const char *ts)
{
    struct tm scannedTime;
    uint64_t result = 0;
    unsigned millis;

    memset(&scannedTime, 0, sizeof(scannedTime));
    int rc = SSCANF(ts, "%d-%d-%d %d:%d:%d.%u",
                    &scannedTime.tm_year, &scannedTime.tm_mon, &scannedTime.tm_mday,
                    &scannedTime.tm_hour, &scannedTime.tm_min, &scannedTime.tm_sec, &millis);
    if (rc == 7) {
        scannedTime.tm_year -= 1900;
        scannedTime.tm_mon  -= 1;
        result = mktime(&scannedTime) * 1000 + (millis/1000);
    }

    return result;
}


/**
 * Init with given value range and update frequency.
 */
CSVValueProvider::CSVValueProvider(const File &file)
  : CSVValueProvider(file.getAbsolutePath())
{
}

/**
 * Init with given value range and update frequency.
 */
CSVValueProvider::CSVValueProvider(const std::string &filename)
  : ValueProvider(0, 0, 0)
  , values()
  , current(values.begin())
{
    // load data from CSV file
    utils::SimpleCsvCharP csv(filename);

    // which load format is used?
    unsigned numCols = csv.getNumCols();
    const std::vector< const char* > &cols = csv.getColumns();
    std::vector<int> columnData;
    char msg[512];

    uint64_t offset = 0;
    //printf("NumCols = %u in %s\n", numCols, filename);

    while (csv.loadLine()) {
        unsigned size = static_cast<unsigned>(cols.size());
        if (size != numCols) {
            snprintf(msg, sizeof(msg), "Invalid number of columns (%u, expected is %u) in line %u of %s", size, numCols, csv.getLine(), filename.c_str());
            msg[sizeof(msg) - 1] = 0;
            throw utils::ValueFormatError(msg);
        }

        if (values.empty() && strchr(cols[0], ':') != NULL) {
            // check if the first column is a timestamp
            // Date from log format is: 2020-01-01 11:04:37.888365

            offset = parseTimestamp(cols[0]);
            if (offset > 0) {
                Log() << "CSVValueProvider uses timestamp offset of " << offset << " in " << filename;
            }
        }

        columnData.clear();
        for (unsigned i = 0; i < size; ++i) {
            if (i == 0 && offset > 0)
            {
                uint64_t v = parseTimestamp(cols[0]);
                if (v > 0) {
                    v -= offset;
                    columnData.push_back(v);
                    // printf("Store offset %ld in line %d\n", v, csv.getLine());
                }
                else {
                    snprintf(msg, sizeof(msg), "Invalid timestamp conversion in line %u of %s", csv.getLine(), filename.c_str());
                    msg[sizeof(msg) - 1] = 0;
                    throw utils::ValueFormatError(msg);
                }
            }
            else {
                int value = atoi(cols[i]);
                columnData.push_back(value);
            }
        }
        values.push_back(columnData);
    }

    if (checkSequence() == 0)
        throw Exception(std::string("No values defined in ") + filename);
}


/**
 * Check consistency return number of value items (no random).
 */
size_t CSVValueProvider::checkSequence()
{
    int      act = 0;
    unsigned row = 0;

    for (auto it : values)
    {
        ++row;
        if (it[0] < act)
        {
            char msg[200];
            snprintf(msg, sizeof(msg), "Relative time in data row %u is lower than previous one (%d < %d)!", row, it[0], act);
            msg[sizeof(msg) - 1] = 0;
            throw Exception(msg);
        }
        act = it[0];
    }

    // first value
    current = values.cbegin();
    lastValueTime = 0;

    // dump();

    return row;
}

/**
 * Just for tests.
 */
void CSVValueProvider::dump() const
{
    printf("Next value up to time %d is %d\n", (*current)[0], (*current)[1]);
    unsigned row = 0;
    for (auto & it : values) {
        printf("Row %2u: value up to time %d is %d\n", row, (it)[0], (it)[1]);
        ++row;
    }
}

/**
 * Get the actual value at the given relative time.
 */
int CSVValueProvider::getValue(uint64_t relativeTimeMs)
{
    unsigned relTime = (*current)[0];

    // printf("Current value up to time %d is %d\n", (*current)[0], (*current)[1]);

    if (lastValueTime + relTime < relativeTimeMs)
    {
        do {
            if (++current == values.cend())
            {
                // start at the beginning again
                lastValueTime += relTime;
                current = values.begin();
                // printf("INC valueTime to %lu, next value is %d\n", lastValueTime, (*current)[1]);
                // dump();
            }

            relTime = (*current)[0];

        } while (lastValueTime + relTime < relativeTimeMs);
    }

    int result = (*current)[1];
    // printf("value at time %lu is %d\n", relativeTimeMs, result);
    return result;
}

/**
 * Get all values at the given relative time: the whole data row with all columns
 */
int CSVValueProvider::getValues(uint64_t relativeTimeMs, std::vector<int> &result)
{
    // assure a proper position in "current"
    getValue(relativeTimeMs);

    // copy whole vector
    result.clear();
    size_t last = (*current).size();
    for (size_t i = 1; i < last; ++i) {
        result.push_back( (*current)[i] );
    }

    return static_cast<int>(last-1);
}


}
