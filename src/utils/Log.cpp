/*
 * Log.cpp
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

#include <cxxabi.h>

#include <sys/time.h>
#include <iomanip>
#include <sstream>
#include <cstring>

#include "Mutexes.h"
#include "Log.h"

static std::ostream* logStream = &std::cerr;
static std::mutex    logMutex;

static std::vector<std::string> errors;


namespace utils {

Log::Log() {
}

Log::~Log() {
}


// log message with strerror() as parameter
void Log::perror(const char* msg, int errorCode)
{
    MutexLock lock(logMutex);

    std::stringstream os;
    printTime(&os);

    const char *c = msg;
    char ch = *c;
    bool consumed = false;

    while (ch)
    {
        if (ch == '%')
        {
            if (*(c+1) == 's') {
                os << strerror(errorCode);
                consumed = true;
            }
            else {
                os << ch;
            }
        }
        else {
            os << ch;
        }
        ++c;
        ch = *c;
    }
    if (!consumed) {
        os << ": " << strerror(errorCode);
    }
    saveAndPrintError(os);
}

void Log::perror(const std::string& msg, int errorCode)
{
    Log::perror(msg.c_str(), errorCode);
}

/**
 * Just start the log line with current date + time.
 */
void Log::printTime(std::ostream *os)
{
    struct timeval current;
    gettimeofday(&current, NULL);
    std::tm* time = localtime( &(current.tv_sec) );

    if (!os)
        os = logStream;

    *os << (time->tm_year + 1900) << '-'
        << std::setfill('0')
        << std::setw(2) << (time->tm_mon + 1) << '-'
        << std::setw(2) << time->tm_mday << ' '
        << std::setw(2) << time->tm_hour << ':'
        << std::setw(2) << time->tm_min << ':'
        << std::setw(2) << time->tm_sec << '.'
        << std::setw(3) << (current.tv_usec / 1000) << "  ";
}

/**
 * Simple message output.
 */
void Log::log(const char* msg)
{
    MutexLock lock(logMutex);
    printTime();
    *logStream << msg << '\n' << std::flush;
}

void Log::log(const std::string& msg)
{
    log(msg.c_str());
}


/**
 * Simple message output with a number behind.
 */
void Log::log(const char* msg, int v)
{
    MutexLock lock(logMutex);
    printTime();
    *logStream << msg << ' ' << v << '\n' << std::flush;
}

void Log::log(const char* msg, uint64_t v)
{
    MutexLock lock(logMutex);
    printTime();
    *logStream << msg << ' ' << v << '\n' << std::flush;
}

void Log::log(const std::string& msg, int v)
{
    log(msg.c_str(), v);
}

void Log::error(const char* msg)
{
    MutexLock lock(logMutex);
    std::stringstream os;
    printTime(&os);
    os << msg;
    saveAndPrintError(os);
}

void Log::error(const std::string& msg)
{
    error(msg.c_str());
}

/**
 * Message output with an exception behind.
 */
void Log::error(const char* msg, const std::exception &ex)
{
    MutexLock lock(logMutex);

    std::stringstream os;
    printTime(&os);

    int status = 0;
    char *realname = abi::__cxa_demangle(typeid(ex).name(), 0, 0, &status);

    os << "Caught exception of type '"
       << (status == 0 ? realname : typeid(ex).name())
       << "' in " << msg << ": " << ex.what();
    saveAndPrintError(os);

    if (realname && status == 0)
        free(realname);
}

void Log::error(const std::string& msg, const std::exception &ex)
{
    error(msg.c_str(), ex);
}

/**
 * Read the error messages that are in the list.
 *
 * @param errorList if the parameter is non-NULL, the messages are moved
 *        into this list and the internal list is cleared. If the value
 *        is NULL, only the number of items is returned.
 * @return the number of messages in the list
 */
size_t Log::getErrorHistory(std::vector<std::string> *errorList)
{
    size_t num = errors.size();

    if (errorList) {
        *errorList = std::move(errors);
    }

    return num;
}

/**
 * Stream operators.
 */
std::ostream& Log::getStream()
{
    MutexLock lock(logMutex);
    printTime();
    return *logStream;
}

/**
 * Append the error to the internal error list and dump into the 'logStream'.
 */
void Log::saveAndPrintError(std::stringstream &os)
{
    std::string s = os.str();
    *logStream << s << '\n' << std::flush;
    errors.push_back(std::move(s));

    // shrink the list
    while (errors.size() > MAX_ERRORS)
        errors.erase(errors.begin());
}

std::ostream& Log::setStream(std::ostream& os)
{
    std::ostream& old = *logStream;
    logStream = &os;
    return old;
}

} /* namespace utils */
