/*
 * DeviceLedStrip.h
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

#ifndef STUBSERVER_DEVICELEDSTRIP_H_
#define STUBSERVER_DEVICELEDSTRIP_H_

#include "DeviceFunctions.h"

namespace stubserver {

/**
 * A dummy device for the LED strip bricklet. This makes not very much, because it
 * just consumes the data, but it calls the callback.
 */
class DeviceLedStrip : public DeviceFunctions
{
    unsigned chipType;
    unsigned frequency;
    unsigned length;
    BasicCallback frameRenderedCb;

    // pixel data
    uint8_t r[320];
    uint8_t g[320];
    uint8_t b[320];

public:
    DeviceLedStrip();

    virtual bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged) override;
    virtual void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged) override;
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICELEDSTRIP_H_ */
