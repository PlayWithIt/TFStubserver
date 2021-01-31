/*
 * FileObject.cpp
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

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/mman.h>
#define USE_MMAP
#else
#include <io.h>
#endif

#include <vector>

#include "Exceptions.h"
#include "FileObject.h"

// different for Windows / MAC
#define LINE_END    '\n'
#define LINE_SKIP   '\r'
#define MAX_CSVLINE 16380


namespace utils {


// Default buffer size
static int defaultBuffSize = 8190;
static unsigned char emptyString[2] = { 0, 0 };
static const uint8_t UTF8_HEADER[] = { 0xEF, 0xBB, 0xBF };

/**
 * Seek to the given position within the file. The position is counted
 * from the beginning of the file (start with 0).
 *
 * @param p  the file offset
 * @return -1 on failure, the new file offset otherwise
 */
int64_t FileObject::seek(int64_t p)
{
#ifdef USE_MMAP
   if (p >= 0 && p < filePos)
   {
      // position is in the current buffer
      next = buffer + p;
      return p;
   }

   return -1;                   // failure: p is wrong
#else
   last    = next;              // force a re-read
   filePos = lseek(handle, p, SEEK_SET);
   return filePos;
#endif
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
unsigned FileObject::compare(FileObject &other, std::string *thisLine, std::string *otherLine, bool hasIgnoreMarker)
{
    char line1[8200];
    char line2[8200];
    int  lineNo = 0;

    this->seek(0);
    other.seek(0);

    do {
        ++lineNo;
        line1[0] = 0;
        line2[0] = 0;

        int size1 = this->gets(line1, sizeof(line1));
        int size2 = other.gets(line2, sizeof(line2));

        if (size1 != size2 && (!hasIgnoreMarker || (!strstr(line1, "@IGNORE@") && !strstr(line2, "@IGNORE@")) )) {
            if (thisLine)
                *thisLine = line1;
            if (otherLine)
                *otherLine = line2;
            return lineNo;
        }

        // else: size1 == size2
        if (size1 == EOF)
            return 0;

        if (strcmp(line1, line2) != 0 && (!hasIgnoreMarker || (!strstr(line1, "@IGNORE@") && !strstr(line2, "@IGNORE@")) )) {
            if (thisLine)
                *thisLine = line1;
            if (otherLine)
                *otherLine = line2;
            return lineNo;
        }

    } while (true);
}

/****************************************************************************
 * Reads the whole file and returns the number of lines in the file.
 * The result is computed each time this method is called (performance!).
 * Set the file-position back to the first char at the end of this method.
 *
 * @return the number of lines in the file
 ***************************************************************************/
int FileObject::countLines()
{
   char line[2000];     // not too large, save stack
   int  lines = 0;
   bool noLf  = false;
   int  size;

   while ((size = gets(line, sizeof(line))) != EOF)
   {
     // when \n is stored: line is shorter than 2000 bytes
     if (size > 0 && line[size-1] == LINE_END)
     {
       ++lines;
       noLf = false;
     }
     else            // no \n
       noLf = true;
   }
   if (noLf)         // \n not found and EOF: the last line has no \n, then
     ++lines;        // add one line, because this is one line
   this->seek(0);
   return lines;
}


/**
 * Return the file offset of the next char (from the beginning of the file)
 */
int64_t FileObject::tell(void) const
{
    return filePos - (last - next);
}


/**
 * Read next chars from input. This method is only called, when
 * USE_MMAP is not defined and sequential buffer reads are necessary.
 * With MMAP the whole file is just one big buffer of virtual memory.
 *
 * @return next char in input or EOF on end
 */
int FileObject::fillBuffer(void)
{
    int n;

    if (handle < 0 || (n = ::read(handle, buffer, buffSize)) <= 0)
    {
        next = last;
        return EOF;
    }

    filePos += n;                 // last char in buffer from SEEK_SET

    last = buffer + n;
    next = buffer + 1;

    return *buffer;
}                //----- fillBuffer



/****************************************************************************
 * FUNCTION : FileObject(const char *name)
 *
 * TASK
 *   creates a file object from a file. The file name is passed as parameter
 *
 * INPUT PARAMETERS : name of the file to be read
 ***************************************************************************/

FileObject::FileObject(const char *_name, bool detectCharset)
  : name(_name), handle(-1)
{
    initBuffer(false, detectCharset);
}

FileObject::FileObject(const std::string &_name, bool detectCharset)
  : name(_name), handle(-1)
{
    initBuffer(false, detectCharset);
}

FileObject::FileObject(const File &file, bool detectCharset)
  : name(file.getFullname()), handle(-1)
{
    initBuffer(!file.exists(), detectCharset);
}


FileObject::FileObject(int _handle, bool detectCharset)
{
    char buf[64];
    sprintf(buf, "<handle %d>", handle);

    name   = buf;
    handle = _handle;

    initBuffer(true, detectCharset);
}                //----- constructor


void FileObject::initBuffer(bool handleSet, bool detectCharset)
{
    fileSize = -1;
    buffer   = NULL;
    csvData  = NULL;
    marker   = 0;

    if (!handleSet)
    {
#ifndef _WIN32
        handle = open(name.c_str(), O_RDONLY);
#else
        handle = open(name.c_str(), O_RDONLY | O_BINARY);
#endif
    }

    if (handle >= 0) {                       // start to read

#ifdef USE_MMAP
        struct stat st;

        if (fstat(handle, &st) != 0) {       // read file size
            throw IOException("fstat", name);
        }

        if (S_ISDIR(st.st_mode)) {           // reading directories is invalid!
            close(handle);                   // always close the file ..
            handle = -1;
            return;
        }

        if ( 0 == st.st_size )
        {
            // mmap does not allow to map empty files, simulate empty buffer
            close(handle);                       // always close the file ..
            handle = -3;                         // isOpen still return true
            buffer = emptyString;
        }
        else {
            buffer = (unsigned char*) mmap(NULL, st.st_size,    // map file into virtual */
                                           PROT_READ,           // memory - read only ...
                                           MAP_FILE | MAP_SHARED,
                                           handle, 0);

            if (buffer == MAP_FAILED) {                // error from 'mmap' ?
                throw IOException("mmap", name);
            }
            close(handle);                             // always close the file ..
            handle = -2;
        }

        fileSize =
        filePos  =
        buffSize = st.st_size;
        last     = buffer + st.st_size;

#else
        buffer = new unsigned char[buffSize = defaultBuffSize];
        last   = buffer + (filePos = ::read(handle, buffer, defaultBuffSize));
#endif

        next = buffer;

        //printf("***\n"
        //       "size=%ld 0=%d 1=%d 2=%d\n"
        //       "***\n", (int)buffSize, buffer[0], buffer[1], buffer[2]);

        // See 'byte order mark': https://de.wikipedia.org/wiki/Byte_Order_Mark
        if (detectCharset && buffSize >= 3) {
            // Hex bytes 0xEF 0xBB 0xBF
            if (memcmp(buffer, UTF8_HEADER, 3) == 0)
            {
                // this is the typical windows header for UTF-8
                charset = "UTF-8";
                next += 3;
            }
            else if ((buffer[0] == 255 && buffer[1] == 254) || (buffer[0] == 254 && buffer[1] == 255))
            {
                // This is the typical windows header for unicode (little endian + big endian)
                charset = "UNICODE";
                next += 2;
            }
        }
    }
    else {
        buffer   = last = NULL;
        buffSize = 0;
        filePos  = 0;
        next     = buffer;
    }
}                //----- initBuffer


FileObject::~FileObject()
{
    delete[] csvData;
    if (handle >= 0)
        close(handle);

#ifdef USE_MMAP
    if (handle == -2)
        munmap(buffer, buffSize);
    else
#endif
        // buffer has not been passed from the outside ...
        if (buffer && handle >= 0)
            delete [] buffer;
}                //----- ~FileObejct


// load all lines from the file into the given list, return number of lines read.
unsigned FileObject::load(std::vector<std::string> &list)
{
    if (!isOpen())
        return 0;

    unsigned line = 0;
    int len;

    if (csvData == NULL)
        csvData = new char[MAX_CSVLINE];

    while ((len = getsNoLf(csvData, MAX_CSVLINE)) >= 0)
    {
        if (len >= static_cast<int>(MAX_CSVLINE - 1))
            throw Exception("FileObject::load: buffer overflow");
        if (len > 0) {
            list.push_back(csvData);
            ++line;
        }
    }

    return line;
}

/**
 * Determines the size of the file. If the size is known, there will be no call to 'stat' !
 *
 * @return the size of the file in bytes
 */
size_t FileObject::size(void) const
{
    if (fileSize == -1)
    {
        struct stat st;
        if (handle >= 0 && fstat(handle, &st) == 0)
            fileSize = st.st_size;
    }

    return fileSize < 0 ? 0 : fileSize;
}                //----- size



/****************************************************************************
 * FUNCTION : readOnce(void *dest, int anz)
 *
 * TASK
 *   reads anz chars from input, but call fillBuffer not more than once.
 *   The result is, that only very few bytes might be read.
 *
 * RETURN VALUE : number of chars read
 ***************************************************************************/

int FileObject::readOnce(char *dest, int anz)
{
    int i, n = last - next;

    if (n >= anz) {                        // data available in one paket
        memcpy(dest, next, anz);
        next += anz;
        return anz;                          // desired number of bytes read
    }

    if (n > 0)                             // something remaining => copy this
        memcpy(dest, next, n);

    if (fillBuffer() == EOF)               // fill buffer again
        return (n > 0 ? n : EOF);

    // new destination
    dest += n;
    i     = anz  - n;                      // remaining number of bytes
    n     = last - buffer;                 // max. available

    if (n >= i) {
        memcpy(dest, buffer, i);           // fillBuffer sets 'next' to 'buffer + 1'
        next = buffer + i;
        return anz;
    }

    memcpy(dest, buffer, n);
    next = last;                           // at EOF
    return anz - (i - n);
}                //----- read



/****************************************************************************
 * FUNCTION : read(void *dest, int anz)
 *
 * TASK
 *   reads anz chars from input
 *
 * RETURN VALUE : number of chars read
 ***************************************************************************/

int FileObject::read(void *dest, int anz)
{
    int alreadyRead = 0;

    while (anz > 0)
    {
        int n = readOnce(((char*) dest) + alreadyRead, anz);
        if (n == EOF)
            return alreadyRead;
        alreadyRead += n;
        anz -= n;
    }

    return alreadyRead;
}                //----- read


/**
 * Loads one line and splits up the line into separate columns.
 * @param cols destination for all columns: the vector is NOT cleared before adding columns.
 * @param trim if set to true blanks before and at the end are cleared
 * @param comment if set, this char introduces a comment line, if set to -1 not comment lines
 * @param sep if set to ' ' and trim is true, multiple blanks are treated as column separator
 * @return the number of columns added or -1 at EOF.
 */
int FileObject::getCols(std::vector<std::string> &cols, bool trim, char comment, char sep)
{
    int c = getchar();
    if (c == EOF)
        return -1;

    if (csvData == NULL)
        csvData = new char[MAX_CSVLINE];

    char *lastCh = csvData;
    const char * const end = lastCh + MAX_CSVLINE - 1;
    bool first = true;
    unsigned numCols = 0;

    while (c != EOF)
    {
        if (c == '\r') {
            c = getchar();
            continue;
        }

        // first 'real' char in line?
        if (first && c != ' ' && c != '\t') {
            if (c == comment) {
                seekEol();
                getchar();     // this is the '\n'
                c = getchar(); // this is first char after '\n'
                continue;
            }
            first = false;
        }

        if (c == sep || c == LINE_END) { // '\n' will NOT be saved in buffer
            *lastCh = 0;

            if (trim && lastCh != csvData)
            {
                char *start = csvData;
                while (*start == ' ' && start != lastCh)
                    ++start;

                unsigned size = lastCh-start;
                while (size > 0 && start[size-1] == ' ')
                    --size;

                // don't add empty cols at the end
                if (c == LINE_END && size == 0)
                    return numCols;

                cols.push_back(std::string(start, size));
                ++numCols;
            }
            else {
                // don't add empty cols at the end
                if (c == LINE_END && lastCh == csvData)
                    return numCols;

                cols.push_back(csvData);
                ++numCols;
            }
            if (c == LINE_END)
                break;
            lastCh = csvData;

            // special case: separator == ' ' and trim: don not create multiple columns if multiple blanks are there
            if (c == ' ' && trim && sep == ' ') {
                while (c == ' ')
                    c = getchar();
                continue;
            }
        }
        else {
            *lastCh = c;
            if (++lastCh == end)
                throw Exception("FileObject::getCols: buffer overflow");
        }

        c = getchar();
    }

    return numCols;
}


/**
 * Loads one line and splits up the line into separate columns.
 * @param cols destination for all columns: the vector is NOT cleared before adding columns.
 * @param trim if set to true blanks before and at the end are cleared
 * @param comment if set, this char introduces a comment line, if set to -1 not comment lines
 * @param sep if set to ' ' and trim is true, multiple blanks are treated as column separator
 * @return the number of columns added or -1 at EOF.
 */
int FileObject::getCols(std::vector<const char *> &cols, bool trim, char comment, char sep)
{
    int c = getchar();
    if (c == EOF)
        return -1;

    if (csvData == NULL)
        csvData = new char[MAX_CSVLINE];

    char *colStart   = csvData;
    char *lastCh     = colStart;
    bool first       = true;
    unsigned numCols = 0;

    const char * const end = colStart + MAX_CSVLINE - 1;

    while (c != EOF)
    {
        if (c == '\r') {
            c = getchar();
            continue;
        }

        // first 'real' char in line?
        if (first && c != ' ' && c != '\t') {
            if (c == comment) {
                seekEol();
                getchar();     // this is the '\n'
                c = getchar(); // this is first char after '\n'
                continue;
            }
            first = false;
        }

        if (c == sep || c == LINE_END) { // '\n' will NOT be saved in buffer

            if (trim && lastCh != colStart)
            {
                char *start = colStart;
                while (*start == ' ' && start != lastCh)
                    ++start;

                unsigned size = lastCh-start;
                while (size > 0 && start[size-1] == ' ')
                    --size;

                // don't add empty cols at the end
                if (c == LINE_END && size == 0)
                    return numCols;

                start[size] = 0;
                cols.push_back(start);
                ++numCols;

                lastCh = start + size;
            }
            else {
                // don't add empty cols at the end
                if (c == LINE_END && lastCh == colStart)
                    return numCols;

                *lastCh = 0;
                cols.push_back(colStart);
                ++numCols;
            }

            if (c == LINE_END)
                break;

            if (++lastCh == end)
                throw Exception("FileObject::getCols: buffer overflow");
            colStart = lastCh;

            // special case: separator == ' ' and trim: don not create multiple columns if multiple blanks are there
            if (c == ' ' && trim && sep == ' ') {
                while (c == ' ')
                    c = getchar();
                continue;
            }
        }
        else {
            *lastCh = c;
            if (++lastCh == end)
                throw Exception("FileObject::getCols: buffer overflow");
        }

        c = getchar();
    }

    return numCols;
}


/**
 * Read one line, returns number of bytes read (without 0-byte), returns -1
 * at EOF. Reads only (max-1) bytes, so that there is space for the trailing
 * 0-byte => return value is always less than max and a 0-byte is saved
 */
int FileObject::gets(char * dest, int max)
{
    if (max <= 0)
        return 0;

    if (max == 1) {
        *dest = 0;
        return 0;   // number of bytes without 0-byte
    }

    char * b = dest;
    char * end = b + max - 1;
    int c = getchar();

    if (c == EOF)
        return -1;

    while (c != EOF)
    {
        *b = c;
        b++;
        if (b == end || c == LINE_END)  // '\n' will be saved in buffer
            break;
        c = getchar();
    }

    *b = 0;
    return b - dest;
}                //----- gets



/****************************************************************************
 * FUNCTION : getsNoLf(char *dest, int max)
 * CREATED  : 08.05.97
 *
 * TASK
 *   reads one line from input and stores the line into the given buffer.
 *   Trailing CR / LF will not be stored in the buffer. But be careful, if
 *   only CR appears in the buffer, it will be treated as binary file, so
 *   CR / LF is the only valid combination!
 *   Reads (max - 1) chars and stores a terminating 0-byte at the end of
 *   the string.
 *
 * INPUT PARAMETERS : buffer pointer and size of the buffer
 * RETURN VALUE     : number of chars read = length of result string
 *                    (without trailing 0-byte)
 ***************************************************************************/

int FileObject::getsNoLf(char * dest, int max)
{
    if (max <= 0)
        return 0;

    if (max == 1) {
        *dest = 0;
        return 0;   // number of bytes without 0-byte
    }

    char * b   = dest;
    char * end = b + max - 1;
    int c = getchar();

    if (c == EOF)
        return -1;

    while (c != EOF)
    {
        if (c == LINE_END)
        {
            if ((unsigned long) b > (unsigned long) dest && *(b-1) == LINE_SKIP)
                b--;
            break;
        }
        *b = c;
        b++;
        if (b == end)
        {
            if (*(b-1) == '\r')
            {
                // Puffer voll und \r am Ende => \r entfernen und gleich noch ein
                // Zeichen mehr ueberlesen (\n) damit der folgende getsNoLf() keine
                // leere Zeile liefert. Diese Zeile passt gerade noch in den Puffer.
                b--;
                c = getchar();
            }
            break;
        }
        c = getchar();
    }

    *b = 0;
    return b - dest;
}                //----- getsNoLf



/****************************************************************************
 * FUNCTION : seekEol(void)
 * CREATED  : 08.05.97
 *
 * TASK
 *   sets the file pointer to the next '\n' character
 *
 * RETURN VALUE : true if a '\n' was found, false otherwise
 ***************************************************************************/

bool FileObject::seekEol(void)
{
    next = (unsigned char*) memchr(next, '\n', last - next);    // look for the line's end

    while (!next) {                            // not found !
        if (fillBuffer() != EOF)               // buffer is not empty
            next = (unsigned char*) memchr(buffer, LINE_END, last - next + 1);
        else {
            // end of file reached
            next = last;
            return false;
        }
    }
    return true;
}                //----- seekEol

/****************************************************************************
 * FUNCTION   : undo(void)
 * CREATED    : 08.09.97
 * LAST CHANGE: tt.mm.jj
 *
 * TASK
 *   set the file pointer back to the previous char. This is only
 *   possible, if the pointer is not at the beginning of the buffer.
 *
 * RETURN VALUE : the previous char of -1 if the pointer is at the
 *                beginning of the buffer.
 ***************************************************************************/

int FileObject::undo(void)
{
    if (next > buffer) {                  // there was already one char
        next--;
        return *next;
    }

    return -1;
}               //----- UndoChar

/****************************************************************************
 * FUNCTION : setBuffSize(int buffSize)
 * CREATED  : 08.05.97
 *
 * TASK
 *   changes the default buffer size for new file objects that should be
 *   at least 100 bytes.
 *
 * INPUT PARAMETER : the new value of the default buffer size
 * RETURN VALUE    : the old value of the default buffer size
 ***************************************************************************/

int FileObject::setBuffSize(int buffSize)
{
    // invalid argument
    if (buffSize < 100)
        return defaultBuffSize;

    int res = defaultBuffSize;
    defaultBuffSize = buffSize;
    return res;
}                //----- setBuffSize

}
