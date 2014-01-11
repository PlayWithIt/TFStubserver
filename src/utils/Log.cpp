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
#include <cstring>

#include "Mutexes.h"
#include "Log.h"


static std::ostream* logStream = &std::cerr;
static std::mutex    logMutex;

namespace utils {

Log::Log() {
}

Log::~Log() {
}


// log message with strerror() as parameter
void Log::perror(const char* msg, int errorCode)
{
    MutexLock lock(logMutex);
    printTime();

    const char *c = msg;
    char ch = *c;
    bool consumed = false;

    while (ch)
    {
        if (ch == '%')
        {
            if (*(c+1) == 's') {
                *logStream << strerror(errorCode);
                consumed = true;
            }
            else {
                *logStream << ch;
            }
        }
        else {
            *logStream << ch;
        }
        ++c;
        ch = *c;
    }
    if (!consumed) {
        *logStream << ": " << strerror(errorCode);
    }
    *logStream << '\n' << std::flush;
}

void Log::perror(const std::string& msg, int errorCode)
{
    Log::perror(msg.c_str(), errorCode);
}

/**
 * Just start the log line with current date + time.
 */
void Log::printTime()
{
    struct timeval current;
    gettimeofday(&current, NULL);
    std::tm* time = localtime( &(current.tv_sec) );

    *logStream << (time->tm_year + 1900) << '-'
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

/**
 * Message output with an exception behind.
 */
void Log::log(const char* msg, const std::exception &ex)
{
    MutexLock lock(logMutex);

    printTime();
    int status = 0;
    char *realname = abi::__cxa_demangle(typeid(ex).name(), 0, 0, &status);

    *logStream << "Caught exception of type '"
               << (status == 0 ? realname : typeid(ex).name())
               << "' in " << msg << ": " << ex.what() << '\n' << std::flush;

    if (realname && status == 0)
        free(realname);
}

void Log::log(const std::string& msg, const std::exception &ex)
{
    log(msg.c_str(), ex);
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

std::ostream& Log::setStream(std::ostream& os)
{
    std::ostream& old = *logStream;
    logStream = &os;
    return old;
}

} /* namespace utils */
