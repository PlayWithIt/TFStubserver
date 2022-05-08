/*
 * DateTime.h
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
#ifndef DATETIME_H_
#define DATETIME_H_

#include <ctime>
#include <chrono>
#include <string>

namespace utils {

/**
 * A simple class that should unify the different usages of date time with standard C
 * and newer C++ classes.
 * <b>
 * The class internally adds +1 to the month of the struct tm, be aware ...
 */
class DateTime {

    std::tm     time;
    std::time_t secondsSinceEpoch;
    unsigned    microsecs;

    /**
     * Split update the seconds in single fields.
     */
    void makeTime();

    /**
     * Init with single fields:<br>
     * year == 2000, month == 1 .. 12, dayOfMonth == 1..31, hour == 0..24
     */
    void init(unsigned year, unsigned month, unsigned dayOfMonth, unsigned hour = 0, unsigned minute = 0, unsigned second = 0);

public:
    /**
     * Determine current date time with microseconds.
     */
    explicit DateTime();

    /**
     * Parses a date time out of the fixed format YYYY-MM-DD HH:MI:SS{.mmm}.
     * This constructor also supports the JSON format YYYY-MM-DDTHH:MI:SS{+nn:mm},
     * but nn:mm is ignored.
     * @see #asString()
     */
    explicit DateTime(const std::string &t);

    /**
     * Use given date / time without microseconds.
     */
    explicit DateTime(const std::time_t t);

    /**
     * Use given date / time with microseconds.
     */
    explicit DateTime(const std::chrono::system_clock::time_point &now);

    /**
     * Create a DateTime with additional seconds: if negative the date is before
     * otherwise after the given DateTime.
     *
     * If secondOffset is for example 86400, then one day is added.
     */
    explicit DateTime(const DateTime &other, int secondOffset);

    /**
     * Init with single fields:<br>
     * year == 2000, month == 1 .. 12, dayOfMonth == 1..31, hour == 0..24
     */
    DateTime(unsigned year, unsigned month, unsigned dayOfMonth, unsigned hour = 0, unsigned minute = 0, unsigned second = 0);

    /**
     * Copy constructor and assignment
     */
    DateTime(const DateTime &other) {
        *this = other;
    }

    DateTime& operator=(const DateTime &other);

    /**
     * Available month values
     */
    enum Month {
        JAN = 1,
        FEB = 2,
        MAR = 3,
        APR = 4,
        MAY = 5,
        JUN = 6,
        JUL = 7,
        AUG = 8,
        SEP = 9,
        OCT = 10,
        NOV = 11,
        DEC = 12
    };

    enum Day {
        SUNDAY    = 0,
        MONDAY    = 1,
        TUESDAY   = 2,
        WEDNESDAY = 3,
        THURSDAY  = 4,
        FRIDAY    = 5,
        SATURDAY  = 6
    };

    /**
     * Returns the year e.g. 2014.
     */
    unsigned year() const {
        return time.tm_year;
    }

    /**
     * Return the month 1 .. 12.
     * (tm_mon is increased internally).
     */
    Month month() const {
        return static_cast<Month>(time.tm_mon);
    }

    /**
     * Returns the day in month 1 .. 31
     */
    unsigned day() const {
        return time.tm_mday;
    }

    /**
     * Returns the hour of the day 0 .. 23
     */
    unsigned hour() const {
        return time.tm_hour;
    }

    /**
     * Returns the minute in the hour 0 .. 59
     */
    unsigned minute() const {
        return time.tm_min;
    }

    /**
     * Returns the seconds in the minute 0 .. 59
     */
    unsigned second() const {
        return time.tm_sec;
    }

    /**
     * Daylight saving time active ?
     */
    bool dst() const {
        return time.tm_isdst != 0;
    }

    /**
     * Return day of week, 0 == sunday.
     */
    Day weekDay() const {
        return static_cast<Day>(time.tm_wday);
    }

    /**
     * Return the "seconds-since-epoch" value
     */
    std::time_t getTime() const {
        return secondsSinceEpoch;
    }

    /**
     * Return the current millisecond
     */
    unsigned millis() const {
        return microsecs / 1000;
    }

    /**
     * Return the current microsecond within the second
     */
    unsigned micros() const {
        return microsecs;
    }

    /**
     * Add the given number of seconds to the current date and recalculate
     * all date / time fields.
     */
    void roll(int seconds);

    DateTime& operator+=(int seconds) {
        roll(seconds);
        return *this;
    }


    /**
     * Is this DateTime before another DateTime (microseconds included!).
     */
    bool before(const DateTime &other) const;

    bool operator<(const DateTime &other) const {
        return this->before(other);
    }

    /**
     * Is this DateTime after another DateTime (microseconds included!).
     */
    bool after(const DateTime &other) const;

    bool operator>(const DateTime &other) const {
        return this->after(other);
    }

    /**
     * Is this DateTime at the same as other DateTime (ignore time)?
     */
    bool sameDayAs(const DateTime &other) const;

    /**
     * Is the DateTime the same as another?
     * Be aware: this also compares the microseconds.
     */
    bool operator==(const DateTime &other) const {
        return secondsSinceEpoch == other.secondsSinceEpoch && microsecs == other.microsecs;
    }

    /**
     * Is the DateTime not equals to another?
     * Be aware: this also compares the microseconds.
     */
    bool operator!=(const DateTime &other) const {
        return secondsSinceEpoch != other.secondsSinceEpoch || microsecs != other.microsecs;
    }

    /**
     * Returns a unified format in the way DD.MM.YYYY {HH:MI}
     * with out without hours and minutes.
     */
    const std::string asStringDMY(bool minutes = false) const;

    /**
     * Returns a unified format in the way YYYY-MM-DD HH:MI:SS{.mmm}
     * Milliseconds are only appended if 'millis = true'
     */
    const std::string asString(bool millis = false) const;
};

} /* namespace utils */

#endif /* DATETIME_H_ */
