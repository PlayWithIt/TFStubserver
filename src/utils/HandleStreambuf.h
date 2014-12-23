/*
 * HandleStreambuf.h
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
#ifndef HANDLESTREAMBUF_H_
#define HANDLESTREAMBUF_H_

#include <iostream>

#include "Object.h"

namespace utils {

/**
 * A simple stream buffer which is build on file handles. This stream buffer just
 * implements basic IO, no seeking and no pushing back data into the buffer.
 */
class HandleStreambuf: public std::basic_streambuf<char>
{
protected:
    char    *buffer;
    int      handle;
    unsigned bufSize;
    int64_t  dataSize;
    bool     input;

    // just for derived classes
    HandleStreambuf(bool _input) : buffer(NULL), handle(-1), bufSize(0), dataSize(0), input(_input) { }

public:
    HandleStreambuf(int _handle, bool _input, unsigned bufSize = 4096);
    virtual ~HandleStreambuf();

    /**
     * Fetches more data from the controlled sequence.
     * @return  The first character from the <em>pending sequence</em>.
     */
    virtual int_type underflow() override;

    /**
     * Returns the number of bytes which are currently in the buffer (read returns without blocking).
     */
    virtual std::streamsize showmanyc() override;

    /**
     * Is called when the buffer is full and must be synced.
     */
    virtual int_type overflow(int_type __c  = traits_type::eof()) override;

    /**
     * Close the handle, this is done automatically in dtor.
     */
    virtual void close();

    /**
     * Write data in buffer to disk (or underlying handle).
     */
    virtual int sync() override;
};

/**
 * A stream buffer that can be used for files only: the file is mapped as a
 * whole into virtual memory using the mmap() functionality.
 */
class MappedFileStreambuf: public HandleStreambuf
{
public:
    MappedFileStreambuf(const char *fileName);
    ~MappedFileStreambuf();

    /**
     *  Alters the stream positions, to an absolute position.
     */
    virtual pos_type seekpos(pos_type, std::ios_base::openmode) override;

    /**
     *  Alters the stream positions relative.
     */
    virtual pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode mode) override;

    /**
     * Close the handle, this is done automatically in dtor.
     */
    virtual void close();
};


/**
 * An input stream which can be build on file handles using the {@link HandleStreambuf}.
 */
class InputStream : public std::istream, public Object
{
    HandleStreambuf *streambuf;

public:
    /**
     * Create an InputStream from an open handle.
     * This type of stream may not be seekable.
     */
    InputStream(int handle);

    /**
     * Create an InputStream from an regular file.
     * This type of stream is seekable.
     */
    InputStream(const char *fileName);

    /**
     * Create an InputStream from an regular file.
     * This type of stream is seekable.
     */
    InputStream(const std::string &fileName);

    virtual ~InputStream();
};


/**
 * An output stream which can be build on file handles using the {@link HandleStreambuf}.
 */
class OutputStream : public std::ostream, public Object
{
    HandleStreambuf streambuf;

public:
    OutputStream(int handle) : streambuf(handle, false)
    {
        std::ostream::init(&streambuf);
    }
};

} /* namespace utils */

#endif /* HANDLESTREAMBUF_H_ */
