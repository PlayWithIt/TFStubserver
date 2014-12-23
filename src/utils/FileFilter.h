/*
 * FileFilter.h
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
#ifndef FILEFILTER_H_
#define FILEFILTER_H_

#include <regex>

namespace utils {

class File;

/**
 * A FilterFilter can be used to filter a <b>directory contents</b> of a single directory
 * non recursive, see {@link File::list}.
 */
class FileFilter
{
public:
    virtual ~FileFilter();

    /**
     * If this method returns true, the given file should be inserted into the result list.
     * @param file - the file to check
     */
    virtual bool operator()(const File& file) = 0;
};


/**
 * This is an abstract base class for filters that filter by filename. The class
 * has a filename pattern for derived classes.
 */
class FilenameFilter : public FileFilter
{
protected:
    std::string filenamePattern;

public:
    FilenameFilter(const char *s) : filenamePattern(s) { }
    FilenameFilter(const std::string &s) : filenamePattern(s) { }
};


/**
 * Filters the directory content just by the end of the filename. If the given
 * suffix matches the end of the filename, the filename is returned.
 * The result might still contain directory names.
 */
class FileSuffixFilter : public FilenameFilter
{
public:
    /**
     * Construct with a filename suffix.
     */
    FileSuffixFilter(const char *s) : FilenameFilter(s) { }

    /**
     * Construct with a filename suffix.
     */
    FileSuffixFilter(const std::string &s) : FilenameFilter(s) { }

    virtual bool operator()(const File& file) override;
};

/**
 * Filters the directory content just by the beginning of the filename. If the given
 * prefix matches the beginning of the filename, the filename is returned.
 * The result might still contain directory names.
 */
class FilePrefixFilter : public FilenameFilter
{
public:
    /**
     * Construct with a filename prefix.
     */
    FilePrefixFilter(const char *s) : FilenameFilter(s) { }

    /**
     * Construct with a filename prefix.
     */
    FilePrefixFilter(const std::string &s) : FilenameFilter(s) { }

    virtual bool operator()(const File& file) override;
};

/**
 * Filters the directory content by regular expressions for filenames. Be aware that
 * the functionality is based on the regex-compiler support which is not fully
 * functional with libstdc++.
 */
class FileRegexFilter : public FilenameFilter
{
    std::regex re;

public:
    /**
     * Construct with a filename regex expression.
     */
    FileRegexFilter(const char *s) : FilenameFilter(s), re(s) { }

    /**
     * Construct with a filename regex expression.
     */
    FileRegexFilter(const std::string &s) : FilenameFilter(s), re(s) { }

    virtual bool operator()(const File& file) override;
};

/**
 * Filters the directory content by file type (directories, regular files
 * and any other type of files).
 */
class FileTypeFilter : public FileFilter
{
    bool dirs;
    bool files;
    bool other;

public:
    /**
     * Filter by given file types.
     */
    FileTypeFilter(bool dirs, bool files, bool other = false);

    virtual bool operator()(const File& file) override;
};

/**
 * A filter which combines two other filters with logical AND. The operator() calls
 * 'first->operator()' and then 'second->operator()' if the first returned true.
 */
class FileFilterAnd : public FileFilter
{
    FileFilter *first;
    FileFilter *second;

    FileFilterAnd(const FileFilterAnd &o) = delete;
    FileFilterAnd& operator=(const FileFilterAnd &o) = delete;

public:
    /**
     * Init with two new filters.
     */
    FileFilterAnd(FileFilter *_first, FileFilter *_second)
      : first(_first), second(_second) { }

    /**
     * Delete first and second!
     */
    virtual ~FileFilterAnd();

    FileFilter *getFirst() const {
        return first;
    }

    FileFilter *getSecond() const {
        return second;
    }

    /**
     * Filter by condition of 'first() AND second()'
     */
    virtual bool operator()(const File& file) override;
};

/**
 * A filter which combines two other filters with logical OR. The operator() calls
 * 'first->operator()' and then 'second->operator()' if the first returned false.
 */
class FileFilterOr : public FileFilter
{
    FileFilter *first;
    FileFilter *second;

    FileFilterOr(const FileFilterOr &o) = delete;
    FileFilterOr& operator=(const FileFilterOr &o) = delete;

public:
    /**
     * Init with two new filters.
     */
    FileFilterOr(FileFilter *_first, FileFilter *_second)
      : first(_first), second(_second) { }

    /**
     * Delete first and second!
     */
    virtual ~FileFilterOr();

    FileFilter *getFirst() const {
        return first;
    }

    FileFilter *getSecond() const {
        return second;
    }

    /**
     * Filter by condition of 'first() OR second()'
     */
    virtual bool operator()(const File& file) override;
};


} /* namespace brickapi */

#endif /* FILEFILTER_H_ */
