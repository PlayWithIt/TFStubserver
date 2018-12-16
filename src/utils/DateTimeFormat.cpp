/*
 * DateTimeFormat.cpp
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

#include <stdexcept>

#include "DateTime.h"
#include "DateTimeFormat.h"

namespace utils {

// defines the index in user defined storage where to save the latest DateTimeFormat object
static int const dateFormatIndex = std::ios_base::xalloc();

const char * const DateTimeFormat::LOG_FORMAT_DATE             = "%Y-%0M-%0D";
const char * const DateTimeFormat::LOG_FORMAT_DATE_TIME        = "%Y-%0M-%0D %0h:%0m:%0s";
const char * const DateTimeFormat::LOG_FORMAT_DATE_TIME_MILLIS = "%Y-%0M-%0D %0h:%0m:%0s.%t";
const char * const DateTimeFormat::LOG_FORMAT_DATE_TIME_MICROS = "%Y-%0M-%0D %0h:%0m:%0s.%u";


DateTimeFormat::DateTimeFormat(const char *_format)
  : format(_format)
  , invalid(false)
{
}

DateTimeFormat::DateTimeFormat(const std::string &_format)
  : format(_format)
  , invalid(false)
{
}

DateTimeFormat::~DateTimeFormat() {
    invalid = true;
}

inline void twoDigit(std::ostream& out, bool zero, unsigned x)
{
    if (x < 10 && zero)
        out << '0';
    out << x;
}

/**
 * Validate format string consistency.
 */
void DateTimeFormat::printTo(std::ostream& out, const DateTime& f) const
{
    char error[64];
    unsigned s;
    bool zero = false;

    char c;
    const char *fmt = format.c_str();

    while ((c = *fmt) != 0)
    {
        // non-format symbol, separator
        if (c != '%')
        {
            out << c;
            ++fmt;
            continue;
        }

        // char char behind '%'
        ++fmt;
        if (*fmt == '0') {
            zero = true;
            ++fmt;
        }
        else
            zero = false;

        switch (*fmt) {
        case '%':
            out << c; break;

        case 'y':
            // y - 2 digit year with leading '0'
            twoDigit(out, zero, f.year() / 100);
            break;

        case 'Y':
            // Y - 4 digit year
            out << f.year();
            break;

        case 'M':
            // M - month in year (1..12)
            twoDigit(out, zero, f.month());
            break;

        case 'D':
            // D - day in month (1..31)
            twoDigit(out, zero, f.day());
            break;

        case 'h':
            // h - hour (0..23)
            twoDigit(out, zero, f.hour());
            break;

        case 'H':
            // hour (12..11am/pm)
            s = f.hour();
            if (s == 0 || s == 12)
                out << "12";
            else
                twoDigit(out, zero, s % 12);
            break;

        case 'm':
            // m - minute (0..59)
            twoDigit(out, zero, f.minute());
            break;

        case 's':
            // s - second (0..59)
            twoDigit(out, zero, f.second());
            break;

        case 't':
            // t - millisecond (0..999)
            s = f.millis();
            if (s < 10)
                out << "00";
            else if (s < 100)
                out << '0';
            out << s;
            break;

        case 'u':
            // t - millisecond (0..999)
            s = f.micros();
            if (s < 100000)
            {
                if (s < 10)
                    out << "00000";
                else if (s < 100)
                    out << "0000";
                else if (s < 1000)
                    out << "000";
                else if (s < 10000)
                    out << "00";
                else
                    out << '0';
            }
            out << s;
            break;

        case 'a':
            out << (f.hour() < 12 ? "am" : "pm");
            break;

        case 0:
            // end of string
            return;

        default:
            sprintf(error, "Format symbol '%%%c' invalid for DateTimeFormat", c);
            throw std::invalid_argument(error);
        }
        ++fmt;
    }
}

/**
 * Mark the stream with the given DateTimeFormat.
 */
std::ostream& operator<< (std::ostream& out, const DateTimeFormat& f) {
    out.pword(dateFormatIndex) = const_cast<DateTimeFormat*>(&f);
    return out;
}

/**
 * Check if a DateTimeFormat is active in the stream and then format the date.
 */
std::ostream& operator<< (std::ostream& out, const DateTime& f)
{
    const DateTimeFormat* manip = (const DateTimeFormat*) out.pword(dateFormatIndex);
    if (manip && false == manip->isInvalid())
        manip->printTo(out, f);
    return out;
}

} /* namespace utils */
