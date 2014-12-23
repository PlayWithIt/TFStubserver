/*
 * HandleStreambuf.cpp
 *
 * Copyright (C) 2014 Holger Grosenick
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

#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "Exceptions.h"
#include "File.h"
#include "HandleStreambuf.h"

namespace utils {


HandleStreambuf::HandleStreambuf(int _handle, bool _input, unsigned _bufSize)
  : buffer(NULL)
  , handle(_handle)
  , bufSize(_bufSize)
  , dataSize(0)
  , input(_input)
{
    if (handle < 0)
        throw std::invalid_argument("Handle may not be negative");
    if (bufSize < 8)
        throw std::invalid_argument("bufSize may not be smaller than 8");
    buffer = new char[bufSize];

    setg(buffer, buffer, buffer);
    if (input)
    {
        // just set a flag that first underflow() really reads data
        // we may not read at construction time yet: bad when pipes are involved
        dataSize = 1;
        setp(buffer, buffer);
    }
    else
        setp(buffer, buffer + bufSize);
}

HandleStreambuf::~HandleStreambuf()
{
    close();
    delete[] buffer;
}

/**
 * Close the handle, this is done automatically in dtor.
 */
void HandleStreambuf::close()
{
    sync();
    if (handle >= 0)
        ::close(handle);
    handle = -1;
}

/**
 * Fill buffer again
 */
HandleStreambuf::int_type HandleStreambuf::underflow()
{
    if (_M_in_cur == _M_in_end)
    {
        if (!input)
            throw std::logic_error("try to read from an output buffer");

        // read next block, if we have reached EOF once (dateSize == 0), never read() again
        if (dataSize > 0)
        {
            // already everything read into memory?
            if (handle > 0)
                dataSize = read(handle, buffer, bufSize);
            else
                dataSize = 0;
        }
        if (dataSize == 0)
        {
            // end of file
            setg(buffer, buffer, buffer);
            return std::basic_streambuf<char>::traits_type::eof();
        }
        if (dataSize < 0)
        {
            throw std::runtime_error("read() returned negative value");
        }
        setg(buffer, buffer, buffer + dataSize);
    }
    return *_M_in_cur;
}

/**
 * Write buffer to disk
 */
HandleStreambuf::int_type HandleStreambuf::overflow(HandleStreambuf::int_type __c)
{
    // flush buffer
    sync();

    // flush last char
    if (__c  != HandleStreambuf::traits_type::eof() && !input)
    {
        buffer[0] = __c;
        if (1 != write(handle, buffer, sizeof(char)))
            throw std::runtime_error("write(__c) returned invalid number of bytes");
    }

    return __c;
}

int HandleStreambuf::sync()
{
    if (input)
        return 0;

    ssize_t s = _M_out_cur - _M_out_beg;
    if (s != 0)
    {
        ssize_t rc = write(handle, buffer, s);
        if (rc != s)
            throw std::runtime_error("write() returned invalid number of bytes");
        setp(buffer, buffer + bufSize);
    }
    return 0;
}

/**
 * Number of bytes in buffer.
 */
std::streamsize HandleStreambuf::showmanyc()
{
    return _M_in_end - _M_in_cur;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/**
 * Create an InputStream from an open handle.
 * This type of stream may not be seekable.
 */
InputStream::InputStream(int handle)
  : streambuf(new HandleStreambuf(handle, true))
{
    std::istream::init(streambuf);
}

/**
 * Create an InputStream from an regular file.
 * This type of stream is seekable.
 */
InputStream::InputStream(const char *fileName)
  : streambuf(new MappedFileStreambuf(fileName))
{
    std::istream::init(streambuf);
}

/**
 * Create an InputStream from an regular file.
 * This type of stream is seekable.
 */
InputStream::InputStream(const std::string &fileName)
  : streambuf(new MappedFileStreambuf(fileName.c_str()))
{
    std::istream::init(streambuf);
}

InputStream::~InputStream() {
    delete streambuf;
}

/**
 * Init a mapped file buffer
 */
MappedFileStreambuf::MappedFileStreambuf(const char *fileName)
  : HandleStreambuf(true)
{
    File fileIn(fileName);
    if (!fileIn.exists()) {
        errno = ENOENT;
        throw utils::FileOpenError(true, fileName);
    }

    if (!fileIn.isRegularFile()) {
        // TODO
        throw std::runtime_error("file is not a regular file!");
    }

    handle = open(fileName, O_RDONLY);
    if (handle < 0)
        throw utils::FileOpenError(true, fileName);

    dataSize = fileIn.size();
    if ( 0 == dataSize )
    {
        // mmap does not allow to map empty files, simulate empty buffer
        buffer = new char[1];
        buffer[0] = 0;
        setg(buffer, buffer, buffer);
        return;
    }

    // map into virtual memory as read-only data
    buffer = (char*) mmap(NULL, dataSize, PROT_READ, MAP_FILE | MAP_SHARED, handle, 0);
    if (buffer == (char*) -1)
    {
        buffer = NULL;
        ::close(handle);
        throw utils::IOException("Error in mmap", fileName);
    }

    // close the file and indicate that the buffer is not allocated
    ::close(handle);
    handle = -4;
    setg(buffer, buffer, buffer + dataSize);
}

/**
 * Safely handle the buffer.
 */
MappedFileStreambuf::~MappedFileStreambuf()
{
    close();
}

/**
 * Release buffer
 */
void MappedFileStreambuf::close()
{
    // unmap buffer if it was created with mmap
    if (handle == -4)
    {
        munmap(buffer, dataSize);
        buffer = NULL;
    }
    else {
        if (handle >= 0)
            ::close(handle);
    }
    handle = -1;
    // otherwise delete[] buffer in base dtor.
}


/**
 * Alters the stream positions, to an absolute position.
 */
std::istream::pos_type MappedFileStreambuf::seekpos(pos_type newPos, std::ios_base::openmode mode)
{
    if ((mode | std::ios_base::in) == 0)
        return pos_type(off_type(-1));

    if (newPos >= 0 && newPos <= dataSize)
    {
        // newPos == dateSize means: position at EOF
        setg(buffer, buffer + newPos, buffer + dataSize);
        return pos_type(newPos);
    }
    return pos_type(off_type(-1));
}

/**
 * Alters the stream positions relative.
 */
std::istream::pos_type MappedFileStreambuf::seekoff(off_type offset, std::ios_base::seekdir direction, std::ios_base::openmode mode)
{
    if ((mode | std::ios_base::in) == 0)
        return pos_type(off_type(-1));

    pos_type newPos;
    if (direction == std::_S_beg)
        newPos = offset;
    else if (direction == std::_S_cur)
        newPos = (_M_in_cur - _M_in_beg) + offset;
    else if (direction == std::_S_end)
        // offset 0 at means: next char is EOF
        newPos = dataSize - offset;
    else
        throw std::logic_error("direction not supported!");

    if (newPos >= 0 && newPos <= dataSize)
    {
        // newPos == dateSize means: position at EOF
        setg(buffer, buffer + newPos, buffer + dataSize);
        return pos_type(newPos);
    }
    return pos_type(off_type(-1));
}

} /* namespace utils */
