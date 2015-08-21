/*
 * CallbackData.h
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

#ifndef CALLBACKDATA_H_
#define CALLBACKDATA_H_

#include "PacketTypes.h"

namespace stubserver {

/**
 * Basic callback data to hold the execution time, repeating period and
 * some parameters like the last value. The values are public in order
 * to make life easier, there is not much logic behind these values.
 */
struct BasicCallback
{
    uint64_t relativeStartTime;
    uint32_t period;             // one shot period or repeating callback (debounce)
    uint8_t  callbackCode;       // code sent back to the client when callback is triggered
    uint8_t  getPeriodFunc;      // get function for the callback period
    uint8_t  setPeriodFunc;      // set function for the callback period: this enables the callback
    int      param1;
    int      param2;
    bool     active;

    BasicCallback(uint8_t getFuncCode, uint8_t setFuncCode, uint8_t cbCode, uint32_t _period = 100);
    BasicCallback(uint64_t relativeStartTime, uint8_t setFuncCode, uint8_t cbCode, int p1, int p2 = -1, uint32_t _period = 0);

    /**
     * Update some callback attributes (those when activating / deactivating).
     */
    void update(uint64_t _relativeStartTime, uint32_t _period, int p1, int p2, bool _active = true) {
        relativeStartTime = _relativeStartTime;
        period = _period;
        param1 = p1;
        param2 = p2;
        active = _active;
    }

    /**
     * Checks if the callback is active and if the current time is after the next
     * planed execution of the callback (relativeStartTime + period).
     */
    bool mayExecute(uint64_t currentTimeMs) const;
};


/**
 * A callback that reacts whenever the value matches specific conditions.
 * param1 / 2 are the min / max values and 'period' is the debounce period.
 * <p>
 * Only the option is private in order to guarantee correct contents.
 * <P>
 * BasicCallback.getPeriodFunc is used for GET_DEBOUNCE_PERIOD
 */
class RangeCallback : public BasicCallback
{
    char     option;                     // option: 'x', '<', '>', 'o', 'i'
    uint8_t  paramSize;

public:
    uint8_t  setThresholdFunctionCode;   // set the threshold callback
    uint8_t  getThresholdFunctionCode;   // get threshold callback data

    /**
     * Default init with period == 100, and paramSize == 2.
     */
    RangeCallback(uint8_t psize = 2);

    void setParamSize(uint8_t psize) {
        paramSize = psize == 2 ? 2 : 4;
    }

    /**
     * Print current state into log-file.
     */
    void logCallbackStatus(uint32_t uid) const;

    /**
     * Change the callback option, if the option is 'x' or invalid,
     * the callback gets disabled.
     * @param o one of 'x', '<', '>', 'o', 'i'
     */
    void setOption(char o);

    /**
     * Check if the function in the packet is get/set threshold
     * @return true if the function was get/set threshold, false otherwise
     */
    bool consumeGetSetThreshold(IOPacket &io);

    /**
     * Return the current option value.
     */
    char getOption() const {
        return option;
    }

    /**
     * Set all function codes at once.
     */
    void setFunctions(uint8_t _setThresholdFunctionCode, uint8_t _getThresholdFunctionCode,
                      uint8_t _setDebounceFunctionCode, uint8_t _getDebounceFunctionCode,
                      uint8_t _callbackCode);
};

} /* namespace stubserver */

#endif /* CALLBACKINFOS_H_ */
