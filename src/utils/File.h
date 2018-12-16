/*
 * File.h
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
#ifndef FILE_H_
#define FILE_H_

#include <string>
#include <list>

#include "Object.h"
#include "FileVisitor.h"

namespace utils {

class FileFilter;


/**
 * The File class provides access to the file system:
 * check if a path is a directory or a file, get file size
 * and list directory contents.
 */
class File : public Object
{
public:
    /**
     * Init with empty name: file does not exist!
     */
    File();
    explicit File(const char *name);
    explicit File(const std::string &name);
    explicit File(const File &parentDir, const char *name);
    explicit File(const File &parentDir, const std::string &name);

    typedef std::list<File> FileList;

    /**
     * Move constructor.
     */
    File(File &&other);

    /**
     * Copy constructor: sets the 'deleteOnDestroy' back to false !!
     */
    File(const File &other);

    /**
     * Copy constructor: sets the 'deleteOnDestroy' back to false !!
     */
    File& operator=(const File &other);

    /**
     * Move operator: takes all the data from the referenced object, including 'deleteOnDestroy',
     * The referenced object cannot be used afterwards, it must be deleted immediately!
     */
    File& operator=(File &&other);

    /**
     * Free resource, delete file if 'deleteOnDestroy' is set.
     */
    virtual ~File();

    /**
     * Holds '/' on Linux.
     */
    static const char PATH_SEP_CHAR;

    /**
     * Holds ':' on Linux, ';' on Windows
     */
    static const char PATH_LIST_SEP_CHAR;

    /**
     * Holds "/" on Linux.
     */
    static const char * const PATH_SEP_STR;

    /**
     * Get the current file or directory name without path. If this is
     * the root-directory, the name is empty.
     */
    const std::string& getName() const {
        return name;
    }

    /**
     * Get the (relative) directory part of the input filename. If there is no directory
     * part, this value is the empty string. The path ends with PATH_SEP_CHAR.
     */
    const std::string& getPath() const {
        return path;
    }

    /**
     * Returns the path and name as passed to the constructor.
     */
    const std::string& getFullname() const {
        return fullname;
    }

    /**
     * Determine the absolute name and resolves symbolic links in the path.
     * Throws an exception if the file does not exist.
     */
    std::string getAbsolutePath() const;

    /**
     * If the file is a symlink: return the path where the symlink points to.
     * Throws an exception if this file isn't a symlink.
     */
    File getLinkedPath() const;

    /**
     * Returns a timestamp in milli-seconds since epoch when the file was last modified or
     * 0 if the file does not exist. Use {@link refresh()} before a call, if the file
     * object lives longer and the latest state on the harddrive should be re-read.
     */
    uint64_t getLastModified() const;

    /**
     * Does the file or directory exist?
     */
    bool exists() const {
        return errorCode == 0;
    }

    /**
     * Is the error code (errno) from stat() call.
     */
    int getErrorCode() const {
        return errorCode;
    }

    /**
     * Returns the parent: if the current File is a file: the parent is a folder;
     * if the current File is a directory, the parent is the parent directory.
     * Returns the actual File if it is already the root directory.
     */
    const File getParent() const;

    /**
     * Creates an empty file if it does not exist, makes nothing if a file exists.
     * @return true if the file exists or was created; returns false if the creation
     * of the file fails.
     */
    bool touch();

    /**
     * Delete/remove the file or directory. The directory must be empty to delete it.
     * @return true if the file/directory was deleted, false otherwise. False is also
     *         returned, if the file currently doesn't exist.
     */
    bool remove();

    /**
     * Update internal status: check again, if the file exists or not, read the size again.
     */
    void refresh() const;

    /**
     * Sets a 'deleteOnDestroy' flag which: if set to true, the file pointed to by this object
     * will be deleted by {@link remove()} if this object gets destroyed (when destructor is called).
     * <P>
     * <b>Be careful if a copy-constructor is called</b> for objects where this flag is set
     * to true: the file maybe deleted to early. Use the move operation instead.
     */
    void setDeleteOnDestroy(bool delFile) {
        deleteOnDestroy = delFile;
    }

    /**
     * Returns the deleteOnDestroy flag, see {@link setDeleteOnDestroy()}.
     */
    bool getDeleteOnDestroy() const {
        return deleteOnDestroy;
    }

    /**
     * Renames or moves the current file to the new name or position. If the new
     * file exists, it will only be overwritten, if the parameter is set. Renaming
     * also changes the internal filename to the new name!
     * <p>
     * This method throws an exception in case of any error while renaming (e.g.
     * if the current File does not exist).
     *
     * @param newNameAndPath new location or name, if the name does not contain a path
     *        the file is moved to the current working directory.
     * @param overwrite overwrite existing files?
     * @return true if rename was OK, false if target exists, but 'overwrite' is false
     */
    bool renameTo(const char *newNameAndPath, bool overwrite = false);

    /**
     * Returns true if this File denotes the same directory or path as the other File.
     */
    bool operator==(const File& other) const;
    bool operator!=(const File& other) const {
        return !(*this == other);
    }

    /**
     * Return true if the file is absolute: starts with '/' on Unix or 'C:\' (drive:\) on Windows.
     */
    bool isAbsolute() const;

    bool isDirectory() const;
    bool isRegularFile() const;
    bool isSymlink() const;

    /**
     * Does the file point to a socket?
     */
    bool isSocket() const;

    /**
     * Does the file point to named pipe (FIFO)?
     */
    bool isPipe() const;

    /**
     * Check file / directory access rights of the current user.
     */
    bool canExecute() const;
    bool canRead() const;
    bool canWrite() const;

    /**
     * Lists the contents of the directory denoted by this File. If the current
     * File doesn't point to a directory or does not exist, an exception is thrown.
     * The method will NOT return "." and ".." in the result and it releases the
     * filter object.
     *
     * @param result matching names will be put here
     * @param filter optional filter which can be used to filter filenames.
     *        If an object is passed, it is deleted at the end of this method!
     * @return the number of items put into the result
     */
    size_t list(FileList &result, FileFilter *filter = NULL) const;

    /**
     * Same as list with FilterFilter pointer, but this filter will not be deleted
     * at the end of this method.
     *
     * @param result matching names will be put here
     * @param filter optional filter filter which can be used to filter filenames,
     *        if an object is passed, it is deleted at the end of this method!
     * @return the number of items put into the result
     */
    size_t list(FileList &result, FileFilter &filter) const {
        return listDirectory(result, &filter);
    }

    /**
     * Searches all files below the directory denoted by this File which match the
     * given regex expression (e. h. "a.*\.cpp". The regex must match the whole filename.
     * If only parts of the name are relevant, a ".*" must be added at the beginning and/or end.
     * <P>
     * File doesn't point to a directory or does not exist, an exception is thrown.
     * The method will NOT return "." and ".." in the result.
     * <P>
     * Be aware that the regex functionality is based on the current implementation of the
     * compiler library, this might not be fully functional.
     *
     * @param result - matching names will be put here
     * @param fileNameRegex - a regex which is used for filename matching (not directories!)
     * @param dirs - also include directory names if they match?
     * @param special - also include non regular files if they match?
     * @return the number of items put into the result
     */
    size_t find(FileList &result, const std::string &fileNameRegex, bool dirs = true, bool special = false) const;

    /**
     * A recursive loop down to all sub-directories is started and the {@link FileVisitor} is triggered
     * for each file and directory. If the current file is a regular file, the visitor is called only
     * once for the current file (non regular use). If the current file is a directory, the directory
     * content is listed and the visitor is called with {@link FileVisitor::visitDirectory} for the
     * current dir, then {@link FileVisitor::visitFile} for all files in the directory and at the
     * end {@link FileVisitor::afterVisitDirectory}.
     *
     * @param visitor - object that is triggered
     * @param maxDepth - max recursion depth, 0 means: just list current directory
     * @param followSymLinks - follow sym links for files or directories?
     * @param visitNonRegular - visit files which are socket or pipes too?
     */
    void visit(FileVisitor &visitor, unsigned maxDepth, bool followSymLinks = true, bool visitNonRegular = false) const;

    /**
     * Search a file in a search-path which is defined by an environment variable (e.g. "PATH").
     * The methods splits the path which the OS specific path separator and checks each path if
     * the file in located there.
     * <P>
     * If the file is not found, an exception is thrown.
     *
     * @param envName - name of the environment variable
     * @param filename - name of the file to look for
     * @throws utils::KeyNotFound if the environment variable does not exist or the specified
     *         file was not found along the path.
     */
    static const File which(const char *envName, const char *filename);

    /**
     * Returns the full path to the current executable (/proc/self/exe on Linux).
     */
    static const File currentExe();

    /**
     * Returns the size in bytes: for directories this is the size in bytes
     * used in the filesystem by the directory blocks.
     */
    size_t size() const;

private:
    std::string path;
    std::string name;
    std::string fullname;
    void *data;
    mutable int errorCode;
    bool deleteOnDestroy;

    void init();
    bool hasAccess(unsigned flag) const;

    size_t listDirectory(FileList &result, FileFilter *filter = NULL) const;

    struct VisitOptions
    {
        FileVisitor &visitor;
        bool followSymLinks;
        bool visitNonRegular;

        VisitOptions(FileVisitor &_visitor, bool _followSymLinks, bool _visitNonRegular)
          : visitor(_visitor), followSymLinks(_followSymLinks), visitNonRegular(_visitNonRegular) { }
    };

    /**
     * Recursively visit all files / directories starting at the given directory.
     */
    FileVisitor::VisitResult recurseInto(const VisitOptions &options, unsigned maxDepth) const;
    FileVisitor::VisitResult visitMe(const VisitOptions &options, unsigned maxDepth) const;

    /**
     * Get file mode.
     */
    unsigned getMode() const;
};

/**
 * This is a helper File type which writes the current process id into the filename
 * given in the constructor argument. If writing the file fails, an exception is thrown.
 * <P>
 * The file is deleted when this object gets destroyed, so this object should currently
 * be located in the main() method.
 * <P>
 * Other than the File constructor, this constructor allows NULL or an empty filename:
 * in this case <b>NO</b> pid-file is written!
 */
class PidFile : public File
{
public:
    explicit PidFile(const char *name);
};

} /* namespace utils */

#endif /* FILE_H_ */
