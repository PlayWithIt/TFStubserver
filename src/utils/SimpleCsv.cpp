/*
 * SimpleCsv.cpp
 *
 * Copyright (C) 2015-2021 Holger Grosenick
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

#include "SimpleCsv.h"
#include "Exceptions.h"

#include <string.h>


namespace utils
{

SimpleCsvBase::SimpleCsvBase(const char *name, char comment, char sep)
    : FileObject(name)
    , comment(comment)
    , columnSep(sep)
{
    open(name);
}

SimpleCsvBase::SimpleCsvBase(const std::string &name, char comment, char sep)
    : FileObject(name)
    , comment(comment)
    , columnSep(sep)
{
    open(name.c_str());
}

SimpleCsvBase::SimpleCsvBase(const File &file, char comment, char sep)
    : FileObject(file)
    , comment(comment)
    , columnSep(sep)
{
    open(file.getFullname().c_str());
}

/**
 * Read the first dataline and try to detected the column separator.
 */
void SimpleCsvBase::open(const char *filename)
{
    numCols = 1;
    line = 0;
    if (!isOpen()) {
        throw utils::FileOpenError(true, filename);
    }

    // empty file ?
    if (this->size() == 0) {
        if (!columnSep)
            columnSep = ';';
        numCols = 0;
        return;
    }

    // already defined in constructor?
    if (columnSep)
        return;

    // find first non-comment line
    char line[4096];
    while (gets(line, sizeof(line)) != EOF) {
        const char *p = line;
        while (*p == ' ')
            ++p;

        // data line found?
        if (*p && *p != comment)
            break;
    }
    seek(0);

    // use CSV load with ';' or '|' as separator
    const char *s = strpbrk(line, ";|\t");
    if ( !s )
        throw utils::RuntimeError(std::string("Did not find one of the excepted column separators in ") + filename);

    // count number of columns: there is always at least one column, the first
    // separator starts the second column !
    columnSep = *s;
    for (const char *c = line; *c != 0; ++c) {
        if (*c == *s)
            ++numCols;
    }
}

} /* namespace parsers */
