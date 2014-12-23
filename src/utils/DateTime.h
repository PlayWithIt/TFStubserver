/*
 * DateTime.h
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
#ifndef DATETIME_H_
#define DATETIME_H_

#include <ctime>
#include <chrono>

namespace utils {

/**
 * A simple class that should unify the different usages of date time with standard C
 * and newer C++ classes.
 */
class DateTime {

    std::tm     time;
    std::time_t secondsSinceEpoch;
    unsigned    microsecs;

    /**
     * Split update the seconds in single fields.
     */
    void makeTime();

public:
    /**
     * Determine current date time with microseconds.
     */
    DateTime();

    /**
     * Use given date / time without microseconds.
     */
    explicit DateTime(const std::time_t t);

    /**
     * Use given date / time with microseconds.
     */
    explicit DateTime(const std::chrono::system_clock::time_point &now);

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
        SUNDAY = 0,
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY
    };

    /**
     * Returns the year e.g. 2014.
     */
    unsigned year() const {
        return time.tm_year;
    }

    /**
     * Return the month 1 .. 12.
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
     * Is the DateTime the same as another?
     */
    bool operator==(const DateTime &other) const {
        return secondsSinceEpoch == other.secondsSinceEpoch && microsecs == other.microsecs;
    }

    /**
     * Is the DateTime not equals to another?
     */
    bool operator!=(const DateTime &other) const {
        return secondsSinceEpoch != other.secondsSinceEpoch || microsecs != other.microsecs;
    }
};

} /* namespace utils */

#endif /* DATETIME_H_ */
