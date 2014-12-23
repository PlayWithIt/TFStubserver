/*
 * FileVisitor.h
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
#ifndef FILEVISITOR_H_
#define FILEVISITOR_H_

#include <stack>

#include "File.h"
#include "FileFilter.h"

namespace utils {

/**
 * FileVisitor interface is used by {@link utils::File::visit()} and is used by the
 * visitor pattern implemented in File to recursively visit all files and directories
 * in a directory tree.
 * <br>
 * See also {@link utils::File}<br>
 * See also {@link utils::FileFilter}
 */
class FileVisitor {
public:
    virtual ~FileVisitor();

    enum VisitResult {
        /**
         * Continue visiting files + directories.
         */
        CONTINUE,
        /**
         * Stop immediately the whole scan.
         */
        STOP,
        /**
         * Stop visiting the current directory, continue in parent directory.
         */
        STOP_DIR,
    };

    /**
     * Is called for each file in a directory.
     * <p>
     * If this method return {@link STOP}, the recursion is stopped immediately.<br>
     * If this method return {@link STOP_DIR}, <code>afterVisitDirectory</code>
     * is called for this directory, and the scan continues with the parent level.
     */
    virtual VisitResult visitFile(const File &f) = 0;

    /**
     * Is called before the tree of this sub-directory is analyzed (start recursion into directory).
     */
    virtual VisitResult visitDirectory(const File &dir) = 0;

    /**
     * Is called after the directory tree was analyzed (end recursion from directory).
     */
    virtual VisitResult afterVisitDirectory(const File &dir) = 0;
};


/**
 * Collects all files that were visited, this is combined with a {@link utils::FileFilter}
 * which is not able on its own to descend into other directories.
 */
class FileCollector : public FileVisitor
{
    std::list<File> *result;
    FileFilter *filter;
    size_t recursionCount;
    size_t added;
    bool allocated;

public:
    /**
     * Inits the FileCollector with a client specific filter object.
     * The filter object gets NOT destroyed, so a 'delete' is necessary after filtering is done.
     */
    FileCollector(FileFilter *_filter = NULL)
     : result(new std::list<File>()), filter(_filter), recursionCount(0), added(0), allocated(true) { }

    /**
     * Inits the FileCollector with a client specific result list and a filter object.
     * The filter object gets NOT destroyed, so a 'delete' is necessary after filtering is done.
     */
    FileCollector(std::list<File> *_result, FileFilter *_filter = NULL)
     : result(_result), filter(_filter), recursionCount(0), added(0), allocated(false) { }

    /**
     * Inits the FileCollector with a client specific result list and a filter object.
     */
    FileCollector(std::list<File> *_result, FileFilter &_filter)
     : result(_result), filter(&_filter), recursionCount(0), added(0), allocated(false) { }

    virtual ~FileCollector();

    /**
     * Get a reference to the result list, this gets invalid if the object gets
     * destroyed if constructor 1 was used.
     */
    std::list<File>& getResult() const {
        return *result;
    }

    /**
     * Returns the number of items added to the list by this search. This might not be the
     * real size of the list, if the list was not empty at the beginning.
     */
    size_t size() const {
        return added;
    }

    bool empty() const {
        return added == 0;
    }

    virtual VisitResult visitFile(const File &f);

    virtual VisitResult visitDirectory(const File &dir);

    virtual VisitResult afterVisitDirectory(const File &dir);
};

} /* namespace utils */

#endif /* FILEVISITOR_H_ */
