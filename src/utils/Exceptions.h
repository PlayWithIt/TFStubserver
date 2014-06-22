/*
 * Exceptions.h
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

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <string>
#include <exception>


namespace utils {


/**
 * Base exception class which holds a message text: this is returned
 * within the what() method.
 */
class Exception : public std::exception
{
    std::string msg;

protected:
    Exception() {  }

    void setMessage(const std::string &m) {
        msg = m;
    }

    void setMessage(const char *m) {
        msg = m;
    }

public:
    explicit Exception(const std::string &msg);
    explicit Exception(const char *msg);
    explicit Exception(const char *msg, int arg);

    // std::~exception is already virtual ...
    virtual ~Exception() noexcept;

    const char *what() const noexcept;
};


/**
 * Exception thrown if the connection to the stack or a socket seems to be lost.
 */
class ConnectionLostException : public Exception
{
public:
    explicit ConnectionLostException(const std::string &msg);
    ~ConnectionLostException() noexcept;
};


/**
 * Exception thrown if a file cannot be opened: it adds the last errno
 * message text.
 */
class FileOpenError : public Exception
{
public:
    /**
     * Error in case of file open: if 'read' is true it was an open for
     * reading, otherwise for writing.
     */
    FileOpenError(bool read, const std::string &filename);

    /**
     * Error in case of file open: if 'read' is true it was an open for
     * reading, otherwise for writing.
     */
    FileOpenError(bool read, const char *filename);
    ~FileOpenError() noexcept;
};


/**
 * Exception thrown if a value has not the expected format.
 */
class ValueFormatError : public Exception
{
public:
    explicit ValueFormatError(const std::string &msg);
    ~ValueFormatError() noexcept;
};


/**
 * Exception thrown if key was not found.
 */
class KeyNotFound : public Exception
{
public:
    explicit KeyNotFound(const std::string &key);
    ~KeyNotFound() noexcept;
};


/**
 * Exception thrown if a value is not in the expected range, different
 * constructors are offered in order to build a defined error string.
 */
class OutOfRange : public Exception
{
public:
    explicit OutOfRange(const std::string &msg)
      : Exception(msg) { }

    explicit OutOfRange(const std::string &hint, unsigned current, unsigned _max);
    explicit OutOfRange(const std::string &hint, unsigned current, unsigned _min, unsigned _max);

    ~OutOfRange() noexcept;
};

} /* namespace utils */
#endif /* EXCEPTIONS_H_ */
