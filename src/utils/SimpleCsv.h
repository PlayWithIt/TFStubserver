/*
 * SimpleCsv.h
 *
 * Copyright (C) 2015 Holger Grosenick
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
#ifndef UTILS_SIMPLECSV_H_
#define UTILS_SIMPLECSV_H_

#include "FileObject.h"

namespace utils
{

/**
 * Small class that reads CSV files in a very simple way. The class does not support
 * CSV files with just one column (without separator): use plain text methods of
 * the FileObject instead.
 */
class SimpleCsvBase : public FileObject
{
protected:
    unsigned numCols;
    unsigned line;
    char     comment;
    char     columnSep;

    void open(const char *filename);

public:
    /**
     * Open the given CSV file, if the given separator is -1, is class tries
     * to determines the separator itself: depending on the first data line, it
     * searches for ';', TAB and '|' and the first char found will be the separator.
     * <P>
     * The constructor throws an Exception if the file cannot be read!
     */
    SimpleCsvBase(const char *name,        char comment = '#', char sep = -1);
    SimpleCsvBase(const std::string &name, char comment = '#', char sep = -1);
    SimpleCsvBase(const File &file,        char comment = '#', char sep = -1);

    /**
     * Returns the number of columns detected in the first (!) line.
     */
    unsigned getNumCols() const {
        return numCols;
    }

    /**
     * Returns the column separator detected in the first (!) line.
     */
    char getColumnSep() const {
        return columnSep;
    }

    unsigned getLine() const {
        return line;
    }
};


/**
 * Small class that reads CSV files in a very simple way. The class does not support
 * CSV files with just one column (without separator): use plain text methods of
 * the FileObject instead.
 */
template <typename _Tp> class SimpleCsvTmpl : public SimpleCsvBase
{
protected:
    std::vector< _Tp > cols;

public:
    /**
     * Open the given CSV file, if the given separator is -1, is class tries
     * to determines the separator itself: depending on the first data line, it
     * searches for ';', TAB and '|' and the first char found will be the separator.
     * <P>
     * The constructor throws an Exception if the file cannot be read!
     */
    SimpleCsvTmpl(const char *name,        char comment = '#', char sep = 0) : SimpleCsvBase(name, comment, sep) { }
    SimpleCsvTmpl(const std::string &name, char comment = '#', char sep = 0) : SimpleCsvBase(name, comment, sep) { }
    SimpleCsvTmpl(const File &file,        char comment = '#', char sep = 0) : SimpleCsvBase(file, comment, sep) { }

    /**
     * Returns a read reference to the column vector.
     */
    const std::vector< _Tp >& getColumns() const {
        return cols;
    }

    /**
     * Gets the columns value of the given column as integer.
     */
    int getInt(unsigned no) const;

    /**
     * Gets the columns value of the given column as string.
     */
    const _Tp& getString(unsigned no) const {
        return cols[no];
    }

    /**
     * Gets the columns value of the given column as char.
     */
    char getChar(unsigned no) const {
        return cols[no][0];
    }


    /**
     * Loads the next line into the internal vector (calls {@link FileObject::getCols()} internally).
     * Return true if data was read, false on EOF. The method will skip empty lines, which means that
     * the column vectors has more than 0 columns afterwards.
     * <P>
     * Only this methods increments the line number.
     */
    bool loadLine()
    {
        cols.clear();
        do {
            if (getCols(cols, true, comment, columnSep) < 0)
                return false;
            ++line;
        } while (cols.empty());
        return true;
    }

};


/**
 * Type specific implementation of this method.
 */
template<> inline int SimpleCsvTmpl<std::string>::getInt(unsigned no) const {
    return atoi(cols[no].c_str());
}

template<> inline int SimpleCsvTmpl<const char*>::getInt(unsigned no) const {
    return atoi(cols[no]);
}


typedef SimpleCsvTmpl<std::string> SimpleCsv;
typedef SimpleCsvTmpl<const char*> SimpleCsvCharP;


} /* namespace parsers */

#endif /* UTILS_SIMPLECSV_H_ */
