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

    // customer character holds 8 bytes
    other = new GetSet<uint64_t>(LCD_20X4_FUNCTION_GET_CUSTOM_CHARACTER, LCD_20X4_FUNCTION_SET_CUSTOM_CHARACTER);
    other = new ArrayDevice(other, 8);
}

/**
 *
 */
bool DeviceLCD::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualisationClient &visualisationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);
    unsigned l, c, x;

    switch (p.header.function_id) {
    case LCD_20X4_FUNCTION_WRITE_LINE:
        l = p.fullData.payload[0];
        c = p.fullData.payload[1];
        if (l >= lines || c >= cols) {
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
        utils::Log() << "New data: " << text[l];

        changedLine = l;
        notify(visualisationClient, TEXT_CHANGE);
        return true;

    case LCD_20X4_FUNCTION_CLEAR_DISPLAY:
        for (unsigned i = 0; i < lines; ++i) {
            text[i] = std::string(cols, ' ');
        }
        notify(visualisationClient, CLEAR_SCREEN);
        return true;

    case LCD_20X4_FUNCTION_BACKLIGHT_ON:
        backlightOn = true;
        notify(visualisationClient, LIGHT_CHANGE);
        return true;

    case LCD_20X4_FUNCTION_BACKLIGHT_OFF:
        backlightOn = false;
        notify(visualisationClient, LIGHT_CHANGE);
        return true;

    case LCD_20X4_FUNCTION_SET_CONFIG:
        cursorVisible = p.fullData.payload[0] != 0;
        blinking      = p.fullData.payload[1] != 0;

        notify(visualisationClient, CURSOR_CHANGE);
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
        for (c = 0; c < cols; ++c)
            defaultText[l][c] = *ch++;
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

    return other->consumeCommand(relativeTimeMs, p, visualisationClient);
}

/**
 * Sets the default text if the counter has reached 0 or triggers some
 * button press/release events if those are active.
 */
void DeviceLCD::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualisationClient &visualisationClient)
{
    unsigned newValue = 0;

    if (visualisationClient.useAsInputSource())
        newValue = visualisationClient.getInputState();

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
            packet.boolValue = n;
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
        notify(visualisationClient, TEXT_CHANGE);
    }
    else if (counter > 0)
        --counter;
}

} /* namespace stubserver */
