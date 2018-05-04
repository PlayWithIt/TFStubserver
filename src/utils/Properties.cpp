/*
 * Properties.cpp
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>    // std::sort
#include <vector>
#include <string.h>
#include <errno.h>

#include "Properties.h"

namespace utils {

/**
 * Default constructor with empty map
 */
Properties::Properties() {
}

/**
 * Constructor which loads the data from a file.
 */
Properties::Properties(const char *filename) {
    load(filename);
}

/**
 * Constructor which loads the data from a file.
 */
Properties::Properties(const std::string& filename) {
    load(filename.c_str());
}

/**
 * Build properties out of a single string which has the format:<pre>
 * key1=value1,key2=value2,key3=value3
 * </pre>
 * The ',' is the separator which must be passed to this constructor.
 */
Properties::Properties(const std::string &data, char separator)
{
    size_t last = data.length();

    for (size_t i = 0; i < last; ++i)
    {
        // skip blanks before key
        while (data[i] == ' ') {
            ++i;
            if (i >= last)
                return;
        }
        size_t start = i;

        // find end of key
        while (data[i] != '=') {
            ++i;
            if (i >= last)
                throw std::logic_error("No '=' found after key");
        }
        size_t end = i;
        while (data[end-1] == ' ')
            --end;

        std::string key = data.substr(start, end - start);

        // skip blanks before key
        while (++i < last && data[i] == ' ') {
            // skip blanks before value
        }

        std::ostringstream os;
        do {
            char c = data[i];
            if (c == '\\') {
                os << data[++i];
            }
            else
                os << c;
        } while (++i < last && data[i] != separator);

        // save value
        put(key, os.str());
    }
}

/**
 * Nothing special
 */
Properties::~Properties() noexcept {
}

/**
 * Remove all items of the container.
 */
void Properties::clear() noexcept {
    values.clear();
}

/**
 * Load the properties out of a file, one value per line, '#' is used for
 * comments.
 */
void Properties::load(const char *filename)
{
    std::ifstream input(filename);

    // open file
    if (!input.is_open()) {
        throw FileOpenError(true, filename);
    }

    // now parse text
    std::string line;
    int lineNo = 0;
    while (std::getline(input, line))
    {
        ++lineNo;
        size_t l = line.length();

        // trim key
        size_t start = 0;
        while (start < l && line[start] == ' ')
            ++start;

        // comment or empty line
        if (l == 0 || line[start] == '#' || start == l)
            continue;

        size_t pos = line.find('=');
        if (pos >= l || pos == 0) {
            std::cerr << "Error in " << filename << '(' << lineNo << "): '=' not found or at invalid location!\n";
            continue;
        }

        size_t end = pos-1;
        while (end > 0 && line[end] == ' ')
            --end;
        std::string left = line.substr(start, end+1);

        // trim values
        start = pos + 1;
        while (start < l && line[start] == ' ')
            ++start;
        end = l - 1;
        while (end > 0 && line[end] == ' ')
            --end;

        std::string right;

        // continuation char at the end of the line?
        if (end > 0 && line[end] == '\\') {
            right = line.substr(start, end - start);
            while (std::getline(input, line)) {

                l = line.length();
                start = 0;
                while (start < l && line[start] == ' ')
                    ++start;
                if (l == 0 || line[l - 1] != '\\') {
                    right += line.substr(start);
                    break;
                }
                right += line.substr(start, l - start - 1);
            }

        }
        else
            right = line.substr(start, end - start + 1);

        // insert new value
        put(left, right);
    }

    input.close();
}

/**
 * Remove an item.
 */
bool Properties::remove(const std::string &key)
{
    return (values.erase(key) != 0);
}

/**
 * Write data into a file.
 */
void Properties::save(const char *filename, bool sort) const
{
    std::ofstream output;
    output.open(filename);

    if (!output.is_open()) {
        throw FileOpenError(false, filename);
    }
    write(output, sort);
    output.close();
}

/**
 * Finds a subset of properties whose key start with the given prefix and
 * puts this subset into a new set of properties or adds them to a passed
 * Properties object.
 *
 * @param prefix the prefix to find in all existing keys
 * @param prugePrefix if true, the given prefix is removed from the key before
 *        the item is inserted into the result properties.
 * @param out optional pointer to Properties where the result should be added.
 *        If this parameter is NULL, a new object is created and returned as
 *        result, otherwise 'out' is returned as result.
 */
Properties* Properties::subset(const std::string &prefix, bool purgePrefix, Properties *out)
{
    Properties* result = out ? out : new Properties();
    size_t keyLen = prefix.size();

    for (auto it = values.cbegin(); it != values.cend(); ++it)
    {
        if (it->first.compare(0, keyLen, prefix) != 0)
            continue;
        if (purgePrefix)
            result->put(it->first.substr(keyLen), it->second);
        else
            result->put(it->first, it->second);
    }
    return result;
}

/**
 * Append test to an already opened output stream.
 */
std::ostream& Properties::write(std::ostream& output, bool sort) const
{
    output << "#\n#\n\n";

    if (sort)
    {
        std::vector<std::string> keys;
        for (auto it = values.cbegin(); it != values.cend(); ++it) {
            keys.push_back(it->first);
        }
        std::sort(keys.begin(), keys.end());

        for (auto it : keys)
        {
            auto item = values.find(it);
            if (item == values.end())
                throw std::logic_error(std::string("Key not found any more: '") + it + "'");
            output << it << '=' << item->second << '\n';
        }
        return output;
    }

    // unsorted but fast
    for (auto it = values.cbegin(); it != values.cend(); ++it)
    {
        output << it->first << '=' << it->second << '\n';
    }
    return output;
}

/**
 * Return a value for the given key, returns NULL if the key is
 * not present in the container.
 */
const char* Properties::get(const char *key) const noexcept
{
    auto it = values.find(key);
    if (it == values.cend())
        return NULL;
    return it->second.c_str();
}

/**
 * Return a value for the given key or the default value if the
 * key is not present in the container.
 */
const char* Properties::get(const char *key, const char *defaultValue) const noexcept
{
    const char *res = get(key);
    return res ? res : defaultValue;
}

/**
 * Return a value for the given key or the default value if the key is not
 * present in the container, but if the value was not in the container, the
 * default value will be added so that the container afterwards has this key.
 */
const char* Properties::getOrSet(const char *key, const char *defaultValue) noexcept
{
    const char *res = get(key);
    if (!res) {
        put(key, defaultValue);
        return defaultValue;
    }
    return res;
}

/**
 * Return a value for the given key, throws an exception if the given
 * key is not present in the container or the value has an invalid format.
 */
bool Properties::getBool(const std::string &key) const
{
    const char *v = get(key);
    if (!v)
        throw KeyNotFound(key);

    if (strcmp(v, "true") == 0)
        return true;
    if (strcmp(v, "false") != 0)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Invalid bool value: '%s', key was '%s'", v, key.c_str());
        buffer[sizeof(buffer) - 1] = 0;
        throw ValueFormatError(buffer);
    }
    return false;
}

/**
 * Return a value for the given key, throws an exception if the given
 * key is not present in the container or the value has an invalid format.
 */
bool Properties::getBool(const std::string &key, bool defaultValue) const
{
    try {
        return getBool(key);
    }
    catch (const KeyNotFound &) {
        return defaultValue;
    }
}

/**
 * Return a value for the given key, throws an exception if the given
 * key is not present in the container or the value has an invalid format.
 */
int Properties::getInt(const std::string &key) const
{
    const char *v = get(key);
    if (!v)
        throw KeyNotFound(key);
    return atoi(v);
}

/**
 * Return a value for the given key, throws an exception if the given
 * key is not present in the container or the value has an invalid format.
 */
int Properties::getInt(const std::string &key, int defaultValue) const
{
    try {
        return getInt(key);
    }
    catch (const KeyNotFound &) {
        return defaultValue;
    }
}

/**
 * Return a value for the given key, throws an exception if the given
 * key is not present in the container or the value has an invalid format.
 */
double Properties::getDouble(const std::string &key) const
{
    const char *v = get(key);
    if (!v)
        throw KeyNotFound(key);

    char *end = const_cast<char*>(v);
    double res = strtod(v, &end);
    int l = strlen(v);
    if (end != (v+l))
        throw ValueFormatError(std::string("Cannot convert to double '") + v + "'");
    return res;
}

/**
 * Return a value for the given key, throws an exception if the given
 * key is not present in the container or the value has an invalid format.
 */
double Properties::getDouble(const std::string &key, double defaultValue) const
{
    try {
        return getDouble(key);
    }
    catch (const KeyNotFound &) {
        return defaultValue;
    }
}

/**
 * Add a value to the container: overwrite an existing value if it is
 * already in the map.
 */
void Properties::put(const char *key, const char *value)
{
    std::pair<TStringMap::iterator, bool> res = values.insert(std::make_pair(key, value));
    if (false == res.second)
        (res.first)->second = value;
}

void Properties::put(const std::string &key, const std::string &value)
{
    std::pair<TStringMap::iterator, bool> res = values.insert(std::make_pair(key, value));
    if (false == res.second)
        (res.first)->second = value;
}

void Properties::put(const std::string &key, bool b)
{
    static std::string TRUE("true");
    static std::string FALSE("false");
    put(key, b ? TRUE : FALSE);
}

void Properties::put(const std::string &key, int i)
{
    char buffer[32];
    sprintf(buffer, "%d", i);
    put(key, std::string(buffer));
}

void Properties::put(const std::string &key, unsigned i)
{
    char buffer[32];
    sprintf(buffer, "%u", i);
    put(key, std::string(buffer));
}

void Properties::put(const std::string &key, double d)
{
    char buffer[32];
    sprintf(buffer, "%f", d);
    put(key, std::string(buffer));
}

/**
 * Put all properties from other into the current object, this would
 * replace existing items.
 */
void Properties::put(const Properties &other)
{
    for (auto it = other.cbegin(); it != other.cend(); ++it)
    {
        put(it->first, it->second);
    }
}

} /* namespace utils */
