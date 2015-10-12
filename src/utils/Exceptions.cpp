/*
 * Exceptions.cpp
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

#include <string.h>
#include <errno.h>

#include "Exceptions.h"
#include "StringUtil.h"

namespace utils {

//--------------------------------------------------------------------------
// utils::Exception
//--------------------------------------------------------------------------
Exception::Exception(const std::string &m)
  : msg(m)
{ }

Exception::Exception(const char *m)
  : msg(m)
{ }

Exception::Exception(const char *m, int arg)
  : msg()
{
    char buffer[1024];

    snprintf(buffer, sizeof(buffer), m, arg);
    buffer[sizeof(buffer) - 1] = 0;
    msg = buffer;
}

Exception::~Exception() NOEXCEPT { }

const char *Exception::what() const NOEXCEPT
{
    return msg.c_str();
}


//--------------------------------------------------------------------------
// utils::ConnectionLostException
//--------------------------------------------------------------------------
ConnectionLostException::ConnectionLostException(const std::string &m)
: Exception(m)
{ }

ConnectionLostException::~ConnectionLostException() NOEXCEPT { }


//--------------------------------------------------------------------------
// utils::FileOpenError
//--------------------------------------------------------------------------
FileOpenError::FileOpenError(bool read, const std::string &filename)
  : IOException(read ? "read" : "write", filename)
{ }

FileOpenError::FileOpenError(bool read, const char *filename)
  : IOException(read ? "read" : "write", filename)
{ }

FileOpenError::~FileOpenError() NOEXCEPT { }


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

KeyNotFound::~KeyNotFound() NOEXCEPT { }


//--------------------------------------------------------------------------
// utils::OutOfRange
//--------------------------------------------------------------------------
OutOfRange::OutOfRange(const std::string &hint, unsigned current, unsigned _max)
  : Exception()
{
    char buffer[256];
    sprintf(buffer, "%s '%u' out of range, max = %u",
            hint.length() > 0 && hint.length() < 150 ? "" : "Value", current, _max);
    setMessage(hint + buffer);
}

OutOfRange::OutOfRange(const std::string &hint, unsigned current, unsigned _min, unsigned _max)
  : Exception()
{
    char buffer[256];
    sprintf(buffer, "%s '%u' out of range, expected range is %u .. %u",
            hint.length() > 0 && hint.length() < 150 ? "" : "Value", current, _min, _max);
    setMessage(hint + buffer);
}

OutOfRange::~OutOfRange() NOEXCEPT { }

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

RuntimeError::~RuntimeError() NOEXCEPT { }

//--------------------------------------------------------------------------
// utils::ValueFormatError
//--------------------------------------------------------------------------
ValueFormatError::ValueFormatError(const std::string &m)
  : Exception(m)
{ }

ValueFormatError::~ValueFormatError() NOEXCEPT { }


//--------------------------------------------------------------------------
// utils::IOException
//--------------------------------------------------------------------------
IOException::IOException(const std::string &func, const std::string &args)
  : Exception(func + '(' + args + "): " + strings::strerror(errno))
{ }

IOException::IOException(int _errno, const std::string &func, const std::string &args)
  : Exception(func + '(' + args + "): " + strings::strerror(_errno))
{ }


IOException::~IOException() NOEXCEPT { }



} /* namespace utils */
