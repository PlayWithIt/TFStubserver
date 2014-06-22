/*
 * CyclicVector.h
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
#ifndef CYCLICVECTOR_H_
#define CYCLICVECTOR_H_

#include <stdexcept>

namespace utils {

/**
 * A vector with:<br>
 * - a constant max size<br>
 * - which allows to reuse items<br>
 * - doesn't have a fixed begin .. end sequence, but makes a kind of rolling vector<br>
 * - => it cannot overflow but has a limited max size
 * <P>
 * So if the end of the vector is reached, the first element gets replaced with the
 * next push_back operation.
 */
template <typename _Tp> class CyclicVector
{
    typedef _Tp           value_type;
    typedef _Tp*          pointer;
    typedef _Tp&          reference;
    typedef const _Tp*    const_pointer;
    typedef const _Tp&    const_reference;

    unsigned allocated;
    unsigned used;
    unsigned first;
    unsigned last;

    pointer vectorStart;

public:

    /**
     * Const iterator in the Vector
     */
    class const_iterator
    {
        const CyclicVector<value_type> &parent;
        unsigned           pos;
        int                direction;

    public:
        const_iterator(const CyclicVector<value_type> &_parent, unsigned _pos, int _direction)
        : parent(_parent), pos(_pos), direction(_direction)
        { }

        bool operator!=(const const_iterator& other) const {
            return (pos != other.pos) || (&parent != &other.parent);
        }

        bool operator==(const const_iterator& other) const {
            return (pos == other.pos) && (&parent == &other.parent);
        }

        /**
         * Prefix increment
         */
        const_iterator& operator++() {
            pos += direction;
            return *this;
        }

        /**
         * Postfix increment
         */
        const_iterator operator++(int x) {
            const_iterator tmp = *this;
            pos += direction;
            return tmp;
        }

        const_reference operator*() const {
            return parent[pos];
        }

        const_pointer operator->() const {
            return &parent[pos];
        }
    };

    /**
     * Non-Const iterator in the Vector
     */
    class iterator
    {
        CyclicVector<value_type> &parent;
        unsigned           pos;
        int                direction;

    public:
        iterator(CyclicVector<value_type> &_parent, unsigned _pos, int _direction)
        : parent(_parent), pos(_pos), direction(_direction)
        { }

        bool operator!=(const iterator& other) const {
            return (pos != other.pos) || (&parent != &other.parent);
        }

        bool operator==(const iterator& other) const {
            return (pos == other.pos) && (&parent == &other.parent);
        }

        /**
         * Prefix increment
         */
        iterator& operator++() {
            pos += direction;
            return *this;
        }

        /**
         * Postfix increment
         */
        iterator operator++(int x) {
            iterator tmp = *this;
            pos += direction;
            return tmp;
        }

        reference operator*() const {
            return parent[pos];
        }

        pointer operator->() const {
            return &parent[pos];
        }
    };

    /**
     * Default init: given max size
     */
    CyclicVector(unsigned _size)
     : allocated(_size)
     , used(0)
     , first(0), last(0)
     , vectorStart(new _Tp[_size])
    { }

    /**
     * Copy constructor
     */
    CyclicVector(const CyclicVector<_Tp> &other)
    { (*this) = other; }

    /**
     * Assignment operator: make a deep copy
     */
    CyclicVector<_Tp>& operator=(const CyclicVector<_Tp> &other)
    {
        this->allocated = other.allocated;
        this->used      = other.used;
        this->first     = 0;
        this->last      = other.used;
        this->vectorStart = new _Tp[other.allocated];

        for (unsigned i = 0; i < used; ++i)
            vectorStart[i] = other[i];

        return *this;
    }

    /**
     * Move constructor
     */
    CyclicVector(CyclicVector<_Tp> &&other)
    { (*this) = std::forward<CyclicVector<value_type>>(other); }

    /**
     * Move operator: make a deep copy
     */
    CyclicVector<_Tp>& operator=(CyclicVector<_Tp> &&other)
    {
        this->allocated = other.allocated;
        this->used      = other.used;
        this->first     = other.first;
        this->last      = other.last;
        this->vectorStart = other.vectorStart;

        other.vectorStart = NULL;
        other.allocated   = 0;
        other.clear();

        return *this;
    }

    virtual ~CyclicVector() {
        delete[] vectorStart;
    }

    /**
     * Remove all elements.
     */
    void clear() {
        first = last = used = 0;
    }

    /**
     * Resize array and throw away existing content!
     */
    void resize(unsigned newSize)
    {
        delete[] vectorStart;
        vectorStart = new _Tp[newSize];
        allocated = newSize;
        clear();
    }

    /**
     * Return the number of used items
     */
    unsigned size() const {
        return used;
    }

    /**
     * Return the max capacity, vector will NOT grow but overwrite items.
     */
    unsigned allocatedSize() const {
        return allocated;
    }

    /**
     * Is the vector empty?
     */
    bool empty() const {
        return 0 == used;
    }

    const_iterator rbegin() const {
        return const_iterator(*this, last-1, -1);
    }

    const_iterator rend() const {
        return const_iterator(*this, 0-1, -1);
    }

    const_iterator cbegin() const {
        return const_iterator(*this, 0, 1);
    }

    const_iterator cend() const {
        return const_iterator(*this, last, 1);
    }

    iterator begin() {
        return iterator(*this, 0, 1);
    }

    iterator end() {
        return iterator(*this, last, 1);
    }

    reference back() {
        return vectorStart[last-1];
    }

    const_reference back() const {
        return vectorStart[last-1];
    }

    reference front() {
        return vectorStart[first];
    }

    const_reference front() const {
        return vectorStart[first];
    }

    /**
     * Add an item at the end of the vector.
     */
    void push_back(const_reference item)
    {
        if (last < allocated)
            vectorStart[last++] = item;
        else {
            // last reaches the end
            vectorStart[0] = item;
            last = 1;
        }
        if (used == allocated) {
            ++first;
            if (first >= allocated)
                first = 0;
        }
        else
            ++used;
    }

    reference operator[](unsigned index) {
        if (index >= used) {
            char buffer[200];
            sprintf(buffer, "Index %u higher than use count %u", index, used);
            throw std::out_of_range(buffer);
        }
        return vectorStart[(first + index) % allocated];
    }

    const_reference operator[](unsigned index) const {
        if (index >= used) {
            char buffer[200];
            sprintf(buffer, "Index %u higher than use count %u", index, used);
            throw std::out_of_range(buffer);
        }
        return vectorStart[(first + index) % allocated];
    }
};

} /* namespace utils */

#endif /* CYCLICVECTOR_H_ */
