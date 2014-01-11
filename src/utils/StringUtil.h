/*
 * StringUtil.h
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
#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

#include <string>
#include <vector>

namespace utils {

/**
 * A helper class that offers only static methods for string functionality.
 */
class StringUtil
{
    StringUtil();
    StringUtil(const StringUtil &) = delete;
    StringUtil& operator=(const StringUtil &) = delete;

public:

    /**
     * Split a string using a given delimiter into a vector of strings.
     * @param s the string to split
     * @param delim the delimiter
     * @param elems the string split will be stored here: the elements will be added,
     *        the vector is not cleared before adding new elements.
     * @return the address of the input vector
     */
    static std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);

    /**
     * Does a string start with the given prefix string?
     */
    static bool startsWith(const std::string &s, const char *prefix);
    static bool startsWith(const std::string &s, const std::string &prefix) {
        return startsWith(s, prefix.c_str());
    }
};

} /* namespace utils */

#endif /* STRINGUTIL_H_ */
