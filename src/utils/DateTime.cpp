/*
 * DateTime.cpp
 *
 * Copyright (C) 2014-2022 Holger Grosenick
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
#include <WinSock2.h>
template<typename T1, typename T2>
constexpr auto localtime_r(T1 in, T2 out) { return localtime_s(out, in); }

#include <chrono>

extern "C" {

int gettimeofday(struct timeval* tp, struct timezone*)
{
    namespace sc = std::chrono;
    sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
    sc::seconds s = sc::duration_cast<sc::seconds>(d);
    tp->tv_sec = s.count();
    tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();
    return 0;
}

}

#else
#include <sys/time.h>
#endif

#include <string.h>

#include "DateTime.h"
#include "Exceptions.h"


// Be aware that these sources are in UTF-8 !!
// the LCD has a different charset: 0xE1 is 'ä'
static const char MONTH_DE[13][6] = {
   "Jan", "Feb", "Mär", "Apr", "Mai", "Jun",
   "Jul", "Aug", "Sep", "Okt", "Nov", "Dez", "???"
};
//static const char MONTH_EN[13][6] = {
//   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
//   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "???"
//};


namespace utils {

/**
 * Determine current date time with microseconds.
 */
DateTime::DateTime()
{
    struct timeval current;
    gettimeofday(&current, nullptr);

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
    time_t current = ::time(nullptr);
    struct tm localTime;
    localtime_r(&current, &localTime);

    // set dst in fileTime;
    memset(&time, 0, sizeof(time));
#ifdef __GNUC__
    // gm_toff is GNU library only, not C standard
    time.tm_gmtoff = localTime.tm_gmtoff;
#endif
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
DateTime::DateTime(const DateTime &other, int offset, OffsetType ot)
  : secondsSinceEpoch(other.secondsSinceEpoch)
  , microsecs(other.microsecs)
{
    if (ot == OffsetType::SECONDS)
        secondsSinceEpoch += offset;
    else if (ot == OffsetType::DAYS)
        secondsSinceEpoch += offset * (24 * 60 * 60);

    makeTime();

    if (ot == OffsetType::YEARS) {
        init(time.tm_year + offset, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
    }
    else if (ot == OffsetType::MONTH) {
        int yo = offset / 12;
        int mo = offset % 12;
        if (time.tm_mon + offset >= 13) {
            ++yo;
            mo -= 12;
        }
        else if (time.tm_mon + offset < 0) {
            --yo;
            mo += 12;
        }
        init(time.tm_year + yo, time.tm_mon + mo, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
    }
}

/**
 * Parses a date time out of the fixed format YYYY-MM-DD HH:MI:SS{.mmm}
 * This constructor also supports the JSON format YYYY-MM-DDTHH:MI:SS+nn:mm,
 * but nn:mm is ignored.
 */
DateTime::DateTime(const std::string &t)
  : secondsSinceEpoch(0)
  , microsecs(0)
{
    // set dst in fileTime;
    initTime(time);

    int year, mon, day, hour, min, sec, msec, count;

    if (t.find('T') == std::string::npos) {
        count = SSCANF(t.c_str(), "%d-%d-%d %d:%d:%d.%d", &year, &mon, &day, &hour, &min, &sec, &msec);
        if (count != 7 && count != 6) {
            // invalid format
            throw ValueFormatError("DateTime(\"" + t + "\"): invalid input format");
        }
    }
    else {
        // JSON format
        count = SSCANF(t.c_str(), "%d-%d-%dT%d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
        if (count != 6) {
            // invalid format
            throw ValueFormatError("DateTime(\"" + t + "\"): invalid input format");
        }
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
        throw Exception("DateTime::makeTime() failed call to localtime_r()");
    }
    time.tm_year += 1900;  // 2000 and higher
    time.tm_mon += 1;      // 1 .. 12
}


/**
 * Returns a month name with 3 chars
 */
const char* DateTime::monthName3() const
{
    Month mon = month();
    if (mon >= JAN && mon <= DEC)
        return MONTH_DE[mon - 1];
    throw Exception("Invalid month value: %d", static_cast<int>(mon));
}

/**
 * Returns a month name with 3 chars
 */
const char* DateTime::monthName3(Month mon)
{
    if (mon >= JAN && mon <= DEC)
        return MONTH_DE[mon - 1];
    throw Exception("Invalid month value: %d", static_cast<int>(mon));
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
 * Add the given number of seconds to the current date and recalculate
 * all date / time fields.
 */
void DateTime::roll(int seconds)
{
    secondsSinceEpoch += seconds;
    makeTime();
}

/**
 * Is this DateTime at the same day as other DateTime (ignore time)?
 */
bool DateTime::sameDayAs(const DateTime &other) const
{
    return day() == other.day() && month() == other.month() && year() == other.year();
}


/**
 * Returns a unified format in the way DD.MM.YYYY {HH:MI}
 * with out without hours and minutes.
 */
const std::string DateTime::asStringDMY(bool minutes) const
{
    char buffer[128];

    if (minutes)
        snprintf(buffer, sizeof(buffer), "%02d.%02d.%04d %02d:%02d",
                 time.tm_mday,
                 time.tm_mon,
                 time.tm_year,
                 time.tm_hour,
                 time.tm_min);
    else
        snprintf(buffer, sizeof(buffer), "%02d.%02d.%04d",
                 time.tm_mday,
                 time.tm_mon,
                 time.tm_year);

    buffer[sizeof(buffer) - 1] = 0;
    return buffer;
}


/**
 * Returns a unified format in the way YYYY-MM-DD HH:MI:SS{.mmm}
 * Milliseconds are only appended if 'millis = true'
 */
const std::string DateTime::asString(bool millis) const
{
    char buffer[128];

    if (millis)
        snprintf(buffer, sizeof(buffer), "%4d-%02d-%02d %02d:%02d:%02d.%03d",
                 time.tm_year,
                 time.tm_mon,
                 time.tm_mday,
                 time.tm_hour,
                 time.tm_min,
                 time.tm_sec,
                 microsecs % 1000);
    else
        snprintf(buffer, sizeof(buffer), "%4d-%02d-%02d %02d:%02d:%02d",
                 time.tm_year,
                 time.tm_mon,
                 time.tm_mday,
                 time.tm_hour,
                 time.tm_min,
                 time.tm_sec);

    buffer[sizeof(buffer) - 1] = 0;
    return buffer;
}

} /* namespace utils */
