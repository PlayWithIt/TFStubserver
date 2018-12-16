/*
 * DateTimeFormat.h
 *
 * Copyright (C) 2014 Holger Grosenick
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
#ifndef DATETIMEFORMAT_H_
#define DATETIMEFORMAT_H_

#include <iostream>

namespace utils {

class DateTime;

/**
 * Provides a formatter which can be used to format an object of type {@link DateTime}.
 *
 * <b>Format string variables:</b><br>
 * y - 2 digit year with leading '0' <br>
 * Y - 4 digit year <br>
 * M - month in year (1..12) <br>
 * 3M - month in year, 3 letter (Jan .. Dec) <br>
 * 9M - month in year, all letters (January .. December) <br>
 * D - day in month (1..31) <br>
 * h - hour (0..23) <br>
 * H - hour (12..11) for am/pm (see http://de.wikipedia.org/wiki/2-mal-12-Stunden-Z%C3%A4hlung) <br>
 * a - am/pm flag
 * m - minute (0..59) <br>
 * s - second (0..59) <br>
 * t - millisecond (000..999 - always 3 digits) <br>
 * u - microsecond (000000..999999 - always 6 digits)
 * <p>
 * if a '%' is followed by '0' the next value has a leading zero (e.g. '01' instead of '1') if necessary.
 */
class DateTimeFormat
{
    std::string format;
    bool invalid;

protected:
    explicit DateTimeFormat();

public:
    explicit DateTimeFormat(const char *_format);
    explicit DateTimeFormat(const std::string &_format);

    virtual ~DateTimeFormat();

    // %Y-%0M-%0D
    static const char * const LOG_FORMAT_DATE;

    // %Y-%0M-%0D %0h:%0m:%0s
    static const char * const LOG_FORMAT_DATE_TIME;
    static const char * const LOG_FORMAT_DATE_TIME_MILLIS;
    static const char * const LOG_FORMAT_DATE_TIME_MICROS;

    /**
     * Is the current object still valid? This is more a workaround for streams which
     * are still open, but the DateTimeFormat was only a temporary object. If the
     * destructor was already called, this flag is not 0.
     */
    bool isInvalid() const {
        return invalid;
    }

    virtual void printTo(std::ostream& out, const DateTime& f) const;
};


/**
 * Mark the stream with the given DateTimeFormat.
 */
std::ostream& operator<< (std::ostream& out, const DateTimeFormat& f);

/**
 * Check if a DateTimeFormat is active in the stream and then format the date.
 */
std::ostream& operator<< (std::ostream& out, const DateTime& f);

} /* namespace utils */

#endif /* DATETIMEFORMAT_H_ */
