/*
 * Exceptions.cpp
 *
 * Copyright (C) 2013-2022 Holger Grosenick
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

#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "Exceptions.h"
#include "StringUtil.h"

namespace utils {

// max message length for exceptions
#define __MSG_LEN 1024

//--------------------------------------------------------------------------
// utils::Exception
//--------------------------------------------------------------------------
Exception::Exception(const std::string &m)
  : msg(m)
{ }

Exception::Exception(const char *m, ...)
{
    va_list args;
    va_start(args, m);
    setMessage(m, args);
    va_end(args);
}

Exception::~Exception() { }


/**
 * set message via printf style format
 */
void Exception::setMessage(const char *m, va_list args)
{
    char buffer[__MSG_LEN];

    ::vsnprintf(buffer, sizeof(buffer), m, args);
    buffer[sizeof(buffer) - 1] = 0;
    msg = buffer;
}

const char *Exception::what() const noexcept
{
    return msg.c_str();
}


//--------------------------------------------------------------------------
// utils::FileOpenError
//--------------------------------------------------------------------------
FileOpenError::FileOpenError(bool read, const std::string &filename)
  : IOException(read ? "read" : "write", filename)
{ }

FileOpenError::FileOpenError(bool read, const char *filename)
  : IOException(read ? "read" : "write", filename)
{ }


//--------------------------------------------------------------------------
// utils::KeyNotFound
//--------------------------------------------------------------------------
static const std::string not_found("' not found");

KeyNotFound::KeyNotFound(const std::string &key)
  : Exception(std::string("Key '") + key + not_found)
{ }

KeyNotFound::KeyNotFound(const std::string &messagePrefix, const std::string &key)
  : Exception(messagePrefix + std::string(" '") + key + not_found)
{ }


//--------------------------------------------------------------------------
// utils::OutOfRange
//--------------------------------------------------------------------------
OutOfRange::OutOfRange(const std::string &hint, unsigned current, unsigned _max)
  : Exception()
{
    char buffer[__MSG_LEN];
    ::snprintf(buffer, sizeof(buffer), "%s: %u is out of range, max = %u",
               hint.length() > 0 && hint.length() < 150 ? "" : "Value", current, _max);
    buffer[sizeof(buffer) - 1] = 0;
    setMessage(hint + buffer);
}

OutOfRange::OutOfRange(const std::string &hint, unsigned current, unsigned _min, unsigned _max)
  : Exception()
{
    char buffer[__MSG_LEN];
    ::snprintf(buffer, sizeof(buffer), "%s: %u is out of range, expected range is %u .. %u",
               hint.length() > 0 && hint.length() < 150 ? "" : "Value", current, _min, _max);
    buffer[sizeof(buffer) - 1] = 0;
    setMessage(hint + buffer);
}


//--------------------------------------------------------------------------
// utils::RuntimeError
//--------------------------------------------------------------------------
RuntimeError::RuntimeError(const std::string &msg)
{
    setMessage(msg + ": " + strings::strerror(errno));
}

RuntimeError::RuntimeError(const char *msg)
{
    setMessage(std::string(msg) + ": " + strings::strerror(errno));
}

RuntimeError::RuntimeError(const char *msg, int _errno)
{
    setMessage(std::string(msg) + ": " + strings::strerror(_errno));
}

//--------------------------------------------------------------------------
// utils::ValueFormatError
//--------------------------------------------------------------------------
ValueFormatError::ValueFormatError(const char *m, ...)
{
    va_list args;
    va_start(args, m);
    setMessage(m, args);
    va_end(args);
}

//--------------------------------------------------------------------------
// utils::IOException
//--------------------------------------------------------------------------
IOException::IOException(const std::string &func, const std::string &args)
  : Exception(func + '(' + args + "): " + strings::strerror(errno))
{ }

IOException::IOException(int _errno, const std::string &func, const std::string &args)
  : Exception(func + '(' + args + "): " + strings::strerror(_errno))
{ }


} /* namespace utils */
