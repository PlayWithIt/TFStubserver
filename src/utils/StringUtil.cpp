/*
 * StringUtil.cpp
 *
 * Copyright (C) 2013 Holger Grosenick
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

#include <sstream>
#include "StringUtil.h"

namespace utils {

StringUtil::StringUtil() {

}

/**
 * Helper to split a string using a given delimiter
 */
std::vector<std::string>& StringUtil::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

/**
 * Does a string start with the given prefix string?
 */
bool StringUtil::startsWith(const std::string &s, const char *prefix)
{
    // search empty?
    if (*prefix == 0)
        return true;

    unsigned last = s.length();
    for (unsigned i = 0; i < last; ++i)
    {
        if (s[i] != *prefix)
            return false;
        ++prefix;
        // end of search string
        if (*prefix == 0)
            return true;
    }
    return false;
}

} /* namespace utils */
