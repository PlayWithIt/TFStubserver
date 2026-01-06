/*
 * Log.cpp
 *
 * Copyright (C) 2013-2023 Holger Grosenick
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
#include "CyclicVector.h"
#include "DateTime.h"
#include "DateTimeFormat.h"
#include "ProcessInformation.h"
#include "StringUtil.h"


namespace utils {

static std::ostream* logStream = &std::cerr;
static std::mutex    logMutex;

const unsigned Log::MAX_ERRORS = 50;      // 50
const unsigned Log::MAX_MESSAGES = 500;    // 500

static utils::DateTimeFormat format(utils::DateTimeFormat::LOG_FORMAT_DATE_TIME_MICROS);
static utils::CyclicVector<std::string> errors(utils::Log::MAX_ERRORS);
static utils::CyclicVector<std::string> messages(utils::Log::MAX_MESSAGES);



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
        flush();
    }
}


/**
 * Log message with strerror(errno), returns the errorCode which was the input.
 * A '%e' in the message is replaced with the error message for errno.
 * A '%f' in the message is replaced with the function value.
 * If these are not present, they'll appears at the end.
 *
 * So the message text by default is:
 * <msg> <function>: <strerror(errno)>
 */
int Log::perror(const char* msg, const char* function, int errorCode)
{
    MutexLock lock(logMutex);

    std::ostringstream os;
    printTime(&os);

    const char *c = msg;
    char ch = *c;
    bool consumedE = false;
    bool consumedF = false;

    while (ch)
    {
        if (ch == '%')
        {
            if (*(c+1) == 'e') {
                if (errorCode > 0 && errno < 200)
                    os << strings::strerror(errorCode) << ", ";
                os << "errorCode=" << errorCode;
                consumedE = true;
                ++c;
            }
            else if (*(c+1) == 'f') {
                os << function;
                consumedF = true;
                ++c;
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
    if (!consumedF && function) {
        os << ' ' << function;
    }
    if (!consumedE) {
        os << ": ";
        if (errorCode > 0 && errno < 200)
            os << strings::strerror(errorCode) << ", ";
        os << "errorCode=" << errorCode;
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
    std::ostringstream os;
    printTime(&os);
    os << msg;
    saveAndPrintMessage(os);
}

/**
 * Simple message output with a number behind.
 */
void Log::log(const char* msg, int v)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg << ' ' << v;
    saveAndPrintMessage(os);
}

void Log::log(const char* msg, unsigned v)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg << ' ' << v;
    saveAndPrintMessage(os);
}

void Log::log(const char* msg, double v)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg << ' ' << std::fixed << std::setprecision(2) << v;
    saveAndPrintMessage(os);
}

void Log::log(const char* msg, uint64_t v)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg << ' ' << v;
    saveAndPrintMessage(os);
}

void Log::log(const char* msg, const char *arg)
{
    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << msg << ' ' << (arg ? arg : "NULL");
    saveAndPrintMessage(os);
}

void Log::log(const std::string& msg, const char *v)
{
    log(msg.c_str(), v);
}

void Log::log(const std::string& msg, double v)
{
    log(msg.c_str(), v);
}

void Log::log(const std::string& msg, int v)
{
    log(msg.c_str(), v);
}

void Log::log(const std::string& msg, unsigned v)
{
    log(msg.c_str(), v);
}

void Log::log(const std::string& msg, uint64_t v)
{
    log(msg.c_str(), v);
}

/**
 * Print information about the current process into the log stream.
 */
void Log::logProcessInformation()
{
    ProcessInformation pi;

    MutexLock lock(logMutex);
    std::ostringstream os;
    printTime(&os);
    os << "Current process information:\n" << pi;
    saveAndPrintMessage(os);
}

/**
 * Log the content of the internal buffer, if there is something not logged yet.
 */
void Log::flush()
{
    if (logType == LogType::ERROR)
        error(os.str());
    else
        log(os.str());
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
 * @param errorList  the messages are moved into this list and the internal list is cleared.
 * @return the number of messages in the list
 */
unsigned Log::getAndClearErrors(std::vector<std::string> &errorList)
{
    MutexLock lock(logMutex);

    for (auto& it : errors)
        errorList.push_back(it);
    errors.clear();

    return errorList.size();
}

/**
 * Read the messages that are in the list (including errors).
 *
 * @param msgList mandatory output parameter, messages are copied into this list.
 * @return the number of messages in the list
 */
unsigned Log::getMessages(std::vector<std::string> &msgList)
{
    MutexLock lock(logMutex);

    for (auto& it : messages)
        msgList.push_back(it);
    return msgList.size();
}

/**
 * Returns true if at least one error is still in the error history list.
 * A call to 'getErrorHistory' should return at least one items then.
 */
bool Log::hasErrors()
{
    return !errors.empty();
}

/**
 * Append the error to the internal error list and dump into the 'logStream'.
 */
void Log::saveAndPrintError(std::ostringstream &os)
{
    std::string s = os.str();
    *logStream << s << '\n' << std::flush;
    messages.push_back(s);
    errors.push_back(std::move(s));
}

/**
 * Append the message to the internal message list and dump into the 'logStream'.
 */
void Log::saveAndPrintMessage(std::ostringstream &os)
{
    std::string s = os.str();
    *logStream << s << '\n' << std::flush;
    messages.push_back(std::move(s));
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
        flush();
        os.str("");
        os.clear();
    }
    else if (c != 13)
        os << c;
    return *this;
}

} /* namespace utils */
