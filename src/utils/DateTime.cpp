/*
 * DateTime.cpp
 *
 * Copyright (C) 2014-2021 Holger Grosenick
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

#ifdef _WIN32
#include <time.h>
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif

#include <string.h>

#include "DateTime.h"
#include "Exceptions.h"


namespace utils {

/**
 * Determine current date time with microseconds.
 */
DateTime::DateTime()
{
    struct timeval current;
    gettimeofday(&current, NULL);

    secondsSinceEpoch = current.tv_sec;
    microsecs = current.tv_usec;

    // if this constructor was used, guarantee that microsecs is not 0
    if (microsecs == 0)
        ++microsecs;

    makeTime();
}

/**
 * Sets all fields of 'time' to 0 but inits the values for 'gmtoff' and 'isdst'.
 */
static void initTime(std::tm &time)
{
    time_t current = ::time(NULL);
    struct tm localTime;
    localtime_r(&current, &localTime);

    // set dst in fileTime;
    memset(&time, 0, sizeof(time));
    time.tm_gmtoff = localTime.tm_gmtoff;
    time.tm_isdst  = localTime.tm_isdst;

    // set isDst to 0 so that July and December times return the same time no matter when
    // the method is called.
}

/**
 * Use given date / time without microseconds.
 */
DateTime::DateTime(const std::time_t t)
  : secondsSinceEpoch(t)
  , microsecs(0)
{
    makeTime();
}

/**
 * Use given date / time with microseconds.
 */
DateTime::DateTime(const std::chrono::system_clock::time_point &now)
{
    auto d = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    secondsSinceEpoch = d / 1000000;
    microsecs = d % 1000000;

    //secondsSinceEpoch = now.time_since_epoch().count() * std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;
    makeTime();
}

/**
 * Create a DateTime with additional seconds: if negative the date is before
 * otherwise after the given DateTime.
 *
 * If secondOffset is for example 86400, then one day is added.
 */
DateTime::DateTime(const DateTime &other, int secondOffset)
  : secondsSinceEpoch(other.secondsSinceEpoch + secondOffset)
  , microsecs(other.microsecs)
{
    makeTime();
}

/**
 * Parses a date time out of the fixed format YYYY-MM-DD HH:MI:SS{.mmm}
 * @see #asString()
 */
DateTime::DateTime(const std::string &t)
  : secondsSinceEpoch(0)
  , microsecs(0)
{
    // set dst in fileTime;
    initTime(time);

    int year, mon, day, hour, min, sec, msec;
    auto count = sscanf(t.c_str(), "%d-%d-%d %d:%d:%d.%d", &year, &mon, &day, &hour, &min, &sec, &msec);
    if (count != 7 && count != 6) {
        // invalid format
        throw ValueFormatError("DateTime(\"" + t + "\"): invalid input format");
    }

    init(year, mon, day, hour, min, sec);
    if (count == 7) {
        microsecs = msec * 1000 + 1;
    }
}

/**
 * Init with single fields:<br>
 * year == 2000, month == 1 .. 12, dayOfMonth == 1..31, hour == 0..24
 */
DateTime::DateTime(unsigned year, unsigned month, unsigned dayOfMonth, unsigned hour, unsigned minute, unsigned second)
   : secondsSinceEpoch(0)
   , microsecs(0)
{
    init(year, month, dayOfMonth, hour, minute, second);
}

/**
 * Init with single fields:<br>
 * year == 2000, month == 1 .. 12, dayOfMonth == 1..31, hour == 0..24
 */
void DateTime::init(unsigned year, unsigned month, unsigned dayOfMonth, unsigned hour, unsigned minute, unsigned second)
{
    // set gmtoff clear isDst
    initTime(time);

    time.tm_year = year - 1900;
    time.tm_mon  = month - 1;
    time.tm_mday = dayOfMonth;
    time.tm_hour = hour;
    time.tm_min  = minute;
    time.tm_sec  = second;

    int oldDst = time.tm_isdst;

    secondsSinceEpoch = mktime(&time);
    makeTime();

    int newDst = time.tm_isdst;
    if (oldDst != newDst) {
        // fix dst offset: if we execute the program in July and handle a date in December
        // the hour is not what we put in as parameter but due to conversion in seconds
        // we have to add or subtract one hour and try again.

        if (oldDst == 1 && newDst == 0) {
            secondsSinceEpoch += 3600;
            makeTime();
        }
        else {
            secondsSinceEpoch -= 3600;
            makeTime();
        }
    }
}

/**
 * Assignment operator
 */
DateTime& DateTime::operator=(const DateTime &other)
{
    if (this != &other) {
        time = other.time;
        secondsSinceEpoch = other.secondsSinceEpoch;
        microsecs = other.microsecs;
    }
    return *this;
}

/**
 * Split update the seconds in single fields.
 */
void DateTime::makeTime()
{
    if ( ! localtime_r( &secondsSinceEpoch, &time ) ) {
        // conversion error !
        throw Exception("DateTime::makeTime() failed call to gmtime_r()");
    }
    time.tm_year += 1900;  // 2000 and higher
    time.tm_mon += 1;      // 1 .. 12
}

/**
 * Is this DateTime before another DateTime (microseconds included!).
 */
bool DateTime::before(const DateTime &other) const
{
    if (secondsSinceEpoch < other.secondsSinceEpoch)
        return true;
    if (secondsSinceEpoch > other.secondsSinceEpoch)
        return false;
    return microsecs < other.microsecs;
}

/**
 * Is this DateTime after another DateTime (microseconds included!).
 */
bool DateTime::after(const DateTime &other) const
{
    if (secondsSinceEpoch > other.secondsSinceEpoch)
        return true;
    if (secondsSinceEpoch < other.secondsSinceEpoch)
        return false;
    return microsecs > other.microsecs;
}


/**
 * Is this DateTime at the same day as other DateTime (ignore time)?
 */
bool DateTime::sameDayAs(const DateTime &other) const
{
    return day() == other.day() && month() == other.month() && year() == other.year();
}

/**
 * Returns a unified format in the way YYYY-MM-DD HH:MI:SS{.mmm}
 * Milliseconds are only appended if 'millis = true'
 */
const std::string DateTime::asString(bool millis) const
{
    char buffer[128];

    if (millis)
        sprintf(buffer, "%4d-%02d-%02d %02d:%02d:%02d.%03d",
                time.tm_year,
                time.tm_mon,
                time.tm_mday,
                time.tm_hour,
                time.tm_min,
                time.tm_sec,
                microsecs % 1000);
    else
        sprintf(buffer, "%4d-%02d-%02d %02d:%02d:%02d",
                time.tm_year,
                time.tm_mon,
                time.tm_mday,
                time.tm_hour,
                time.tm_min,
                time.tm_sec);

    return buffer;
}

} /* namespace utils */
