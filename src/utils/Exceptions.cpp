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

Exception::~Exception() noexcept { }

const char *Exception::what() const noexcept
{
    return msg.c_str();
}


//--------------------------------------------------------------------------
// utils::ConnectionLostException
//--------------------------------------------------------------------------
ConnectionLostException::ConnectionLostException(const std::string &m)
: Exception(m)
{ }

ConnectionLostException::~ConnectionLostException() noexcept { }


//--------------------------------------------------------------------------
// utils::FileOpenError
//--------------------------------------------------------------------------
FileOpenError::FileOpenError(bool read, const std::string &filename)
{
    std::string m(read ? "Error reading '" : "Error writing '");
    m = m + filename + "': " + strerror(errno);
    setMessage(m);
}

FileOpenError::FileOpenError(bool read, const char *filename)
{
    std::string m(read ? "Error reading '" : "Error writing '");
    m = m + filename + "': " + strerror(errno);
    setMessage(m);
}

FileOpenError::~FileOpenError() noexcept { }


//--------------------------------------------------------------------------
// utils::KeyNotFound
//--------------------------------------------------------------------------
KeyNotFound::KeyNotFound(const std::string &key)
  : Exception(std::string("Key '") + key + "' not found")
{ }

KeyNotFound::~KeyNotFound() noexcept { }


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
    sprintf(buffer, "%s' %u' out of range, expected range is %u .. %u",
            hint.length() > 0 && hint.length() < 150 ? "" : "Value", current, _min, _max);
    setMessage(hint + buffer);
}

OutOfRange::~OutOfRange() noexcept { }


//--------------------------------------------------------------------------
// utils::ValueFormatError
//--------------------------------------------------------------------------
ValueFormatError::ValueFormatError(const std::string &m)
  : Exception(m)
{ }

ValueFormatError::~ValueFormatError() noexcept { }



} /* namespace utils */
