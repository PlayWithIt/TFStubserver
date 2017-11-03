/*
 * DeviceRTC.cpp
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

#include <sys/time.h>
#include <ctime>

#include <bricklet_real_time_clock.h>

#include "BrickStack.h"
#include "DeviceRTC.h"

namespace stubserver {

DeviceRTC::DeviceRTC()
    : realTimeDelta(0)
    , relativeStartTime(0)
    , callbackPeriod(0)
    , callbackCount(0)
    , offset(0)
    , alarmMonth(-1)
    , alarmDay(-1)
    , alarmHour(-1)
    , alarmMinute(-1)
    , alarmSecond(-1)
    , alarmWeekday(-1)
    , alarmInterval(-1)
    , nextAlarm(-1)
    , alarmActive(false)
{
    brickletTime = time(NULL);
}

/**
 * Put the current date time into the IOPacket structure
 */
void DeviceRTC::setDateTime(uint64_t relativeTimeMs, IOPacket &out)
{
    std::tm tmData;

    // copy away as fast as possible
    memcpy(&tmData, std::localtime(&brickletTime), sizeof(tmData));

    // uint16_t *ret_year, uint8_t *ret_month, uint8_t *ret_day, uint8_t *ret_hour, uint8_t *ret_minute, uint8_t *ret_second, uint8_t *ret_centisecond, uint8_t *ret_weekday
    out.uint16Value         = tmData.tm_year + 1900;
    out.fullData.payload[2] = tmData.tm_mon + 1;
    out.fullData.payload[3] = tmData.tm_mday;
    out.fullData.payload[4] = tmData.tm_hour;
    out.fullData.payload[5] = tmData.tm_min;
    out.fullData.payload[6] = tmData.tm_sec;
    out.fullData.payload[7] = (relativeTimeMs % 1000) / 10;  // not millis, but 100th
    out.fullData.payload[8] = tmData.tm_wday == 0 ? REAL_TIME_CLOCK_WEEKDAY_SUNDAY : tmData.tm_wday;

    uint64_t timestamp = brickletTime * 1000 + relativeTimeMs % 1000;  // value in milliseconds
    memcpy(out.fullData.payload + 9, &timestamp, sizeof(timestamp));
}

/**
 * Consume client commands.
 */
bool DeviceRTC::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    std::tm tmData;
    int8_t *curr;

    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case REAL_TIME_CLOCK_FUNCTION_SET_DATE_TIME:
        // uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t centisecond, uint8_t weekday
        // ignore the incoming weekday, this gets calculated !
        tmData.tm_year  = p.uint16Value - 1900;
        tmData.tm_mon   = p.fullData.payload[2] - 1;
        tmData.tm_mday  = p.fullData.payload[3];
        tmData.tm_hour  = p.fullData.payload[4];
        tmData.tm_min   = p.fullData.payload[5];
        tmData.tm_sec   = p.fullData.payload[6];
        tmData.tm_yday  = 0;
        tmData.tm_wday  = 0;
        tmData.tm_isdst = -1;

        brickletTime  = mktime(&tmData);
        realTimeDelta = time(NULL) - brickletTime;
        return true;

    case REAL_TIME_CLOCK_FUNCTION_GET_DATE_TIME:
        // uint16_t *ret_year, uint8_t *ret_month, uint8_t *ret_day, uint8_t *ret_hour, uint8_t *ret_minute, uint8_t *ret_second, uint8_t *ret_centisecond, uint8_t *ret_weekday
        p.header.length += 9;
        setDateTime(relativeTimeMs, p);
        return true;

    case REAL_TIME_CLOCK_FUNCTION_GET_TIMESTAMP:
        p.header.length += sizeof(p.int64Value);
        p.int64Value = brickletTime * 1000 + relativeTimeMs % 1000;  // value in milliseconds
        return true;

    case REAL_TIME_CLOCK_FUNCTION_SET_DATE_TIME_CALLBACK_PERIOD:
        relativeStartTime = relativeTimeMs;
        callbackPeriod = p.uint32Value;
        return true;

    case REAL_TIME_CLOCK_FUNCTION_GET_DATE_TIME_CALLBACK_PERIOD:
        p.uint32Value = callbackPeriod;
        return true;

    case REAL_TIME_CLOCK_FUNCTION_SET_ALARM:
        alarmActive = false;

        // int8_t month, int8_t day, int8_t hour, int8_t minute, int8_t second, int8_t weekday, int32_t interval
        curr = &p.int8Value;
        alarmMonth = *curr;
        if (alarmMonth >= 0)
            alarmActive = true;
        ++curr;

        alarmDay = *curr;
        if (alarmDay >= 0)
            alarmActive = true;
        ++curr;

        alarmHour = *curr;
        if (alarmHour >= 0)
            alarmActive = true;
        ++curr;

        alarmMinute = *curr;
        if (alarmMinute >= 0)
            alarmActive = true;
        ++curr;

        alarmSecond = *curr;
        if (alarmSecond >= 0)
            alarmActive = true;
        ++curr;

        alarmWeekday = *curr;
        if (alarmWeekday >= 0)
            alarmActive = true;
        ++curr;

        alarmInterval = *((int32_t*)curr);

        if (alarmActive)
            // date/time is set: wait until the time is reached (time will not match nextAlarm)
            nextAlarm = brickletTime - 1;
        else
            // no date/time is set: brickletTime will reach nextAlarm if alarmInterval > 0
            nextAlarm = brickletTime + alarmInterval;

        printf("Alarm hour: %d, interval: %d, active: %d, next: %lX\n", alarmHour, alarmInterval, alarmActive, nextAlarm);
        return true;

    case REAL_TIME_CLOCK_FUNCTION_GET_ALARM:
        p.header.length += 10;

        curr = &p.int8Value;
        *curr = alarmMonth;
        ++curr;
        *curr = alarmDay;
        ++curr;
        *curr = alarmHour;
        ++curr;
        *curr = alarmMinute;
        ++curr;
        *curr = alarmSecond;
        ++curr;
        *curr = alarmWeekday;
        ++curr;
        *((int32_t*)curr) = alarmInterval;
        return true;

    case REAL_TIME_CLOCK_FUNCTION_SET_OFFSET:
        offset = p.int8Value;
        return true;

    case REAL_TIME_CLOCK_FUNCTION_GET_OFFSET:
        p.int8Value = offset;
        p.header.length += sizeof(int8_t);
        return true;

    default:
        return false;
    }
}

void DeviceRTC::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    // update max 100 times per second
    if (++callbackCount % 10 == 0)
    {
        // update bricklet time
        time_t current = time(NULL);
        time_t newTime = current - realTimeDelta;

        if (newTime != brickletTime)
        {
            brickletTime = newTime;
            if (alarmActive || alarmInterval > 0)
            {
                IOPacket packet(uid, REAL_TIME_CLOCK_CALLBACK_ALARM, 17);  // 9 + sizeof(int64)
                setDateTime(relativeTimeMs, packet);

                bool match = false;
                if ( (alarmMonth  < 0 || alarmMonth   == packet.fullData.payload[2])
                  && (alarmDay    < 0 || alarmDay     == packet.fullData.payload[3])
                  && (alarmHour   < 0 || alarmHour    == packet.fullData.payload[4])
                  && (alarmMinute < 0 || alarmMinute  == packet.fullData.payload[5])
                  && (alarmSecond < 0 || alarmSecond  == packet.fullData.payload[6])
                  && (alarmWeekday< 0 || alarmWeekday == packet.fullData.payload[8]) )
                    match = alarmActive;

                if (match || (alarmInterval > 0 && nextAlarm == newTime)) {
                    brickStack->dispatchCallback(packet);
                    nextAlarm = newTime + alarmInterval;
                }
            }
        }

        if (callbackPeriod > 0 && relativeTimeMs > relativeStartTime)
        {
            IOPacket packet(uid, REAL_TIME_CLOCK_CALLBACK_DATE_TIME, 17);  // 9 + sizeof(int64)
            setDateTime(relativeTimeMs, packet);
            brickStack->dispatchCallback(packet);

            relativeStartTime += callbackPeriod;
        }
    }
}

} /* namespace stubserver */
