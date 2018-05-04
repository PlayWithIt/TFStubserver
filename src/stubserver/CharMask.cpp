/*
 * CharMask.cpp
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


#include <string.h>
#include <strings.h>

#include "CharMask.h"

namespace stubserver {

/**
 * Init the bitmask from a 2-dimensional String array
 */
CharMask::CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7, const char *m8)
{
    const char *mask[] = {m1, m2, m3, m4, m5, m6, m7, m8, NULL };
    init(mask);
}

/**
 * Init the bitmask from a 2-dimensional String array
 */
CharMask::CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7)
{
    const char *mask[] = {m1, m2, m3, m4, m5, m6, m7, NULL };
    init(mask);
}

/**
  * Init the bitmask directly from 8 bytes.
  * @param inputMask bitmask with one byte per line in the bitmask
  */
CharMask::CharMask(const unsigned char inputMask[])
{
    bzero(bitMask, sizeof(bitMask));
    const unsigned leftBit = 0b00010000;

    for (unsigned y = 0; y < 8; ++y)
    {
        unsigned char byte = inputMask[y];
        for (unsigned x = 0; x < NUM_COLS; ++x)
        {
            if ((byte & (leftBit >> x)) != 0)
                bitMask[x][y] = true;
        }
    }
}

CharMask& CharMask::operator=(const CharMask &other)
{
    memcpy(bitMask, other.bitMask, sizeof(bitMask));
    return *this;
}

void CharMask::init(const char** mask)
{
    bzero(bitMask, sizeof(bitMask));
    unsigned w = strlen(mask[0]);

    for (unsigned y = 0; mask[y] && y < NUM_LINES; ++y)
    {
        const char *s = mask[y];
        for (unsigned x = 0; x < w; ++x)
        {
            char ch = s[x];
            if (ch == 'X' || ch == 'x')
                bitMask[x][y] = true;
        }
    }
}

/**
 * Default init: all bits are false -> this is a blank.
 */
CharMask::CharMask()
{
    bzero(bitMask, sizeof(bitMask));
}


/**
 * much, much code to set the pixels of each char in the pixel font.
 */
ScreenFont::ScreenFont(bool forOled)
{
    bzero(chars, sizeof(chars));

    set('!', new CharMask("  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "     ",
                          "  X  " ));

    set('"', new CharMask(" X X ",
                          " X X ",
                          " X X ",
                          "     ",
                          "     ",
                          "     ",
                          "     " ));

    set('#', new CharMask(" X X ",
                          " X X ",
                          "XXXXX",
                          " X X ",
                          "XXXXX",
                          " X X ",
                          " X X " ));

    set('$', new CharMask("  X  ",
                          " XXXX",
                          "X X  ",
                          " XXX ",
                          "  X X",
                          "XXXX ",
                          "  X  " ));

    set('%', new CharMask("XX   ",
                          "XX  X",
                          "   X ",
                          "  X  ",
                          " X   ",
                          "X  XX",
                          "   XX"));

    set('&', new CharMask(" XX  ",
                          "X  X ",
                          "X X  ",
                          " X   ",
                          "X X X",
                          "X  X ",
                          " XX X"));

    set('\'', new CharMask(" XX  ",
                           "  X  ",
                           " X   ",
                           "     ",
                           "     ",
                           "     ",
                           "     "));

    set('(', new CharMask("   X ",
                          "  X  ",
                          " X   ",
                          " X   ",
                          " X   ",
                          "  X  ",
                          "   X " ));

    set(')', new CharMask(" X   ",
                          "  X  ",
                          "   X ",
                          "   X ",
                          "   X ",
                          "  X  ",
                          " X   " ));

    set('*', new CharMask("     ",
                          "  X  ",
                          "x X x",
                          " XXX ",
                          "x X x",
                          "  X  ",
                          "     "));

    set('+', new CharMask("     ",
                          "  X  ",
                          "  X  ",
                          "XXXXX",
                          "  X  ",
                          "  X  ",
                          "     "));

    set(',', new CharMask("     ",
                          "     ",
                          "     ",
                          "     ",
                          " XX  ",
                          "  X  ",
                          " X   "));

    set('-', new CharMask("     ",
                          "     ",
                          "     ",
                          "XXXXX",
                          "     ",
                          "     ",
                          "     "));

    set('.', new CharMask("     ",
                          "     ",
                          "     ",
                          "     ",
                          "     ",
                          " XX  ",
                          " XX  "));

    set('/', new CharMask("     ",
                          "    X",
                          "   X ",
                          "  X  ",
                          " X   ",
                          "X    ",
                          "     "));

    set('0', new CharMask(" XXX ",
                          "X   X",
                          "X  XX",
                          "X X X",
                          "XX  X",
                          "X   X",
                          " XXX "));

    set('1', new CharMask("  X  ",
                          " XX  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          " XXX " ));

    set('2', new CharMask(" XXX ",
                          "X   X",
                          "    X",
                          "   X ",
                          "  X  ",
                          " X   ",
                          "XXXXX"));

    set('3', new CharMask("XXXXX",
                          "   X ",
                          "  X  ",
                          "   X ",
                          "    X",
                          "X   X",
                          " XXX " ));

    set('4', new CharMask("   X ",
                          "  XX ",
                          " X X ",
                          "X  X ",
                          "XXXXX",
                          "   X ",
                          "   X "));

    set('5', new CharMask("XXXXX",
                          "X    ",
                          "XXXX ",
                          "    X",
                          "    X",
                          "X   X",
                          " XXX " ));

    set('6', new CharMask("  XX ",
                          " X   ",
                          "X    ",
                          "XXXX ",
                          "X   X",
                          "X   X",
                          " XXX " ));

    set('7', new CharMask("XXXXX",
                          "    X",
                          "   X ",
                          "  X  ",
                          " X   ",
                          " X   ",
                          " X   "));

    set('8', new CharMask(" XXX ",
                          "X   X",
                          "X   X",
                          " XXX ",
                          "X   X",
                          "X   X",
                          " XXX " ));

    set('9', new CharMask(" XXX ",
                          "X   X",
                          "X   X",
                          " XXXX",
                          "    X",
                          "   X ",
                          " XX  "));

    set('<', new CharMask("   X ",
                          "  X  ",
                          " X   ",
                          "X    ",
                          " X   ",
                          "  X  ",
                          "   X "));


    set('=', new CharMask("     ",
                          "     ",
                          "XXXXX",
                          "     ",
                          "XXXXX",
                          "     ",
                          "     "));

    set('>', new CharMask("X    ",
                          " X   ",
                          "  X  ",
                          "   X ",
                          "  X  ",
                          " X   ",
                          "X    "));

    set('?', new CharMask(" XXX ",
                          "X   X",
                          "X   X",
                          "   X ",
                          "  X  ",
                          "     ",
                          "  X  " ));

    set('A', new CharMask(" XXX ",
                          "X   X",
                          "X   X",
                          "XXXXX",
                          "X   X",
                          "X   X",
                          "X   X" ));

    set('B', new CharMask("XXXX ",
                          "X   X",
                          "X   X",
                          "XXXX ",
                          "X   X",
                          "X   X",
                          "XXXX " ));

    set('C', new CharMask(" XXX ",
                          "X   X",
                          "X    ",
                          "X    ",
                          "X    ",
                          "X   X",
                          " XXX " ));

    set('D', new CharMask("XXXX ",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          "XXXX " ));

    set('E', new CharMask("XXXXX",
                          "X    ",
                          "X    ",
                          "XXX  ",
                          "X    ",
                          "X    ",
                          "XXXXX" ));

    set('F', new CharMask("XXXXX",
                          "X    ",
                          "X    ",
                          "XXX  ",
                          "X    ",
                          "X    ",
                          "X    " ));

    set('G', new CharMask(" XXX ",
                          "X   X",
                          "X    ",
                          "X XXX",
                          "X   X",
                          "X   X",
                          " XXXX" ));

    set('H', new CharMask("X   X",
                          "X   X",
                          "X   X",
                          "XXXXX",
                          "X   X",
                          "X   X",
                          "X   X" ));

    set('I', new CharMask(" XXX ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          " XXX " ));

    set('J', new CharMask("  XXX",
                          "   X ",
                          "   X ",
                          "   X ",
                          "   X ",
                          "X  X ",
                          " XX  " ));

    set('K', new CharMask("X   X",
                          "X  X ",
                          "X X  ",
                          "XX   ",
                          "X X  ",
                          "X  X ",
                          "X   X" ));

    set('L', new CharMask("X    ",
                          "X    ",
                          "X    ",
                          "X    ",
                          "X    ",
                          "X    ",
                          "XXXXX" ));

    set('M', new CharMask("X   X",
                          "XX XX",
                          "X X X",
                          "X X X",
                          "X   X",
                          "X   X",
                          "X   X" ));

    set('N', new CharMask("X   X",
                          "X   X",
                          "XX  X",
                          "X X X",
                          "X  XX",
                          "X   X",
                          "X   X" ));

    set('O', new CharMask(" XXX ",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          " XXX " ));

    set('P', new CharMask("XXXX ",
                          "X   X",
                          "X   X",
                          "XXXX ",
                          "X    ",
                          "X    ",
                          "X    " ));

    set('Q', new CharMask(" XXX ",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X X X",
                          " XXX ",
                          "   X "));

    set('R', new CharMask("XXXX ",
                          "X   X",
                          "X   X",
                          "XXXX ",
                          "X   X",
                          "X   X",
                          "X   X" ));

    set('S', new CharMask(" XXXX",
                          "X    ",
                          "X    ",
                          " XXX ",
                          "    X",
                          "    X",
                          "XXXX "));

    set('T', new CharMask("XXXXX",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  " ));

    set('U', new CharMask("X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          " XXX "));

    set('V', new CharMask("X   X",
                          "X   X",
                          "X   X",
                          "X   X",
                          " X X ",
                          " X X ",
                          "  X  " ));

    set('W', new CharMask("X   X",
                          "X   X",
                          "X   X",
                          "X X X",
                          "X X X",
                          "X X X",
                          " X X "));

    set('X', new CharMask("X   X",
                          "X   X",
                          " X X ",
                          "  X  ",
                          " X X ",
                          "X   X",
                          "X   X" ));

    set('Y', new CharMask("X   X",
                          "X   X",
                          " X X ",
                          " X X ",
                          "  X  ",
                          "  X  ",
                          "  X  "));

    set('Z', new CharMask("XXXXX",
                          "   X ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          " X   ",
                          "XXXXX" ));

    set('[', new CharMask(" XXX ",
                          " X   ",
                          " X   ",
                          " X   ",
                          " X   ",
                          " X   ",
                          " XXX "));

    set('\\', new CharMask("X X X",
                           " XXX ",
                           "XXXXX",
                           "  X  ",
                           "XXXXX",
                           "  X  ",
                           "  X  "));

    set(']', new CharMask(" XXX ",
                          "   X ",
                          "   X ",
                          "   X ",
                          "   X ",
                          "   X ",
                          " XXX "));

    set('_', new CharMask("     ",
                          "     ",
                          "     ",
                          "     ",
                          "     ",
                          "     ",
                          "XXXXX"));

    // the index values are that one from the LCD / OLED charset!
    unsigned i_ae, i_oe, i_ue;
    unsigned i_FF;
    if (forOled) {
        i_ae = 0x84;
        i_oe = 0x94;
        i_ue = 0x81;
        i_FF = 0xDB;
    }
    else {
        i_ae = 0xE1;
        i_oe = 0xEF;
        i_ue = 0xF5;
        i_FF = 0xFF;
    }

    // ä
    set(i_ae, new CharMask(" X X ",
                           "     ",
                           " XXX ",
                           "    X",
                           " XXXX",
                           "X   X",
                           " XXXX" ));
    // ß
    set(0xE2, new CharMask("     ",
                           " XXX ",
                           "X   X",
                           "XXXX ",
                           "X   X",
                           "XXXX ",
                           "X    ",
                           "X    " ));

    set('a', new CharMask("     ",
                          "     ",
                          " XXX ",
                          "    X",
                          " XXXX",
                          "X   X",
                          " XXXX" ));

    set('b', new CharMask("X    ",
                          "X    ",
                          "X XX ",
                          "XX  X",
                          "X   X",
                          "X   X",
                          "XXXX " ));

    set('c', new CharMask("     ",
                          "     ",
                          " XXX ",
                          "X    ",
                          "X    ",
                          "X   X",
                          " XXX " ));

    set('d', new CharMask("    X",
                          "    X",
                          " XX X",
                          "X  XX",
                          "X   X",
                          "X   X",
                          " XXXX" ));

    set('e', new CharMask("     ",
                          "     ",
                          " XXX ",
                          "X   X",
                          "XXXXX",
                          "X    ",
                          " XXX " ));

    set('f', new CharMask("  XX ",
                          " X  X",
                          " X   ",
                          "XXX  ",
                          " X   ",
                          " X   ",
                          " X   " ));

    set('g', new CharMask("     ",
                          "     ",
                          " XXX ",
                          "X   X",
                          "X   X",
                          " XXXX",
                          "    X",
                          " XXX " ));

    set('h', new CharMask("X    ",
                          "X    ",
                          "X XX ",
                          "XX  X",
                          "X   X",
                          "X   X",
                          "X   X" ));

    set('i', new CharMask("  x  ",
                          "     ",
                          " xX  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          " XXX " ));

    set('j', new CharMask("   X ",
                          "     ",
                          "  xx ",
                          "   X ",
                          "   X ",
                          "X  X ",
                          " XX  " ));

    set('k', new CharMask("X    ",
                          "X    ",
                          "X  X ",
                          "X X  ",
                          "XX   ",
                          "X X  ",
                          "X  X " ));

    set('l', new CharMask(" XX  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          " XXX " ));

    set('m', new CharMask("     ",
                          "     ",
                          "XX X ",
                          "X X X",
                          "X X X",
                          "X   X",
                          "X   X" ));

    set('n', new CharMask("     ",
                          "     ",
                          "X XX ",
                          "XX  X",
                          "X   X",
                          "X   X",
                          "X   X" ));

    set('o', new CharMask("     ",
                          "     ",
                          " XXX ",
                          "X   X",
                          "X   X",
                          "X   X",
                          " XXX " ));

    // ö
    set(i_oe, new CharMask(" X X ",
                           "     ",
                           " XXX ",
                           "X   X",
                           "X   X",
                           "X   X",
                           " XXX " ));

    set('p', new CharMask("     ",
                          "     ",
                          "XXXX ",
                          "X   X",
                          "XXXX ",
                          "X    ",
                          "X    " ));

    set('q', new CharMask("     ",
                          "     ",
                          " XXXX",
                          "X   X",
                          " XXXX",
                          "    X",
                          "    X"));

    set('r', new CharMask("     ",
                          "     ",
                          "X XX ",
                          "XX  X",
                          "X    ",
                          "X    ",
                          "X    " ));

    set('s', new CharMask("     ",
                          "     ",
                          " xxxx",
                          "x    ",
                          " xxxX",
                          "    X",
                          "XXXX "));

    set('t', new CharMask(" X   ",
                          " X   ",
                          "XXX  ",
                          " X   ",
                          " X   ",
                          " X  X",
                          "  XX " ));

    set('u', new CharMask("     ",
                          "     ",
                          "X   X",
                          "X   X",
                          "X   X",
                          "X  XX",
                          " XX X"));

    // ü
    set(i_ue, new CharMask(" X X ",
                           "     ",
                           "X   X",
                           "X   X",
                           "X   X",
                           "X  XX",
                           " XX X"));

    set('v', new CharMask("     ",
                          "     ",
                          "X   X",
                          "X   X",
                          "X   X",
                          " X X ",
                          "  X  " ));

    set('w', new CharMask("     ",
                          "     ",
                          "X   X",
                          "X   X",
                          "X X X",
                          "X X X",
                          " X X "));

    set('x', new CharMask("     ",
                          "     ",
                          "X   X",
                          " X X ",
                          "  X  ",
                          " X X ",
                          "X   X" ));

    set('y', new CharMask("     ",
                          "     ",
                          "X   X",
                          "X   X",
                          " XXX ",
                          "    X",
                          " XXX "));

    set('z', new CharMask("     ",
                          "     ",
                          "XXXXX",
                          "   X ",
                          "  X  ",
                          " X   ",
                          "XXXXX" ));

    set('{', new CharMask("   X ",
                          "  X  ",
                          "  X  ",
                          " X   ",
                          "  X  ",
                          "  X  ",
                          "   X " ));

    set('|', new CharMask("  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  ",
                          "  X  "));

    set('}', new CharMask(" X   ",
                          "  X  ",
                          "  X  ",
                          "   X ",
                          "  X  ",
                          "  X  ",
                          " X   " ));

    // 255 has all pixels active for LCD
    set(i_FF, new CharMask("XXXXX",
                           "XXXXX",
                           "XXXXX",
                           "XXXXX",
                           "XXXXX",
                           "XXXXX",
                           "XXXXX",
                           "XXXXX" ));

    //------------------------------------------------------------------------------
    if (forOled) {
        set(' ', new CharMask());   // blank chars
        set(255, new CharMask());

        set(':', new CharMask("     ",
                              "     ",
                              "  X  ",
                              "     ",
                              "  X  ",
                              "     ",
                              "     "));

        set(';', new CharMask("     ",
                              "     ",
                              "  X  ",
                              "     ",
                              "  X  ",
                              "  X  ",
                              " X   "));

        set(0x01, new CharMask(" XXX ",
                               "XXXXX",
                               "X X X",
                               "XXXXX",
                               "XX XX",
                               "X   X",
                               " XXX " ));

        set(0x02, new CharMask(" XXX ",
                               "XXXXX",
                               "X X X",
                               "XXXXX",
                               "X   X",
                               "X   X",
                               " XXX " ));

        set(0x03, new CharMask("     ",
                               " X X ",
                               "XXXXX",
                               "XXXXX",
                               "XXXXX",
                               " XXX ",
                               "  X  " ));

        set(0x04, new CharMask("     ",
                               "  X  ",
                               " XXX ",
                               "XXXXX",
                               "XXXXX",
                               " XXX ",
                               "  X  " ));

        set(0x05, new CharMask(" XXX ",
                               " X X ",
                               "XXXXX",
                               "X X X",
                               "XXXXX",
                               "  X  ",
                               " XXX " ));

        set(0x06, new CharMask("  X  ",
                               " XXX ",
                               "XXXXX",
                               "XXXXX",
                               "XXXXX",
                               "  X  ",
                               " XXX " ));

        set(0x07, new CharMask("     ",
                               "     ",
                               "  X  ",
                               " XXX ",
                               " XXX ",
                               "  X  ",
                               "     " ));

        set(0x08, new CharMask("XXXXX",
                               "XXXXX",
                               "XX XX",
                               "X   X",
                               "X   X",
                               "XX XX",
                               "XXXXX",
                               "XXXXX" ));

        set(0x09, new CharMask("     ",
                               "     ",
                               "  X  ",
                               " X X ",
                               " X X ",
                               "  X  ",
                               "     " ));

        set(0x0A, new CharMask("XXXXX",
                               "XXXXX",
                               "XX XX",
                               "X X X",
                               "X X X",
                               "XX XX",
                               "XXXXX",
                               "XXXXX" ));

        set(0x0B, new CharMask("     ",
                               "  XXX",
                               "   XX",
                               " XX X",
                               "X X  ",
                               "X X  ",
                               " X   " ));

        set(0x0C, new CharMask(" XXX ",
                               "X   X",
                               "X   X",
                               " XXX ",
                               "  X  ",
                               "XXXXX",
                               "  X  " ));

        set(0x0D, new CharMask(" XXXX",
                               " X  X",
                               " XXXX",
                               " X   ",
                               " X   ",
                               " X   ",
                               "XX   " ));

        set(0x0E, new CharMask(" XXXX",
                               " X  X",
                               " XXXX",
                               " X  X",
                               " X  X",
                               " X XX",
                               "XX   " ));

        set(0x0F, new CharMask("  X  ",
                               "X X X",
                               " XXX ",
                               "XX XX",
                               "XX XX",
                               " XXX ",
                               "X X X",
                               "  X  " ));

        set(0x10, new CharMask("X    ",
                               "XX   ",
                               "XXXX ",
                               "XXXXX",
                               "XXXX ",
                               "XX   ",
                               "X    " ));

        set(0x11, new CharMask("    X",
                               "   XX",
                               " XXXX",
                               "XXXXX",
                               "  XXX",
                               "   XX",
                               "    X" ));

        set(0x12, new CharMask("  X  ",
                               " XXX ",
                               "X X X",
                               "  X  ",
                               "X X X",
                               " XXX ",
                               "  X  " ));

        set(0x13, new CharMask("XX XX",
                               "XX XX",
                               "XX XX",
                               "XX XX",
                               "XX XX",
                               "     ",
                               "XX XX" ));

        set(0x14, new CharMask(" XX X",
                               "X X X",
                               "X X X",
                               " XX X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0x15, new CharMask("  XX ",
                               " X  X",
                               " X X ",
                               "  X X",
                               "   X ",
                               "    X",
                               " X  X",
                               "  XX " ));

        set(0x16, new CharMask("     ",
                               "     ",
                               "     ",
                               "     ",
                               "     ",
                               "XXXXX",
                               "XXXXX" ));

        set(0x17, new CharMask("  X  ",
                               " XXX ",
                               "X X X",
                               "  X  ",
                               "X X X",
                               " XXX ",
                               "  X  ",
                               "XXXXX" ));

        set(0x18, new CharMask("     ",
                               "  X  ",
                               " XXX ",
                               "X X X",
                               "  X  ",
                               "  X  ",
                               "  X  " ));

        set(0x19, new CharMask("     ",
                               "  X  ",
                               "  X  ",
                               "  X  ",
                               "X X X",
                               " XXX ",
                               "  X  " ));

        set(0x1A, new CharMask("     ",
                               "  X  ",
                               "   X ",
                               "XXXXX",
                               "   X ",
                               "  X  ",
                               "     " ));

        set(0x1B, new CharMask("     ",
                               "  X  ",
                               " X   ",
                               "XXXXX",
                               " X   ",
                               "  X  ",
                               "     " ));

        set(0x1C, new CharMask("     ",
                               "X    ",
                               "X    ",
                               "X    ",
                               "XXXXX",
                               "     ",
                               "     " ));

        set(0x1D, new CharMask("     ",
                               " X X ",
                               "XXXXX",
                               "XXXXX",
                               " X X ",
                               "     ",
                               "     " ));

        set(0x1E, new CharMask("     ",
                               "  X  ",
                               "  X  ",
                               " XXX ",
                               "XXXXX",
                               "XXXXX",
                               "     " ));

        set(0x1F, new CharMask("     ",
                               "XXXXX",
                               "XXXXX",
                               " XXX ",
                               "  X  ",
                               "  X  ",
                               "     " ));

        set('~', new CharMask(" X   ",
                              "X X X",
                              "   X ",
                              "     ",
                              "     ",
                              "     ",
                              "     " ));

        set(0x80, new CharMask(" XXX ",
                               "X   X",
                               "X    ",
                               "X    ",
                               "X   X",
                               " XXX ",
                               "   X ",
                               " XX  " ));

        set(0x99, new CharMask("X   X",
                               " XXX ",
                               "X   X",
                               "X   X",
                               "X   X",
                               "X   X",
                               "X   X",
                               " XXX " ));

        set(0x9A, new CharMask("X   X",
                               "     ",
                               "X   X",
                               "X   X",
                               "X   X",
                               "X   X",
                               "X   X",
                               " XXX " ));

        set(0x9B, new CharMask("  X  ",
                               "  X  ",
                               "XXXXX",
                               "X X  ",
                               "X X  ",
                               "XXXXX",
                               "  X  ",
                               "  X  " ));

        set(0xB0, new CharMask("X X X",
                               "     ",
                               "X X X",
                               "     ",
                               "X X X",
                               "     ",
                               "X X X",
                               "     " ));

        set(0xB1, new CharMask(" X X ",
                               "X X X",
                               " X X ",
                               "X X X",
                               " X X ",
                               "X X X",
                               " X X ",
                               "X X X" ));

        set(0xB2, new CharMask("XXXXX",
                               "X X X",
                               "XXXXX",
                               "X X X",
                               "XXXXX",
                               "X X X",
                               "XXXXX",
                               "X X X" ));

        set(0xB3, new CharMask("   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xB4, new CharMask("   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "XXXX ",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xB5, new CharMask("   X ",
                               "   X ",
                               "XXXX ",
                               "   X ",
                               "XXXX ",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xB6, new CharMask("  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "XXX X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xB7, new CharMask("     ",
                               "     ",
                               "     ",
                               "     ",
                               "XXXXX",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xB8, new CharMask("     ",
                               "     ",
                               "XXXX ",
                               "   X ",
                               "XXXX ",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xB9, new CharMask("  X X",
                               "  X X",
                               "XXX X",
                               "  X X",
                               "XXX X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xBA, new CharMask("  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xBB, new CharMask("     ",
                               "     ",
                               "XXXXX",
                               "    X",
                               "XXX X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xBC, new CharMask("  X X",
                               "  X X",
                               "XXX X",
                               "    X",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xBD, new CharMask("  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xBE, new CharMask("   X ",
                               "   X ",
                               "XXXX ",
                               "   X ",
                               "XXXX ",
                               "     ",
                               "     ",
                               "     " ));

        set(0xBF, new CharMask("     ",
                               "     ",
                               "     ",
                               "     ",
                               "XXXX ",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xC0, new CharMask("   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "   XX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xC1, new CharMask("   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xC2, new CharMask("     ",
                               "     ",
                               "     ",
                               "     ",
                               "XXXXX",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xC3, new CharMask("   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "   XX",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xC4, new CharMask("     ",
                               "     ",
                               "     ",
                               "     ",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xC5, new CharMask("   X ",
                               "   X ",
                               "   X ",
                               "   X ",
                               "XXXXX",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xC6, new CharMask("   X ",
                               "   X ",
                               "   XX",
                               "   X ",
                               "   XX",
                               "   X ",
                               "   X ",
                               "   X " ));

        set(0xC7, new CharMask("  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xC8, new CharMask("  X X",
                               "  X X",
                               "  X X",
                               "  X  ",
                               "  XXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xC9, new CharMask("     ",
                               "     ",
                               "  XXX",
                               "  X  ",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xCA, new CharMask("  X X",
                               "  X X",
                               "XXX X",
                               "     ",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xCB, new CharMask("     ",
                               "     ",
                               "XXXXX",
                               "     ",
                               "XXX X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xCC, new CharMask("  X X",
                               "  X X",
                               "  X X",
                               "  X  ",
                               "  X X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xCD, new CharMask("     ",
                               "     ",
                               "XXXXX",
                               "     ",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xCE, new CharMask("  X X",
                               "  X X",
                               "XXX X",
                               "     ",
                               "XXX X",
                               "  X X",
                               "  X X",
                               "  X X" ));

        set(0xCF, new CharMask("   X ",
                               "   X ",
                               "XXXXX",
                               "     ",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     " ));

        set(0xDC, new CharMask("     ",
                               "     ",
                               "     ",
                               "     ",
                               "XXXXX",
                               "XXXXX",
                               "XXXXX",
                               "XXXXX" ));

        set(0xDD, new CharMask("XXX  ",
                               "XXX  ",
                               "XXX  ",
                               "XXX  ",
                               "XXX  ",
                               "XXX  ",
                               "XXX  ",
                               "XXX  " ));

        set(0xDE, new CharMask("   XX",
                               "   XX",
                               "   XX",
                               "   XX",
                               "   XX",
                               "   XX",
                               "   XX",
                               "   XX" ));

        set(0xDF, new CharMask("XXXXX",
                               "XXXXX",
                               "XXXXX",
                               "XXXXX",
                               "     ",
                               "     ",
                               "     ",
                               "     " ));

        set(0xF8, new CharMask(" XXX ",
                               "XX XX",
                               "XX XX",
                               " XXX ",
                               "     ",
                               "     ",
                               "     ",
                               "     " ));
    }
    else {
        // on LCD there are a lot of blanks
        for (unsigned i = 0x10; i < 0xA0; ++i)
            if (chars[i] == NULL)
                chars[i] = new CharMask();

        set(':', new CharMask("     ",
                              " XX  ",
                              " XX  ",
                              "     ",
                              " XX  ",
                              " XX  ",
                              "     "));

        set(';', new CharMask("     ",
                              " XX  ",
                              " XX  ",
                              "     ",
                              " XX  ",
                              "  X  ",
                              " X   "));

        set(0x7E, new CharMask("     ",
                               "  X  ",
                               "   X ",
                               "XXXXX",
                               "   X ",
                               "  X  ",
                               "     " ));

        set(0x7F, new CharMask("     ",
                               "  X  ",
                               " X   ",
                               "XXXXX",
                               " X   ",
                               "  X  ",
                               "     " ));

        set(0xA1, new CharMask("   ",
                               "   ",
                               "   ",
                               "   ",
                               "XXX",
                               "X X",
                               "XXX",
                               "   "));

        set(0xDF, new CharMask("XXX",
                               "X X",
                               "XXX",
                               "   ",
                               "   ",
                               "   ",
                               "   "));

        set(0xEB, new CharMask("X X",
                               " X ",
                               "X X",
                               "   ",
                               "   ",
                               "   ",
                               "   "));
    }

    // guarantee that all chars at least have a value
    for (int i = 0; i < 256; ++i)
        if (chars[i] == NULL)
            chars[i] = new CharMask(*get('?'));
}

ScreenFont::~ScreenFont() {
    for (int i = 0; i < 256; ++i)
        delete chars[i];
}

}
