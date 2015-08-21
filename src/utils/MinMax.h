/*
 * MinMax.h
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
#ifndef MINMAX_H_
#define MINMAX_H_

#include <limits>

namespace utils {


/**
 * Helper structure to store min/max values for numeric values.
 * This class uses an absolute minimum value, so not usable for general purpose.
 */
template <typename _Tp> class MinMax
{
    typedef _Tp value_type;

    value_type min, max;

public:
    /**
     * Default init.
     */
    MinMax() { reset(); }

    /**
     * Init with given min and max.
     */
    MinMax(value_type _min, value_type _max)
      : min(_min), max(_max) { }

    /**
     * Check if the new value if lower than min or larger than max
     * and update min/max if necessary.
     */
    void newValue(value_type v) {
        // if there is just one value: set min == max
        if (v < min)
            min = v;
        if (v > max)
            max = v;
    }

    /**
     * Clear min and max.
     */
    void reset() {
        min = std::numeric_limits<_Tp>::max();
        max = std::numeric_limits<_Tp>::lowest();
    }

    /**
     * Return the current minimum.
     */
    value_type getMin() const {
        return min;
    }

    /**
     * Return the current maximum.
     */
    value_type getMax() const {
        return max;
    }
};

// often used
typedef MinMax<double> MinMaxDouble;
typedef MinMax<int>    MinMaxInt;


} /* namespace utils */

#endif /* MINMAX_H_ */
