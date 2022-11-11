/*
 * StringUtil.cpp
 *
 * Copyright (C) 2013-2022 Holger Grosenick
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

#include <math.h>
#include <string.h>
#include <sstream>

#include "StringUtil.h"
#include "Exceptions.h"

namespace utils {
namespace strings {

/**
 * Helper to split a string using a given delimiter
 */
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


/**
 * Parse a double where the decimal point must be a '.', so NOT locale specific
 * and only the format NNN.NNNeYY is supported.
 *
 * Throws an exception if parsing fails!
 */
double parseDouble(const char *s)
{
    bool negate = false;
    bool digits = false;
    double res = 0;
    double factor = 0.1;

    if (!s) {
        throw ValueFormatError("Cannot convert NULL to double");
    }

    if (*s == '-') {
        negate = true;
        ++s;
    }
    if (*s == '+') {
        // just skip
        ++s;
    }

    while ((*s >= '0' && *s <= '9') || *s == '.') {
        char c = *s;
        ++s;
        if (c == '.') {
            if (digits)
                throw ValueFormatError(std::string("Multiple '.' in string, cannot convert to double '") + s + "'");
            digits = true;
            continue;
        }
        if (!digits)
            res = res * 10.0 + (c - '0');
        else {
            res += (factor * (c - '0'));
            factor *= 0.1;
        }
    }

    if (*s == 'e' || *s == 'E') {
        // exponent
        ++s;
        int exp = 1;
        if (*s == '+')
            ++s;
        else if (*s == '-') {
            exp = -1;
            ++s;
        }
        if (*s == 0)
            throw ValueFormatError(std::string("Missing exponent string, cannot convert to double '") + s + "'");

        // first digit
        exp *= (*s - '0');
        ++s;

        // other digits
        while (*s >= '0' && *s <= '9') {
            exp = (10 * exp) + (*s - '0');
            ++s;
        }

        res *= pow(10.0, exp);
    }

    // end of string not reached ?
    if (*s)
        throw ValueFormatError(std::string("Cannot convert to double '") + s + "'");

    return negate ? -res : res;
}

double parseDouble(const std::string &s) {
    return parseDouble(s.c_str());
}

/**
 * Replaces all occurrences of 'old' with the text 'newStr'
 * @return in input string.
 */
std::string& replace(std::string& inOut, const char *old, const char *newStr)
{
    size_t pos;
    size_t oldLen = strlen(old);

    if ((pos = inOut.find(old)) != std::string::npos) {
        inOut.replace(pos, oldLen, newStr);
    }
    return inOut;
}


/**
 * Replaces all occurrences of 'old' with the text 'newStr'
 * @return in input string.
 */
std::string& replaceAll(std::string& inOut, const char *old, const char *newStr)
{
    size_t pos = 0;
    size_t oldLen = strlen(old);
    size_t newLen = strlen(newStr);

    while((pos = inOut.find(old, pos)) != std::string::npos) {
        inOut.replace(pos, oldLen, newStr);
        pos += newLen;
    }
    return inOut;
}

/**
 * Does a string start with the given prefix string?
 */
bool startsWith(const std::string &s, const char *prefix)
{
    // search empty?
    if (*prefix == 0)
        return true;

    size_t last = s.length();
    for (size_t i = 0; i < last; ++i)
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

/**
 * We could use std::locale here but unfortunately if the locale de_DE is not
 * defined by default, an admin must install/create it or the program would crash!
 */
inline void myLower(char &in)
{
    if (in >= 'A' && in <= 'Z')
        in += 32;
    else {
        switch ((uint8_t)in) {
        case UCHAR_Auml:
            in = (char)UCHAR_auml;
            break;
        case UCHAR_Ouml:
            in = (char)UCHAR_ouml;
            break;
        case UCHAR_Uuml:
            in = (char)UCHAR_uuml;
            break;
        }
    }
}

inline void myUpper(char &in)
{
    if (in >= 'a' && in <= 'z')
        in -= 32;
    else {
        switch ((uint8_t)in) {
        case UCHAR_auml:
            in = (char)UCHAR_Auml;
            break;
        case UCHAR_ouml:
            in = (char)UCHAR_Ouml;
            break;
        case UCHAR_uuml:
            in = (char)UCHAR_Uuml;
            break;
        }
    }
}

/**
 * Converts the given String to lower case (using ASCII locale) and returns
 * the address of the string.
 */
std::string& toLower(std::string &s)
{
    for (auto it = s.begin(); it != s.end(); ++it)
        myLower(*it);
    return s;
}

/**
 * Converts the given String to lower case (using ASCII locale) and returns
 * the address of the string.
 */
char* toLower(char *s)
{
    for (char *it = s; *it; ++it)
        myLower(*it);
    return s;
}

/**
 * Converts the given String to upper case (using ASCII locale) and returns
 * the address of the string.
 */
std::string& toUpper(std::string &s)
{
    for (auto it = s.begin(); it != s.end(); ++it)
        myUpper(*it);
    return s;
}

/**
 * Converts the given String to upper case (using ASCII locale) and returns
 * the address of the string.
 */
char* toUpper(char *s)
{
    for (char *it = s; *it; ++it)
        myUpper(*it);
    return s;
}

/**
 * Prints a double with the given number of digits into a char buffer using the
 * given decimal separator (must be '.' or ','). If sep is neither '.' nor ','
 * the decimal separator remains unchanged and is locale specific.
 *
 * The buffer must be large enough to hold a double with max value (e.h. 512).
 */
void toString(char buf[512], double d, unsigned digits, char sep)
{
    char fmt[32];

    if (!buf)
        throw std::invalid_argument("buffer may not be NULL");
    if (digits > 20)
        throw OutOfRange("Number of digits may not be larger than 20", digits, 20);

    snprintf(fmt, sizeof(fmt), "%%.%df", digits);   // this should not create an overflow
    snprintf(buf, 200, fmt, d);                     // double can be very long
    buf[199] = 0;
    if (sep == '.') {
        // change decimal separator
        char *pos = strchr(buf, ',');
        if (pos)
            *pos = '.';
    }
    else if (sep == ',') {
        // change decimal separator
        char *pos = strchr(buf, '.');
        if (pos)
            *pos = ',';
    }
}

/**
 * Similar to std::to_string this method converts a double into a string, but
 * the number of digits can be controlled. The decimal separator is locale
 * specific ('.' or ',').
 */
std::string toString(double d, unsigned digits)
{
    char buff[512];
    toString(buff, d, digits, 0);
    return buff;
}

/**
 * Similar to std::to_string this method converts a double into a string, but
 * the number of digits can be controlled and the decimal separator must be
 * specific too (can be used to always print with '.' for example).
 */
std::string toString(double d, unsigned digits, char sep)
{
    char buff[512];
    toString(buff, d, digits, sep);
    return buff;
}

/**
 * Does a string end with the given suffix string?
 */
bool endsWith(const std::string &whole, const std::string &suffix)
{
    size_t suffix_l = suffix.length();
    if (suffix_l == 0)
        return true;

    // string too short?
    size_t whole_l = whole.length();
    if (whole_l < suffix_l)
        return false;

    return (0 == whole.compare (whole_l - suffix_l, suffix_l, suffix));
}

/**
 * Returns a locale specific error text for the given error code (see 'errno'). This
 * method is thread-safe.
 */
std::string strerror(int err)
{
    char buff[2048];
    buff[0] = 0;
#ifdef _WIN32
    strerror_s(buff, sizeof(buff), err);
    return buff;
#else
    return strerror_r(err, buff, sizeof(buff));
#endif
}

inline bool isSpace(char ch) {
    return (ch == ' ' || ch == '\t');
}

/**
 * Removes leading and trailing blanks and tabs.
 */
std::string& trim(std::string &in)
{
    size_t l = in.length();

    // empty string
    if (l == 0)
        return in;

    size_t first = 0;
    while (first < l && isSpace(in[first]))
        ++first;

    // only spaces?
    if (first == l) {
        in.clear();
        return in;
    }

    while (l-1 > first && isSpace(in[l-1]))
        --l;

    in = in.substr(first, l - first);
    return in;
}

} /* namespace strings */
} /* namespace utils */
