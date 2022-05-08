/*
 * FileObject.h
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

#ifndef FILEOBJECT_H_
#define FILEOBJECT_H_

#include <vector>
#include "Object.h"
#include "File.h"

namespace utils {

/**
 * FileObject is a class for fast read-only file access. The class
 * handles line ends correctly when the method 'getsNoLf' is used (WIN / Linux, not Mac).
 *
 * Within LINUX this class uses nmap(2), which offers a very efficient
 * file access and does NOT use ANY buffers. The whole file is mapped into
 * virtual memory and is read just like memory that has been swapped out.
 */
class FileObject : public Object
{
private:
    std::string name;        // filename
    std::string charset;

    unsigned char *buffer;   // io buffer
    unsigned char *next;     // next char in input
    unsigned char *last;     // last char of input in current buffer
    char   *csvData;         // optionally allocated for getCols
    int64_t buffSize;        // size of allocated buffer
    int64_t filePos;         // position of last '::read()'
    int64_t marker;          // latest marker set by 'mark()'
    int     handle;          // file handle

    mutable int64_t fileSize;  // size of the file

    int  fillBuffer(void);                  // read next bytes
    void initBuffer(bool handleSet, bool detectCharset);
    int  readOnce  (char *dest, int max);   // read max, but call fillBuffer once

    // cannot be used: copy constructor + assignment opr.
    FileObject(const FileObject &) = delete;
    FileObject & operator= (const FileObject &) = delete;

public:
    /**
     * Open the file with the given name. If the file cannot be opened, the
     * method isOpen() returns false and 'errno' is set.
     * <P>
     * If 'detectCharset' is true, the first bytes are checked for a byte order mark
     * and the data pointer is moved forward (byte order mark is not returned.
     */
    FileObject(const char *name, bool detectCharset = false);
    FileObject(const std::string &name, bool detectCharset = false);
    FileObject(const File &file, bool detectCharset = false);

    // use an open file (see 'man open(2)')
    FileObject(int handle, bool detectCharset = false);

    virtual ~FileObject();      // free buffer

    // returns the name which was passed to the constructor
    // or '<handle X>' if the handle constructor was used.
    const std::string getName() const {
        return name;
    }

    size_t  size (void) const;  // get file size in bytes (binary) or 0 if file is not present
    int64_t tell (void) const;  // get current file position
    int64_t seek (int64_t p);   // seek to position, return -1 on error or 'p' if OK

    // set a position marker
    void mark() {
        marker = tell();
    }

    // rewind to last position defined by mark().
    bool rewind() {
        return seek(marker) == marker;
    }

    bool seekEol(void);                 // set file position to next '\n', return true if '\n' was found
    bool isOpen (void) const;           // is the file open ?
    int  read   (void *dest, int max);  // read max chars, return # chars read
    int  getchar(void);                 // read one char, return EOF or the char
    int  undo   (void);                 // put back last char into buffer IF POSSIBLE
                                        // return -1 if not possible

    /**
     * Searches the given text in the first 'maxPos' bytes of the input file and
     * resets the read position back to the beginning.
     * @param text text to search (case sensitive)
     * @param maxPos max byte index where the text must appear, -1 for any position in the file.
     * @return -1 if the text was not found, otherwise the byte index from start.
     */
    int64_t contains(const char *text, int64_t maxPos);

    // Reads the whole file and returns the number of lines in the file.
    // The result is computed each time this method is called (performance!).
    // Set the file-position back to the first char at the end of this method.
    int countLines(void);

    // Read one line, returns number of bytes read (without 0-byte), returns -1
    // at EOF. Reads only (max-1) bytes, so that there is space for the trailing
    // 0-byte => return value is always less than max and a 0-byte is saved
    int gets(char *dest, int max);

    // Like gets, but remove \r\n from the string immediately. Only \n is
    // the line separator. If this char is missing, the line is not terminated.
    int getsNoLf(char *dest, int max);

    // Return the automatically detected charset or an empty string if the detection
    // was disabled (see constructor args).
    // @return UTF-8, UNICODE or an empty string
    const std::string getCharset() const {
        return charset;
    }

    /**
     * Resets the current and the other FileObject to the start of the file and compares
     * the file line by line. The maximum line length is 8192 bytes.
     * @param other - the other text file to compare with
     * @param thisLine - will be filled with the first line that does not match
     * @param otherLine - will be filled with the first line that does not match
     * @param hasIgnoreMarker - if true, lines with '@IGNORE@' will not be compared.
     * @return 0 means that the files are equal, a value larger than 0 defines the line
     *         number of the mismatch; in this case the output strings are filled if
     *         the pointers are present.
     */
    unsigned compare(FileObject &other, std::string *thisLine = NULL, std::string *otherLine = NULL, bool hasIgnoreMarker = false);

    bool operator==(FileObject &other) {
        return compare(other) == 0;
    }
    bool operator!=(FileObject &other) {
        return compare(other) != 0;
    }

    /**
     * Reset the file to the start and compare the content byte by byte with the given string,
     * only useful for small files.
     * @return true if the file content matches exactly the text in the string
     */
    bool operator==(const std::string &content);
    bool operator!=(const std::string &content) {
        return ! (*this == content);
    }

    /**
     * Loads one line and splits up the line into separate columns. A single column may
     * not be larger than 8190 chars.
     *
     * @param cols destination for all columns: the vector is NOT cleared before adding columns.
     * @param trim if set to true blanks before and at the end are cleared
     * @param comment if set, this char introduces a comment line, if set to -1 not comment lines
     * @param sep if set to ' ' and trim is true, multiple blanks are treated as column separator
     * @return the number of columns added or -1 at EOF.
     */
    int getCols(std::vector<std::string> &cols, bool trim = true, char comment = '#', char sep = ';');

    /**
     * Loads one line and splits up the line into separate columns: different to the method
     * above the data is stored in an internal line buffer, the total line may not be longer
     * than 16000 bytes, but the method is more efficient as string copy is not necessary.
     *
     * @param cols destination for all columns: the vector is NOT cleared before adding columns.
     * @param trim if set to true blanks before and at the end are cleared
     * @param comment if set, this char introduces a comment line, if set to -1 not comment lines
     * @param sep if set to ' ' and trim is true, multiple blanks are treated as column separator
     * @return the number of columns added or -1 at EOF.
     */
    int getCols(std::vector<const char *> &cols, bool trim = true, char comment = '#', char sep = ';');

    // load all lines from the file into the given list, return number of lines read.
    unsigned load(std::vector<std::string> &list);

    // this method is more for testing purpose:
    // change default buffer size to a smaller value in order to force
    // more buffer reads. Returns the old value of the buffer size.
    static int setBuffSize(int buffSize);

};                //----- class FileObject


/****************************************************************************
 *                        FileObject inline functions
 ***************************************************************************/

inline bool FileObject::isOpen(void) const
{
    return (handle != -1);
}                //----- is open


inline int FileObject::getchar(void)
{
    return (next >= last ? fillBuffer() : *next++);
}                //----- ReadChar

}

#endif
