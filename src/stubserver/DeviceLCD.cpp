/*
 * DeviceLCD.cpp
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

#include <bricklet_lcd_20x4.h>
#include <string.h>

#include "DeviceLCD.h"

/**
 * Default init.
 */
DeviceLCD::DeviceLCD(unsigned _cols, unsigned _lines)
    : cols(_cols)
    , lines(_lines)
    , cursorX(0)
    , cursorY(0)
    , counter(-1)
    , cursor(false)
    , blinking(false)
{
    for (unsigned i = 0; i < _lines; ++i) {
        memset(chars[i], ' ', _cols);
        memset(defaultText[i], ' ', _cols);
        chars[i][_cols] = 0;
        defaultText[i][_cols] = 0;
    }

    // customer character holds 8 bytes
    other = new GetSet<uint64_t>(LCD_20X4_FUNCTION_GET_CUSTOM_CHARACTER, LCD_20X4_FUNCTION_SET_CUSTOM_CHARACTER);
    other = new ArrayDevice(other, 8);
    other = new EnableDisableBool(other, LCD_20X4_FUNCTION_BACKLIGHT_ON, LCD_20X4_FUNCTION_BACKLIGHT_OFF, LCD_20X4_FUNCTION_IS_BACKLIGHT_ON);
}

/**
 *
 */
bool DeviceLCD::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);
    unsigned l, c;

    switch (p.header.function_id) {
    case LCD_20X4_FUNCTION_WRITE_LINE:
        l = p.fullData.payload[0];
        c = p.fullData.payload[1];
        if (l >= lines || c >= cols) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
            return true;
        }

        while (c < cols && p.fullData.payload[c + 2] != 0) {
            chars[l][c] = (char) p.fullData.payload[c + 2];
            ++c;
        }
        stateChanged = true;
        return true;

    case LCD_20X4_FUNCTION_CLEAR_DISPLAY:
        for (unsigned i = 0; i < lines; ++i) {
            memset(chars[i], ' ', cols);
        }
        stateChanged = true;
        return true;

    case LCD_20X4_FUNCTION_SET_CONFIG:
        cursor   = p.fullData.payload[0] != 0;
        blinking = p.fullData.payload[1] != 0;
        stateChanged = true;
        return true;

    case LCD_20X4_FUNCTION_GET_CONFIG:
        p.header.length += 2;
        p.fullData.payload[0] = cursor;
        p.fullData.payload[1] = blinking;
        return true;

    case LCD_20X4_FUNCTION_SET_DEFAULT_TEXT:
        l = p.fullData.payload[0];
        if (l >= lines) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
            return true;
        }
        memcpy(defaultText[l], p.fullData.payload+1, cols);
        return true;

    case LCD_20X4_FUNCTION_SET_DEFAULT_TEXT_COUNTER:
        counter = p.int32Value;
        return true;

    case LCD_20X4_FUNCTION_IS_BUTTON_PRESSED:
        p.header.length += 1;
        p.fullData.payload[0] = 0;
        return true;
    }

    return other->consumeCommand(relativeTimeMs, p, stateChanged);
}

/**
 * Sets the default text if the counter has reached 0 or triggers some
 * button press/release events if those are active.
 */
void DeviceLCD::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged)
{
    if (counter == 0) {
        // display default text
        for (unsigned i = 0; i < lines; ++i) {
            memcpy(chars[i], defaultText[i], cols);
        }
        stateChanged = true;
    }
    else if (counter > 0)
        --counter;
}
