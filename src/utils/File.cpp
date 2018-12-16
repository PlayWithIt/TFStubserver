/*
 * File.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>

#include <stdexcept>

#ifdef _WIN32
#include <Windows.h>
#include <process.h>
#define getpid _getpid
#else
#include <dirent.h>
#include <unistd.h>
#endif

#include "StringUtil.h"
#include "Exceptions.h"
#include "File.h"
#include "FileFilter.h"
#include "Log.h"

#ifndef PATH_MAX
#define PATH_MAX 2048
#endif

#define ERR_MOVED_AWAY 999999

#define STAT_BUF(obj) ((struct stat*) obj->data)


namespace utils {

const char File::PATH_SEP_CHAR = '/';
const char File::PATH_LIST_SEP_CHAR = ':';
const char * const File::PATH_SEP_STR = "/";

/**
 * Empty init: object cannot be used!
 */
File::File()
  : path()
  , name()
  , fullname()
  , data(new struct stat())
  , errorCode(1)
  , deleteOnDestroy(false)
{
    memset(data, 0, sizeof(struct stat));
}

/**
 * Write the pid file.
 */
PidFile::PidFile(const char *_name)
  : File(_name && _name[0] ? _name : "???")
{
    if (_name && _name[0])
    {
        setDeleteOnDestroy(true);

        FILE *f = fopen(getFullname().c_str(), "w");
        if (f == NULL) {
            Log::perror(getFullname());
            return;
        }
        fprintf(f, "%d", getpid());
        fclose(f);
        refresh();
    }
}

/**
 * Simple file with name
 */
File::File(const char *_name)
  : fullname(_name)
{
    init();
}


File::File(const std::string &_name)
  : fullname(_name)
{
    init();
}

/**
 * Create a file from a parent file and a filename
 */
File::File(const File &parentDir, const char *name)
{
    const std::string &parent = parentDir.isDirectory() ? parentDir.getFullname() : parentDir.getPath();
    if (*name == PATH_SEP_CHAR || parent.length() == 0 || parent[parent.length() - 1] == PATH_SEP_CHAR)
        fullname = parent + std::string(name);
    else
        fullname = parent + PATH_SEP_CHAR + std::string(name);
    init();
}

File::File(const File &parentDir, const std::string &name)
{
    const std::string &parent = parentDir.isDirectory() ? parentDir.getFullname() : parentDir.getPath();
    if ((name.length() > 0 && name[0] == PATH_SEP_CHAR) || parent.length() == 0 || parent[parent.length() - 1] == PATH_SEP_CHAR)
        fullname = parent + name;
    else
        fullname = parent + PATH_SEP_CHAR + name;
    init();
}

/**
 * Init name + path and stat data buffer.
 */
void File::init()
{
    errorCode = 0;
    deleteOnDestroy = false;

    size_t l = fullname.length();
    if (l == 0)
        throw std::invalid_argument("File::init(): the filename must have a length > 0");

    data = new struct stat();

    if (l > 1 && fullname[l-1] == PATH_SEP_CHAR)
    {
        // path != "/" but last char of name is '/' -> strip off the last '/'
        fullname.erase(--l);
    }

    size_t pos = fullname.find_last_of(PATH_SEP_CHAR);
    if (pos == std::string::npos)
        name = fullname;
    else if (l == 0)
        path = fullname;
    else if (l > 0)
    {
        path = fullname.substr(0, pos+1);  // normal directory with '/' at the end
        name = fullname.substr(pos + 1);
    }

    refresh();
}

/**
 * Init with assignment operator
 */
File::File(const File &other)
  : data(new struct stat())
{
    *this = other;
}

/**
 * Assignment operator.
 */
File& File::operator=(const File &other)
{
    path      = other.path;
    name      = other.name;
    fullname  = other.fullname;
    errorCode = other.errorCode;
    deleteOnDestroy = false;
    refresh();
    return *this;
}

/**
 * Move constructor.
 */
File::File(File &&other)
{
    path      = std::move(other.path);
    name      = std::move(other.name);
    fullname  = std::move(other.fullname);
    errorCode = other.errorCode;
    deleteOnDestroy = other.deleteOnDestroy;
    data      = other.data;

    other.deleteOnDestroy = false;
    other.errorCode = ERR_MOVED_AWAY;
    other.data = NULL;
}

/**
 * Move operator.
 */
File& File::operator=(File &&other)
{
    path      = std::move(other.path);
    name      = std::move(other.name);
    fullname  = std::move(other.fullname);
    errorCode = other.errorCode;
    deleteOnDestroy = other.deleteOnDestroy;
    memcpy(data, other.data, sizeof(struct stat));

    other.deleteOnDestroy = false;
    other.errorCode = ERR_MOVED_AWAY;
    return *this;
}

/**
 * Compare two File objects.
 */
bool File::operator==(const File& other) const
{
    // not equal if the status is not equal
    refresh();
    other.refresh();
    if (exists() != other.exists())
        return false;

    // if both exists: check file position
    if (exists())
    {
        struct stat* os = (struct stat*) other.data;
        return STAT_BUF(this)->st_dev == os->st_dev
            && STAT_BUF(this)->st_ino == os->st_ino;
    }

    // both don't exist: just compare the name.
    return fullname.compare(other.fullname) == 0;
}

/**
 * Release stat buffer
 */
File::~File()
{
    if (deleteOnDestroy && data)
        remove();
    delete STAT_BUF(this);
}

/**
 * Determine the absolute name, throws an exception if the file does not exist.
 */
std::string File::getAbsolutePath() const
{
    if (!exists())
        throw std::logic_error("Cannot determine the absolute path of a non-existing file");

    char path[PATH_MAX + 100];
    char *res = realpath(fullname.c_str(), path);
    if (!res)
    {
        int e = errno;
        snprintf(path, sizeof(path), "realpath(%s) failed", fullname.c_str());
        path[sizeof(path) - 1] = 0;
        throw utils::RuntimeError(path, e);
    }
    return path;
}

/**
 * Return the file mode if available.
 */
unsigned File::getMode() const
{
    if (errorCode == ERR_MOVED_AWAY)
        throw std::logic_error("Object has been moved, cannot be used any more!");
    return STAT_BUF(this)->st_mode;
}

/**
 * Returns the parent: if the current File is a file: the parent is a folder;
 * if the current File is a directory, the parent is the parent directory.
 * Returns the actual File if it is already the root directory.
 */
const File File::getParent() const
{
    std::string p = this->getAbsolutePath();
    if (p.length() <= 2)
        return *this;

    size_t pos = p.rfind(PATH_SEP_CHAR);
    if (pos == std::string::npos)
        return *this;

    return File(p.substr(0, pos));
}

/**
 * Returns a timestamp in milli-seconds since epoch when the file was last modified or
 * 0 if the file does not exist. Use {@link refresh()} before a call, if the file
 * object lives longer and the latest state on the harddrive should be re-read.
 */
uint64_t File::getLastModified() const
{
    if (!exists())
        return false;
    return STAT_BUF(this)->st_mtim.tv_sec * 1000 + STAT_BUF(this)->st_mtim.tv_nsec / 1000;
}

bool File::isAbsolute() const {
    return fullname.length() > 0 && fullname[0] == PATH_SEP_CHAR;
}

bool File::isDirectory() const {
    return S_ISDIR(getMode());
}

bool File::isRegularFile() const {
    return S_ISREG(getMode());
}

bool File::isSymlink() const {
    return S_ISLNK(getMode());
}

bool File::isSocket() const {
    return S_ISSOCK(getMode());
}

bool File::isPipe() const {
    return S_ISFIFO(getMode());
}

/**
 * Check the access rights.
 */
bool File::hasAccess(unsigned flag) const
{
    if (!exists())
        return false;
    int rc = access(fullname.c_str(), flag);
    if (rc != 0) {
        int e = errno;
        if (e != EACCES)
            Log::error(std::string("access() failed for ") + fullname + std::string(" with errno "), e);
        return false;
    }
    return true;
}

bool File::canExecute() const {
    return hasAccess(X_OK);
}

bool File::canRead() const {
    return hasAccess(R_OK);
}

bool File::canWrite() const {
    return hasAccess(W_OK);
}

/**
 * Returns the full path to the current executable (/proc/self/exe on Linux).
 */
const File File::currentExe()
{
    char buf[PATH_MAX];
    ssize_t s = readlink("/proc/self/exe", buf, sizeof(buf));
    if (s <= 0)
    {
        Log::perror("/proc/self/exe");
        throw std::runtime_error("Cannot read a symlink /proc/self/exe");
    }

    // terminate string
    buf[s] = 0;

    return File(buf);
}

/**
 * Lists the contents of the directory denoted by this File. If the current
 * File doesn't point to a directory or does not exist, an exception is thrown.
 *
 * @param result matching names will be put here
 * @param filter optional filter filter which can be used to filter filenames.
 * @return the number of items put into the result
 */
size_t File::listDirectory(FileList &result, FileFilter *filter) const
{
    if (!exists())
        throw std::logic_error("Cannot list a non-existing directory");
    if (!isDirectory())
        throw std::logic_error("Cannot list file which isn't a directory");
    if (!canRead())
        throw std::logic_error(std::string("No read access for directory ") + fullname);

    DIR *dir = opendir(fullname.c_str());
    if (dir == NULL) {
        throw std::logic_error("opendir() failed for a directory");
    }

    size_t numItems = 0;
    struct dirent *pEntry;

    pEntry = readdir(dir);
    while (pEntry)
    {
        // do not include "." and ".." in the list
        if (strcmp(pEntry->d_name, ".") && strcmp(pEntry->d_name, ".."))
        {
            File f(*this, pEntry->d_name);
            if (!filter || (*filter)(f))
            {
                result.push_back(std::move(f));
                ++numItems;
            }
        }
        pEntry = readdir(dir);
    }
    closedir(dir);
    return numItems;
}

/**
 * Find all files with a specific name.
 */
size_t File::find(FileList &result, const std::string &fileNameRegex, bool dirs, bool special) const
{
    FileFilterAnd filter(new FileTypeFilter(dirs, true, special), new FileRegexFilter(fileNameRegex));
    FileCollector collector(&result, filter);

    // visit all with maxDepth 100, follow symlinks
    visit(collector, 100, true, special);
    return collector.size();
}

/**
 * Search and release filter object.
 */
size_t File::list(FileList &result, FileFilter *filter) const
{
    try {
        size_t s = listDirectory(result, filter);
        delete filter;
        return s;
    }
    catch (...) {
        delete filter;
        throw;
    }
}

/**
 * Recursively visit all files / directories starting at the given directory.
 */
FileVisitor::VisitResult File::visitMe(const VisitOptions &options, unsigned maxDepth) const
{
    FileVisitor::VisitResult vr = FileVisitor::VisitResult::CONTINUE;

    if (isSymlink())
    {
        if (!options.followSymLinks)
            return vr;

        // visit the symlink location instead
        char buf[PATH_MAX];
        ssize_t s = readlink(getFullname().c_str(), buf, sizeof(buf));
        if (s <= 0)
        {
            Log::perror(getFullname());
            throw std::runtime_error(std::string("Cannot read a symlink ") + getFullname());
        }

        // terminate string
        buf[s] = 0;

        if (buf[0] == PATH_SEP_CHAR)
        {
            // absolute link location
            File linked(buf);
            return linked.visitMe(options, maxDepth);
        }
        else {
            // relative link location
            File linked(*this, buf);
            return linked.visitMe(options, maxDepth);
        }
    }

    if (isDirectory())
    {
        if (maxDepth > 0)
        {
            vr = options.visitor.visitDirectory(*this);
            if (vr == FileVisitor::VisitResult::STOP)
                return vr;
            if (vr == FileVisitor::VisitResult::STOP_DIR) {
                options.visitor.afterVisitDirectory(*this);
                return FileVisitor::VisitResult::CONTINUE;
            }

            // recurse with max depth - 1
            vr = recurseInto(options, maxDepth - 1);
            if (vr == FileVisitor::VisitResult::STOP)
                return vr;

            // what to do at end of of directory
            vr = options.visitor.afterVisitDirectory(*this);
        }
    }
    else {
        if (options.visitNonRegular || isRegularFile())
            vr = options.visitor.visitFile(*this);
        else
            vr = FileVisitor::VisitResult::CONTINUE;
    }

    if (vr == FileVisitor::VisitResult::STOP)
        return vr;
    if (vr == FileVisitor::VisitResult::STOP_DIR)
        return FileVisitor::VisitResult::CONTINUE;

    return vr;
}

/**
 * Recursively visit all files / directories starting at the given directory.
 */
FileVisitor::VisitResult File::recurseInto(const VisitOptions &options, unsigned maxDepth) const
{
    FileList result;
    listDirectory(result, NULL);

    for (auto it : result)
    {
        FileVisitor::VisitResult vr = it.visitMe(options, maxDepth);

        if (vr == FileVisitor::VisitResult::STOP)
            return vr;
        if (vr == FileVisitor::VisitResult::STOP_DIR)
            return FileVisitor::VisitResult::CONTINUE;
    }
    return FileVisitor::VisitResult::CONTINUE;
}


void File::visit(FileVisitor &visitor, unsigned maxDepth, bool followSymLinks, bool visitNonRegular) const
{
    if (!exists() || !isDirectory())
        throw std::logic_error(std::string("'visit()' may only be called for existing directories, but '") + fullname + "' is invalid!");

    VisitOptions options(visitor, followSymLinks, visitNonRegular);
    visitMe(options, maxDepth);
}

/**
 * stat() the file again.
 */
void File::refresh() const
{
    // if a file is a link: stat() will follow the link but lstat() not !
    errorCode = lstat(fullname.c_str(), STAT_BUF(this));
    if (errorCode != 0)
    {
        errorCode = errno;

        // errno is not thread save -> if we still have 0 use a default...
        if (errorCode == 0)
            errorCode = ENOENT;
    }
}

/**
 * Remove the file or directory.
 */
bool File::remove()
{
    if (!exists()) {
        return false;
    }
    int rc = ::remove(fullname.c_str());
    if (rc != 0)
        return false;
    refresh();
    return true;
}

/**
 * Renames or moves the current file to the new name or position. If the new
 * file exists, it will only be overwritten, if the parameter is set.
 * <p>
 * This method throws an exception in case of any error while renaming (e.g.
 * if the current File does not exist).
 *
 * @param newNameAndPath new location or name, if the name does not contain a path
 *        the file is moved to the current working directory.
 * @param overwrite overwrite existing files?
 */
bool File::renameTo(const char *newNameAndPath, bool overwrite)
{
    if (!exists())
        throw std::logic_error("Source files does not exist, cannot rename");

    File target(newNameAndPath);
    if (target.exists())
    {
        if (!overwrite)
            return false;
        if (!target.remove())
            return false;
    }

    int rc = rename(fullname.c_str(), newNameAndPath);
    if (rc != 0)
    {
        char buffer[1024];
        int e = errno;
        snprintf(buffer, sizeof(buffer), "Cannot rename to %s", newNameAndPath);
        buffer[sizeof(buffer) - 1] = 0;
        throw utils::RuntimeError(buffer, e);
    }

    *this = target;
    refresh();
    return true;
}

// return the file / directory size
size_t File::size() const {
    return STAT_BUF(this)->st_size;
}

/**
 * Create an empty file if it does not exist.
 */
bool File::touch()
{
    if (exists()) {
        return true;
    }
    if (errorCode == ERR_MOVED_AWAY)
        return false;

    FILE *f = fopen(fullname.c_str(), "a");
    if (!f)
        return false;
    fclose(f);
    refresh();
    return true;
}

/**
 * Similar as the 'which' command in the command line
 */
const File File::which(const char *envName, const char *filename)
{
    const char *env = getenv(envName);
    if (!env)
        throw KeyNotFound("Environment variable", envName);

    // split path list and look into every directory of the list
    std::vector<std::string> paths;
    for (auto it : strings::split(env, PATH_LIST_SEP_CHAR, paths))
    {
        File dir(it);
        if (!dir.exists())
            continue;

        File result(dir, filename);
        if (result.exists()) {
            return result;
        }
    }
    throw KeyNotFound("File not found along path", filename);
}

} /* namespace utils */
