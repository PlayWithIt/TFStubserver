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
#include "VisualizationClient.h"

namespace stubserver {

/**
 * Simulates the LCD device with some lines of text and a cursor.
 * The backlight flag is simulated with other functions.
 */
class DeviceLCD : public DeviceFunctions, public LcdState
{
    int         counter;            // default text counter
    uint8_t     customChar[8*8];
    std::string defaultText[MAX_LINES];

    /**
     * Dumps the contents of the LCD to stdout.
     */
    void dump() const;

public:
    DeviceLCD(unsigned _cols, unsigned _lines);

    /**
     * Return the char definition of the given customer char as an array of
     * 8 bytes. The index must be in the customer char range (0..7) otherwise
     * NULL will be returned.
     */
    const uint8_t* getCustomerChar(unsigned index) const override;

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* DEVICELCD_H_ */
