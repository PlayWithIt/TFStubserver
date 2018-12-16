/*
 * DateTime.cpp
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

#ifdef _WIN32
#include <time.h>
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif

#include "DateTime.h"


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
 * Assignment operator
 */
DateTime& DateTime::operator=(const DateTime &other)
{
    time = other.time;
    secondsSinceEpoch = other.secondsSinceEpoch;
    microsecs = other.microsecs;
    return *this;
}

/**
 * Split update the seconds in single fields.
 */
void DateTime::makeTime()
{
    localtime_r( &secondsSinceEpoch, &time );
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


} /* namespace utils */
