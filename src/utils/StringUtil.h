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
namespace strings
{
#define UCHAR_auml  0344
#define UCHAR_ouml  0366
#define UCHAR_uuml  0374
#define UCHAR_Auml  0304
#define UCHAR_Ouml  0326
#define UCHAR_Uuml  0334

    /**
     * Split a string using a given delimiter into a vector of strings.
     * @param s the string to split
     * @param delim the delimiter
     * @param elems the string split will be stored here: the elements will be added,
     *        the vector is not cleared before adding new elements.
     * @return the address of the input vector
     */
    std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);

    /**
     * Does a string start with the given prefix string?
     */
    bool startsWith(const std::string &s, const char *prefix);

    inline bool startsWith(const std::string &s, const std::string &prefix) {
        return startsWith(s, prefix.c_str());
    }

    /**
     * Replaces the first occurrence of 'old' with the text 'newStr'
     * @return in input string.
     */
    std::string& replace(std::string& inOut, const char *old, const char *newStr);

    /**
     * Replaces all occurrences of 'old' with the text 'newStr'
     * @return in input string.
     */
    std::string& replaceAll(std::string& inOut, const char *old, const char *newStr);

    /**
     * Does a string end with the given suffix string?
     */
    bool endsWith(const std::string &s, const std::string &suffix);

    /**
     * Converts the given String to lower case (using ASCII locale) and returns
     * the address of the string.
     */
    std::string& toLower(std::string &s);
    char *toLower(char *s);

    /**
     * Converts the given String to upper case (using ASCII locale) and returns
     * the address of the string.
     */
    std::string& toUpper(std::string &s);
    char *toUpper(char *s);

    /**
     * Returns a locale specific error text for the given error code (see 'errno'). This
     * method is thread-safe.
     */
    std::string strerror(int err);

    /**
     * Removes leading and trailing blanks and tabs.
     */
    std::string& trim(std::string &in);

} /* namespace strings */
} /* namespace utils */

#endif /* STRINGUTIL_H_ */
