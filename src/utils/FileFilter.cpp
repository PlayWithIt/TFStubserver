/*
 * FileFilter.cpp
 *
 * Copyright (C) 2014-2021 Holger Grosenick
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

#ifndef _WIN32
#include <elf.h>
#endif
#include <iostream>

#include "File.h"
#include "FileFilter.h"
#include "FileObject.h"
#include "StringUtil.h"


namespace utils {


// ---
FileFilter::~FileFilter() { }

/**
 * Functor to check if the file should be returned as result.
 */
bool FileSuffixFilter::operator()(const File& file) {
    return strings::endsWith(file.getName(), filenamePattern);
}

/**
 * Functor to check if the file should be returned as result.
 */
bool FilePrefixFilter::operator()(const File& file) {
    return file.getName().compare(0, filenamePattern.length(), filenamePattern) == 0;
}

/**
 * Functor to check if the file should be returned as result.
 */
bool FileRegexFilter::operator()(const File& file) {
    return std::regex_match(file.getName(), re);
}

/**
 * Just filter by type, all names.
 */
FileTypeFilter::FileTypeFilter(bool _dirs, bool _files, bool _other)
  : dirs(_dirs)
  , files(_files)
  , other(_other)
{ }

/**
 * Functor to check if the file should be returned as result.
 */
bool FileTypeFilter::operator()(const File& file)
{
    if (file.isRegularFile())
        return files;
    if (file.isDirectory())
        return dirs;
    return other;
}

/**
 * If the argument is true, the operator returns true for ELF files,
 * if it is false, it returns false in case of ELF files.
 */
FileFilterElf::FileFilterElf(bool include)
  : include(include)
{ }

/**
 * Return the value of 'include' in case of ELF files and '!include'
 * for all others.
 */
bool FileFilterElf::operator()(const File& file)
{
#ifdef _WIN32
    return !include;
#else
    FileObject obj(file);
    if (!obj.isOpen()) {
        // std::cerr << file.getFullname() << ": open error\n";
        return !include;
    }

    char header[16];
    if (obj.read(header, 16) != 16)
        return !include;

    if (header[0] != ELFMAG0 || header[1] != ELFMAG1 || header[2] != ELFMAG2  || header[3] != ELFMAG3) {
        // std::cerr << file.getFullname() << ": wrong header\n";
        return !include;
    }
    if (header[EI_CLASS] < ELFCLASS32 || header[EI_CLASS] > ELFCLASS64) {
        // std::cerr << file.getFullname() << ": wrong class\n";
        return !include;
    }
    if (header[EI_VERSION] != EV_CURRENT) {
        // std::cerr << file.getFullname() << ": wrong version\n";
        return !include;
    }
    return include;
#endif
}

/**
 * Delete first and second!
 */
FileFilterAnd::~FileFilterAnd()
{
    delete first;
    delete second;
}

/**
 * Filter by condition of 'first() AND second()'
 */
bool FileFilterAnd::operator()(const File& file)
{
    return (*first)(file) && (*second)(file);
}

/**
 * Delete first and second!
 */
FileFilterOr::~FileFilterOr()
{
    delete first;
    delete second;
}

/**
 * Filter by condition of 'first() OR second()'
 */
bool FileFilterOr::operator()(const File& file)
{
    return (*first)(file) || (*second)(file);
}

} /* namespace brickapi */
