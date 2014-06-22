/*
 * DeviceLCD.h
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

#ifndef DEVICELCD_H_
#define DEVICELCD_H_

#include <string>
#include "DeviceFunctions.h"

namespace stubserver {

/**
 * Simulates the LCD device with some lines of text and a cursor.
 * The backlight flag is simulated with other functions.
 */
class DeviceLCD : public DeviceFunctions
{
    static const unsigned MAX_COLS  = 20;
    static const unsigned MAX_LINES = 10;

    typedef char TextLine[MAX_COLS + 1];

    unsigned    cols, lines;
    unsigned    cursorX, cursorY;
    int         counter;            // default text counter
    bool        cursor, blinking;
    TextLine    chars[MAX_LINES];
    TextLine    defaultText[MAX_LINES];

    /**
     * Dumps the contents of the LCD to stdout.
     */
    void dump() const;

public:
    DeviceLCD(unsigned _cols, unsigned _lines);

    bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged);
    void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged);
};

} /* namespace stubserver */

#endif /* DEVICELCD_H_ */
