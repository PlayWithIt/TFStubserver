/*
 * CharMask.h
 *
 * Copyright (C) 2015-2017 Holger Grosenick
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

#ifndef CHARMASK_H
#define CHARMASK_H

namespace stubserver {

class CharMask;
typedef const CharMask *PCharMask;

/**
 * A class that holds the bitmask for the dotted LCD or OLED font.
 * The class is const internally.
 * 
 * @author Holger Grosenick
 */
class CharMask
{
public:
    /**
     * Max number of lines in the dot matrix.
     */
    static const unsigned NUM_LINES = 9;

    /**
      * Max number of columns in the dot matrix.
      */
    static const unsigned NUM_COLS = 6;

    /**
      * Init the bitmask directly from 8 bytes.
      * @param inputMask bitmask with one byte per line in the bitmask
      */
    CharMask(const unsigned char inputMask[8]);

    /**
      * Init the bitmask from a 2-dimensional String array
      * @param mask a string array which defines the bitmask for each dotted char.
      */
    CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7, const char *m8);
    CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7);

    /**
     * Default init: all bits are false -> this is a blank.
     */
    CharMask();

    /**
     * Copy constructor.
     */
    CharMask(const CharMask& other) {
        *this = other;
    }

    CharMask& operator=(const CharMask &other);

    /**
     * Returns true is a dot of the char display is on.
     * @param x dot x 0..5
     * @param y dot y 0..8
     * @return true or false
     */
    bool isOn(unsigned x, unsigned y) const {
        return bitMask[x][y];
    }

private:
    /**
     * The dot matrix
     */
    bool bitMask[NUM_COLS][NUM_LINES];

    /**
     * Init bitmask from char array.
     */
    void init(const char** mask);
};


/**
 * The font holds 256 bitmasks, one for each char.
 */
class ScreenFont
{
    PCharMask chars[256];

public:
    explicit ScreenFont(bool forOled);
    ~ScreenFont();

    /**
     * Init / update mask.
     */
    void set(char c, PCharMask mask) {
        unsigned i = static_cast<unsigned char>(c);
        delete chars[i];
        chars[i] = mask;
    }

    /**
     * Get char mask.
     */
    PCharMask get(char c) const {
        return chars[static_cast<unsigned char>(c)];
    }

};

}

#endif
