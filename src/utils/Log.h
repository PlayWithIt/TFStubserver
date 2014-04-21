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
#include <vector>
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

    static void printTime(std::ostream *os = NULL);
    static void saveAndPrintError(std::stringstream &os);

public:
    /** max error in the memory list */
    static const size_t MAX_ERRORS = 50;

    // simple text output
    static void log(const char* msg);
    static void log(const std::string& msg);

    // text and a number after the text
    static void log(const char* msg, int v);
    static void log(const std::string& msg, int v);
    static void log(const char* msg, uint64_t v);

    // log message with strerror() as parameter
    static void perror(const char* msg, int errorCode = errno);
    static void perror(const std::string& msg, int errorCode = errno);

    // log error message and put into error history
    static void error(const char* msg);
    static void error(const std::string& msg);

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

    // get the current output stream: can be used to directly log into the stream
    static std::ostream& getStream();

    // switch the logging stream
    static std::ostream& setStream(std::ostream& os);

};

} /* namespace utils */
#endif /* LOG_H_ */
