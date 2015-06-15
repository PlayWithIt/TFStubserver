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

#include <utils/Log.h>

#include "BrickStack.h"
#include "DeviceLCD.h"

namespace stubserver {

/**
 * Default init.
 */
DeviceLCD::DeviceLCD(unsigned _cols, unsigned _lines)
    : LcdState(_cols, _lines)
    , counter(-1)
{
    for (unsigned i = 0; i < MAX_LINES; ++i) {
        text[i] = std::string(_cols, ' ');
        defaultText[i] = text[i];
    }
}

/**
 *
 */
bool DeviceLCD::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);
    unsigned l, c, x;

    switch (p.header.function_id) {
    case LCD_20X4_FUNCTION_WRITE_LINE:
        l = p.fullData.payload[0];
        c = p.fullData.payload[1];
        if (l >= lines || c >= cols) {
            utils::Log() << "LCD::write(l=" << l << ',' << "c=" << c << ") out of range";
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
            return true;
        }

        // 2 is the byte offset in payload
        x = 2;
        while (c < cols && p.fullData.payload[x] != 0) {
            text[l][c] = (char) p.fullData.payload[x];
            ++c;
            ++x;
        }

        cursorX = c;
        if (c >= cols) {
            cursorX = 0;
            cursorY = l+1;
            if (l >= lines)
                l = 0;
        }
        else
            cursorY = l;

        changedLine = l;
        notify(visualizationClient, TEXT_CHANGE);
        return true;

    case LCD_20X4_FUNCTION_CLEAR_DISPLAY:
        for (unsigned i = 0; i < lines; ++i) {
            text[i] = std::string(cols, ' ');
        }
        notify(visualizationClient, CLEAR_SCREEN);
        return true;

    case LCD_20X4_FUNCTION_BACKLIGHT_ON:
        backlightOn = true;
        notify(visualizationClient, LIGHT_CHANGE);
        return true;

    case LCD_20X4_FUNCTION_BACKLIGHT_OFF:
        backlightOn = false;
        notify(visualizationClient, LIGHT_CHANGE);
        return true;

    case LCD_20X4_FUNCTION_SET_CONFIG:
        cursorVisible = p.fullData.payload[0] != 0;
        blinking      = p.fullData.payload[1] != 0;

        notify(visualizationClient, CURSOR_CHANGE);
        return true;

    case LCD_20X4_FUNCTION_GET_CONFIG:
        p.header.length += 2;
        p.fullData.payload[0] = cursorVisible;
        p.fullData.payload[1] = blinking;
        return true;

    case LCD_20X4_FUNCTION_IS_BACKLIGHT_ON:
        p.header.length += 1;
        p.boolValue = backlightOn;
        return true;

    case LCD_20X4_FUNCTION_SET_DEFAULT_TEXT: {
        l = p.fullData.payload[0];
        if (l >= lines) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
            return true;
        }
        const uint8_t *ch = p.fullData.payload+1;
        for (c = 0; c < cols && *ch; ++c)
            defaultText[l][c] = *ch++;
    }
        return true;

    case LCD_20X4_FUNCTION_SET_CUSTOM_CHARACTER: {
        unsigned index = p.uint8Value;
        if (index > 7) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
            return true;
        }
        memcpy(customChar + (index * 8), p.fullData.payload + 1, 8);
        changedLine = index;
        notify(visualizationClient, CUSTOM_CHAR);
    }
        return true;

    case LCD_20X4_FUNCTION_GET_CUSTOM_CHARACTER: {
        unsigned index = p.uint8Value;
        if (index > 7) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
            return true;
        }
        memcpy(p.fullData.payload, customChar + (index * 8), 8);
        p.header.length += 8;
    }
        return true;

    case LCD_20X4_FUNCTION_SET_DEFAULT_TEXT_COUNTER:
        counter = p.int32Value;
        return true;

    case LCD_20X4_FUNCTION_IS_BUTTON_PRESSED:
        p.header.length += 1;
        p.fullData.payload[0] = buttonState & (1 << p.uint8Value) ? true : false;
        return true;
    }

    return false;
}

/**
 * Return the char definition of the given customer char as an array of
 * 8 bytes. The index must be in the customer char range (0..7) otherwise
 * NULL will be returned.
 */
const uint8_t* DeviceLCD::getCustomerChar(unsigned index) const
{
    if (index < 8)
        return customChar + (index*8);
    return NULL;
}

/**
 * Sets the default text if the counter has reached 0 or triggers some
 * button press/release events if those are active.
 */
void DeviceLCD::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    unsigned newValue;

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = 0;

    // TODO: no value provider to button state yet
    //else
    //    newValue = values->getValue(relativeTimeMs) & enabledBits;

    if (newValue != buttonState)
    {
        // trigger button press / release
        for (unsigned i = 0; i < 4; ++i)
        {
            bool o = buttonState & (1 << i);  // true (1) means 'pressed'
            bool n = newValue & (1 << i);

            if (o == n)
                continue;

            IOPacket packet(uid, n ? LCD_20X4_CALLBACK_BUTTON_PRESSED : LCD_20X4_CALLBACK_BUTTON_RELEASED, 1);
            packet.uint8Value = i;
            brickStack->dispatchCallback(packet);
        }
        buttonState = newValue;
    }

    if (counter == 0) {
        // display default text
        for (unsigned i = 0; i < lines; ++i) {
            text[i] = defaultText[i];
        }
        changedLine = -1;
        notify(visualizationClient, TEXT_CHANGE);
        counter = -1;
    }
    else if (counter > 0)
        --counter;
}

} /* namespace stubserver */
