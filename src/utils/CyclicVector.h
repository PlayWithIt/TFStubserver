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
 * - => it cannot overflow but has a limited max size and might <b>overwrite</b> items.
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

    pointer vectorStart;

    inline reference getAddr(unsigned index) const {
        return vectorStart[(first + index) % allocated];
    }

public:

    /**
     * Common iterator template for the vector, can be an iterator or const_iterator.
     */
    template <typename _Parent, typename _Ref, typename _Ptr> class _vector_iterator
    {
        _Parent  &parent;
        unsigned pos;
        int      direction;

    public:

        typedef std::random_access_iterator_tag iterator_category;
        typedef _Ptr                            pointer;
        typedef _Ref                            reference;
        typedef _vector_iterator                 _Self;


        _vector_iterator(_Parent &_parent, unsigned _pos, int _direction)
        : parent(_parent), pos(_pos), direction(_direction)
        { }

        bool operator!=(const _vector_iterator<_Parent, _Ref, _Ptr>& other) const {
            return (pos != other.pos) || (&parent != &other.parent);
        }

        bool operator==(const _vector_iterator<_Parent, _Ref, _Ptr>& other) const {
            return (pos == other.pos) && (&parent == &other.parent);
        }

        /**
         * Prefix increment
         */
        _Self& operator++() {
            pos += direction;
            return *this;
        }

        /**
         * Postfix increment
         */
        _Self operator++(int x) {
            const_iterator tmp = *this;
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
     * iterator's in the Vector
     */
    typedef _vector_iterator< const CyclicVector<_Tp>, const _Tp&, const _Tp*> const_iterator;
    typedef _vector_iterator< CyclicVector<_Tp>, _Tp&, _Tp*> iterator;

    /**
     * Default init: given max size; the vector will NOT grow but overwrite
     * other existing elements.
     */
    CyclicVector(unsigned _size)
     : allocated(_size)
     , used(0)
     , first(0)
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
        this->vectorStart = new _Tp[other.allocated];

        // the []-operator will recalculate the source index
        for (unsigned i = 0; i < used; ++i)
            vectorStart[i] = other[i];

        return *this;
    }

    /**
     * Move constructor: call move assignment.
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
        first = used = 0;
    }

    /**
     * Resize array and copy all items into the new array, one by one (time consuming !)
     */
    void resize(unsigned newSize)
    {
        unsigned oldSize  = allocated;
        unsigned oldFirst = first;
        pointer old = vectorStart;

        vectorStart = new _Tp[newSize];
        allocated = newSize;
        first = 0;

        // copy old content
        for (unsigned i = 0; i < used; ++i) {
            vectorStart[i] = old[(oldFirst + i) % oldSize];
        }

        delete[] old;
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

    /**
     * Is the vector full? If yes, the next push operation will overwrite.
     */
    bool full() const {
        return used == allocated;
    }

    const_iterator rbegin() const {
        return const_iterator(*this, used-1, -1);
    }

    const_iterator rend() const {
        return const_iterator(*this, 0-1, -1);
    }

    const_iterator cbegin() const {
        return const_iterator(*this, 0, 1);
    }

    const_iterator cend() const {
        return const_iterator(*this, used, 1);
    }

    const_iterator begin() const {
        return const_iterator(*this, 0, 1);
    }

    const_iterator end() const {
        return const_iterator(*this, used, 1);
    }

    iterator begin() {
        return iterator(*this, 0, 1);
    }

    iterator end() {
        return iterator(*this, used, 1);
    }

    reference back() {
        if (empty())
            throw std::out_of_range("back() used on an empty container!");
        return getAddr(used - 1);
    }

    const_reference back() const {
        if (empty())
            throw std::out_of_range("back() used on an empty container!");
        return getAddr(used - 1);
    }

    reference front() {
        if (empty())
            throw std::out_of_range("front() used on an empty container!");
        return vectorStart[first];
    }

    const_reference front() const {
        if (empty())
            throw std::out_of_range("front() used on an empty container!");
        return vectorStart[first];
    }

    /**
     * Removes the front-items (index 0) from the vector.
     */
    void pop_front() {
        if (empty())
            throw std::out_of_range("pop_front() used on an empty container!");
        if (++first == allocated)
            first = 0;
        --used;
    }

    /**
     * Removes the back-items (index == size() - 1) from the vector.
     */
    void pop_back() {
        if (empty())
            throw std::out_of_range("pop_back() used on an empty container!");
        --used;
    }

    /**
     * Add an item at the end of the vector.
     * @return true if the vector was already full BEFORE adding an element and one value was overwritten
     */
    bool push_back(const_reference item)
    {
        bool isFull = full();
        if (isFull) {
            ++first;
            if (first >= allocated)
                first = 0;
        }
        else
            ++used;
        getAddr(used - 1) = item;
        return isFull;
    }

    /**
     * Add an item at the beginning of the vector.
     * @return true if the vector was already full BEFORE adding an element and one value was overwritten
     */
    bool push_front(const_reference item)
    {
        if (first == 0)
            first = allocated - 1;
        else
            --first;

        if (full()) {
            vectorStart[first] = item;
            return true;
        }
        if (used < allocated)
            ++used;
        vectorStart[first] = item;
        return false;
    }

    reference operator[](unsigned index) {
        if (index >= used) {
            char buffer[200];
            sprintf(buffer, "Index %u higher than use count %u", index, used);
            throw std::out_of_range(buffer);
        }
        return getAddr(index);
    }

    const_reference operator[](unsigned index) const {
        if (index >= used) {
            char buffer[200];
            sprintf(buffer, "Index %u higher than use count %u", index, used);
            throw std::out_of_range(buffer);
        }
        return getAddr(index);
    }
};

} /* namespace utils */

#endif /* CYCLICVECTOR_H_ */
