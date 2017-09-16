/*
 * Log.h
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

#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <errno.h>

// on VS 2015 'ERROR' seems to be a #define
#ifdef ERROR
#undef ERROR
#endif

namespace utils {

/**
 * Simple log to stderr which puts a date/time in front of each line and
 * each call to 'log' start a new line.
 */
class Log {

public:
    enum LogType {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    /**
     * Constructor for a temporary Log-record which prints the log data if
     * a newline appears in the input.
     */
    Log(LogType t = INFO);

    /**
     * Constructor for a temporary Log-record which prints the log data if
     * a newline appears in the input, if any double value is printed into
     * this stream, 'fixed' format will be used with the given number of decimals.
     */
    Log(unsigned precision, LogType t = INFO);
    ~Log();

    /** max error in the memory list */
    static const size_t MAX_ERRORS = 50;

    Log& operator<<(const char *);
    Log& operator<<(const char);

    Log& operator<<(const std::string& s) {
        *this << s.c_str();
        return *this;
    }

    template<typename _Tp> Log& operator<<(const _Tp& v) {
        os << v;
        return *this;
    }

    // simple text output which avoids creating temporary objects
    static void log(const char* msg);
    static void log(const std::string& msg) {
        log(msg.c_str());
    }

    // text and a number after the text
    static void log(const char* msg, int v);
    static void log(const std::string& msg, int v);
    static void log(const char* msg, uint64_t v);
    static void log(const char* msg, const char *arg);
    static void log(const std::string& msg, const char *arg);
    static void log(const Log& _log);

    /**
     * Log message with strerror() as additional text, returns the errorCode which was the input.
     */
    static int perror(const char* msg, int errorCode = errno);

    /**
     * Log message with strerror() as additional text, returns the errorCode which was the input.
     */
    static int perror(const std::string& msg, int errorCode = errno);

    // log error message and put into error history
    static void error(const char* msg);
    static void error(const std::string& msg) {
        error(msg.c_str());
    }
    static void error(const std::string& msg, const char *arg);
    static void error(const std::string& msg, int arg);

    // exception trace
    static void error(const char* msg, const std::exception &ex);
    static void error(const std::string& func, const std::exception &ex);

    /**
     * Read the error messages that are in the list.
     *
     * @param errorList if the parameter is non-NULL, the messages are moved
     *        into this list and the internal list is cleared. If the value
     *        is NULL, only the number of items is returned.
     * @return the number of messages in the list
     */
    static size_t getErrorHistory(std::vector<std::string> *errorList = NULL);

    // switch the logging stream
    static std::ostream& setStream(std::ostream& os);

private:
    std::ostringstream os;
    LogType logType;

    // no copy
    Log(const Log &other) = delete;
    Log& operator=(const Log &other) = delete;

    static void saveAndPrintError(std::ostringstream &os);

    // print date time and microseconds to the stream
    static void printTime(std::ostream *os = NULL);

};

} /* namespace utils */
#endif /* LOG_H_ */
