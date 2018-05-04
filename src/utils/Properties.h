/*
 * Properties.h
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

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <unordered_map>
#include <iostream>

#include "Object.h"
#include "Exceptions.h"


namespace utils {

/**
 * A simple class which can hold name/value pairs and load/save
 * this from file.
 */
class Properties : public Object
{
    typedef std::unordered_map<std::string, std::string> TStringMap;
    TStringMap values;

public:
    /** create empty property set */
    Properties();
    virtual ~Properties() noexcept;

    /**
     * Load properties from file.
     * @param filename the path to the file
     * @throws FileOpenError if the file cannot be opened
     */
    Properties(const char *filename);

    /**
     * Load properties from file.
     * @param filename the path to the file
     * @throws FileOpenError if the file cannot be opened
     */
    Properties(const std::string &filename);

    /**
     * Build properties out of a single string which has the format:<pre>
     * key1=value1,key2=value2,key3=value3
     * </pre>
     * The ',' is the separator which must be passed to this constructor.
     */
    Properties(const std::string &data, char separator);

    /**
     * Finds a subset of properties whose key start with the given prefix and
     * puts this subset into a new set of properties or adds them to a passed
     * Properties object.
     *
     * @param prefix the prefix to find in all existing keys
     * @param purgePrefix if true, the given prefix is removed from the key before
     *        the item is inserted into the result properties.
     * @param out optional pointer to Properties where the result should be added.
     *        If this parameter is NULL, a new object is created and returned as
     *        result, otherwise 'out' is returned as result.
     */
    Properties* subset(const std::string &prefix, bool purgePrefix = true, Properties *out = NULL);

    /**
     * Remove all items of the container.
     */
    void clear() noexcept;

    /**
     * Enable iterating through the properties.
     */
    TStringMap::const_iterator cbegin() const {
        return values.cbegin();
    }
    TStringMap::const_iterator cend() const {
        return values.cend();
    }

    /**
     * Return a value for the given key, returns NULL if the key is not present
     * in the container.
     */
    const char *get(const char *key) const noexcept;
    const char *get(const std::string &key) const noexcept {
        return get(key.c_str());
    }

    /**
     * Return a value for the given key or the default value if the key is not
     * present in the container.
     */
    const char *get(const char *key, const char *defaultValue) const noexcept;
    const char *get(const std::string &key, const char *defaultValue) const noexcept {
        return get(key.c_str(), defaultValue);
    }

    /**
     * Return a value for the given key or the default value if the key is not
     * present in the container, but if the value was not in the container, the
     * default value will be added so that the container afterwards has this key.
     */
    const char *getOrSet(const char *key, const char *defaultValue) noexcept;
    const char *getOrSet(const std::string &key, const char *defaultValue) noexcept {
        return getOrSet(key.c_str(), defaultValue);
    }

    /**
     * Return a value for the given key, throws an exception if the given
     * key is not present in the container or the value has an invalid format.
	 * @throw KeyNotFound ValueFormatError
     */
    bool getBool(const std::string &key) const;

    /**
     * Return a value for the given key, throws an exception if the given
     * key is not present in the container or the value has an invalid format.
     * @throw ValueFormatError
     */
    bool getBool(const std::string &key, bool defaultValue) const;

    /**
     * Return a value for the given key, return a default value if the key is not
     * present in the container; throws an exception if the value has an invalid format.
     * @throw KeyNotFound, ValueFormatError
     */
    int getInt(const std::string &key) const;

    /**
     * Return a value for the given key, throws an exception if the given
     * key is not present in the container or the value has an invalid format.
     * @throw ValueFormatError
     */
    int getInt(const std::string &key, int defaultValue) const;

    /**
     * Return a value for the given key, throws an exception if the given
     * key is not present in the container or the value has an invalid format.
     * @throw KeyNotFound, ValueFormatError
     */
    double getDouble(const std::string &key) const;

    /**
     * Return a value for the given key, return a default value if the key is not
     * present in the container; throws an exception if the value has an invalid format.
     * @throw KeyNotFound, ValueFormatError
     */
    double getDouble(const std::string &key, double defaultValue) const;

    /**
     * This the properties map emtpy?
     */
    bool isEmpty() const {
        return values.empty();
    }

    /**
     * Loads properties from file: current values are NOT cleared before adding new ones.
     * Throws an exception in case of error.
     */
    void load(const char *filename);
    void load(const std::string &filename) {
        load(filename.c_str());
    }

    /**
     * Saves the properties into a file: replaces the file if this already exists.
     * By default, the keys are not sorted, but if 'sort' is set to true the keys
     * will be sorted in the output (costs extra time).
     * Throws an exception in case of error.
     */
    void save(const char *filename, bool sort = false) const;

    std::ostream& write(std::ostream&, bool sort = false) const;

    /**
     * Remove an item from the properties, return true if the key exists, false otherwise.
     */
    bool remove(const std::string &key);

    /**
     * Add a new key/value pair.
     */
    void put(const char *key, const char *value);
    void put(const std::string &key, const std::string &value);
    void put(const std::string &key, bool value);
    void put(const std::string &key, double value);
    void put(const std::string &key, int value);
    void put(const std::string &key, unsigned value);

    /**
     * Put all properties from other into the current object, this would
     * replace existing items.
     */
    void put(const Properties &other);

    /**
     * Returns the number of elements in the container.
     */
    size_t size() const {
        return values.size();
    }
};

} /* namespace utils */


/**
 * Simple stream syntax.
 */
inline std::ostream& operator<<(std::ostream& os, const utils::Properties &p) {
    return p.write(os);
}


#endif /* PROPERTIES_H_ */
