/*
 * DeviceRTC.h
 *
 * Copyright (C) 2017 Holger Grosenick
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

#ifndef STUBSERVER_DEVICERTC_H_
#define STUBSERVER_DEVICERTC_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * Class that simulates the real-time clock bricklet.
 */
class DeviceRTC : public DeviceFunctions, public RealTimeClockState
{
    int64_t  realTimeDelta;
    uint64_t relativeStartTime;    // time change callback
    uint32_t callbackPeriod;
    unsigned callbackCount;
    int8_t   offset;

    int8_t   alarmMonth;           // alarm callback
    int8_t   alarmDay;
    int8_t   alarmHour;
    int8_t   alarmMinute;
    int8_t   alarmSecond;
    int8_t   alarmWeekday;
    int32_t  alarmInterval;
    time_t   nextAlarm;
    bool     alarmActive;

    /**
     * Put the current date time into the IOPacket structure
     */
    void setDateTime(uint64_t relativeTimeMs, IOPacket &out);

public:
    DeviceRTC();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICERTC_H_ */
