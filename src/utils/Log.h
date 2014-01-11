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

#include <string>
#include <iostream>
#include <errno.h>


namespace utils {

/**
 * Simple log to stderr which puts a date/time in front of each line and
 * each call to 'log' start a new line.
 */
class Log {

    Log();
    Log(const Log &other) = delete;
    ~Log();

    static void printTime();

public:

    // simple text output
    static void log(const char* msg);
    static void log(const std::string& msg);

    // text and a number after the text
    static void log(const char* msg, int v);
    static void log(const std::string& msg, int v);
    static void log(const char* msg, uint64_t v);

    // exception trace
    static void log(const char* msg, const std::exception &ex);
    static void log(const std::string& func, const std::exception &ex);

    // log message with strerror() as parameter
    static void perror(const char* msg, int errorCode = errno);
    static void perror(const std::string& msg, int errorCode = errno);

    // get the current output stream: can be used to directly log into the stream
    static std::ostream& getStream();

    // switch the logging stream
    static std::ostream& setStream(std::ostream& os);

};

} /* namespace utils */
#endif /* LOG_H_ */
