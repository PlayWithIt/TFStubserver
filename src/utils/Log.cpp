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

#ifndef _WIN32
#include <cxxabi.h>
#endif

#include <iomanip>

#include "Mutexes.h"
#include "Log.h"
#include "DateTime.h"
#include "DateTimeFormat.h"
#include "StringUtil.h"
#include "CyclicVector.h"

static std::ostream* logStream = &std::cerr;
static std::mutex    logMutex;

static utils::DateTimeFormat format(utils::DateTimeFormat::LOG_FORMAT_DATE_TIME_MICROS);
static utils::CyclicVector<std::string> errors(utils::Log::MAX_ERRORS);


namespace utils {

Log::Log(unsigned precision, LogType t) : logType(t)
{
    os << std::fixed << std::setprecision(precision);
    if (t == LogType::ERROR)
        os << "ERROR: ";
    if (t == LogType::FATAL)
        os << "FATAL: ";
}

/**
 * Constructor for a temporary Log-record which prints the log data if
 * a newline appears in the input.
 */
Log::Log(LogType t) : logType(t)
{
    if (t == LogType::ERROR)
        os << "ERROR: ";
    if (t == LogType::FATAL)
        os << "FATAL: ";
}

Log::~Log()
{
    if (os.tellp() > 0)
    {
        // Still some data left?
        log(*this);
    }
}


// log message with strerror() as parameter
int Log::perror(const char* msg, int errorCode)
{
    MutexLock lock(logMutex);

    std::ostringstream os;
    printTime(&os);

    const char *c = msg;
    char ch = *c;
    bool consumed = false;

    while (ch)
    {
        if (ch == '%')
        {
            if (*(c+1) == 's') {
                os << strings::strerror(errorCode);
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
        os << ": " << strings::strerror(errorCode);
    }
    saveAndPrintError(os);
    return errorCode;
}

int Log::perror(const std::string& msg, int errorCode)
{
    return Log::perror(msg.c_str(), errorCode);
}

/**
 * Just start the log line with current date + time.
 */
void Log::printTime(std::ostream *os)
{
    DateTime current;  // get actual time
    if (!os)
        os = logStream;
    *os << format << current << "  ";
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

void Log::log(const char* msg, const char *arg)
{
    MutexLock lock(logMutex);
    printTime();
    *logStream << msg << ' ' << (arg ? arg : "NULL") << '\n' << std::flush;
}

void Log::log(const std::string& msg, const char *v)
{
    log(msg.c_str(), v);
}

void Log::log(const std::string& msg, int v)
{
    log(msg.c_str(), v);
}

void Log::log(const Log& _log)
{
    if (_log.logType == ERROR)
        Log::error(_log.os.str());
    else
        Log::log(_log.os.str());
}

void Log::error(const char* msg)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg;
    saveAndPrintError(os);
}

void Log::error(const std::string &msg, const char* arg)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg << ' ' << arg;
    saveAndPrintError(os);
}

void Log::error(const std::string &msg, int arg)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg << ' ' << arg;
    saveAndPrintError(os);
}

/**
 * Message output with an exception behind.
 */
void Log::error(const char* msg, const std::exception &ex)
{
    MutexLock lock(logMutex);

    std::ostringstream os;
    printTime(&os);

#ifdef _WIN32
    os << "Exception of type '" << typeid(ex).name() << "'";
#else
    int status = 0;
    char *realname = abi::__cxa_demangle(typeid(ex).name(), 0, 0, &status);

    if (status == 0 && realname) {
        // exception name is readable
        os << realname;
        // see description of __cxa_demangle: must be freed
        free(realname);
    }
    else {
        os << "Exception of type '" << typeid(ex).name() << "'";
    }

#endif

    os << " in " << msg << ": " << ex.what();
    saveAndPrintError(os);
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
        for (auto& it : errors)
            errorList->push_back(it);
        errors.clear();
    }

    return num;
}

/**
 * Append the error to the internal error list and dump into the 'logStream'.
 */
void Log::saveAndPrintError(std::ostringstream &os)
{
    std::string s = os.str();
    *logStream << s << '\n' << std::flush;
    errors.push_back(std::move(s));
}

std::ostream& Log::setStream(std::ostream& os)
{
    std::ostream& old = *logStream;
    logStream = &os;
    return old;
}

/**
 * Stream methods: print data into internal buffer and flush at '\n'
 */
Log& Log::operator<<(const char *s)
{
    while (*s) {
        *this << *s;
        ++s;
    }
    return *this;
}

Log& Log::operator<<(const char c)
{
    if (c == 10) {
        log(*this);
        os.str("");
        os.clear();
    }
    else if (c != 13)
        os << c;
    return *this;
}

} /* namespace utils */
