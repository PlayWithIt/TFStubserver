/*
 * DeviceLed.h
 *
 * Copyright (C) 2021 Holger Grosenick
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

#ifndef STUBSERVER_DEVICELED_H_
#define STUBSERVER_DEVICELED_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * RGB LED which just holds a color state.
 * To make the use easier, it is derived from the dual button state,
 * so that we don't have too many different kinds of UI structures.
 */
class DeviceLed : public V2Device, public DualButtonState
{
    uint8_t functionGet;
    uint8_t functionSet;

public:
    DeviceLed(uint8_t functionGet, uint8_t functionSet);
    virtual ~DeviceLed();

    /**
     * Consume get/set commands (there are no callbacks ...)
     */
    bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient) override;

};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICELED_H_ */
