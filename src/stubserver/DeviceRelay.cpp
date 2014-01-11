/*
 * DeviceRelay.cpp
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

#include <cstring>
#include <sstream>

#include <bricklet_industrial_quad_relay.h>
#include <bricklet_industrial_digital_out_4.h>
#include <bricklet_dual_relay.h>
#include <bricklet_remote_switch.h>

#include <utils/Exceptions.h>
#include <utils/Log.h>

#include "BrickStack.h"
#include "DeviceRelay.h"

using utils::Log;


DeviceRelay::DeviceRelay(unsigned _type, int n)
  : DeviceFunctions(), type(_type), numSwitches(n), callbacks()
{
    bzero(switchOn, sizeof(switchOn));
    bzero(functionCodes, sizeof(functionCodes));

    uint8_t cbFunc = 0;
    if (n == 4 && type == INDUSTRIAL_QUAD_RELAY_DEVICE_IDENTIFIER)
    {
        functionCodes[FUNC_SET_STATE] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_VALUE;
        functionCodes[FUNC_SET_SELECTED] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_SELECTED_VALUES;
        functionCodes[FUNC_GET_STATE] = INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_VALUE;
        functionCodes[FUNC_SET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_MONOFLOP;
        functionCodes[FUNC_GET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_MONOFLOP;
        cbFunc = INDUSTRIAL_QUAD_RELAY_CALLBACK_MONOFLOP_DONE;
    }
    else if (n == 4 && type == INDUSTRIAL_DIGITAL_OUT_4_DEVICE_IDENTIFIER)
    {
        functionCodes[FUNC_SET_STATE] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_VALUE;
        functionCodes[FUNC_SET_SELECTED] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_SELECTED_VALUES;
        functionCodes[FUNC_GET_STATE] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_VALUE;
        functionCodes[FUNC_SET_MONOFLOP] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_MONOFLOP;
        functionCodes[FUNC_GET_MONOFLOP] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_MONOFLOP;
        cbFunc = INDUSTRIAL_DIGITAL_OUT_4_CALLBACK_MONOFLOP_DONE;
    }
    else if (n == 2)
    {
        functionCodes[FUNC_SET_STATE] = DUAL_RELAY_FUNCTION_SET_STATE;
        functionCodes[FUNC_SET_SELECTED] = DUAL_RELAY_FUNCTION_SET_SELECTED_STATE;
        functionCodes[FUNC_GET_STATE] = DUAL_RELAY_FUNCTION_GET_STATE;
        functionCodes[FUNC_SET_MONOFLOP] = DUAL_RELAY_FUNCTION_SET_MONOFLOP;
        functionCodes[FUNC_GET_MONOFLOP] = DUAL_RELAY_FUNCTION_GET_MONOFLOP;
        cbFunc = DUAL_RELAY_CALLBACK_MONOFLOP_DONE;
    }
    else
        throw utils::LogicError("Only 2/4 switches per relay are supported!");

    // one callback per switch
    for (int i = 0; i < n; ++i) {
        BasicCallback cb(0, 0, cbFunc, i);
        callbacks.push_back(cb);
    }
}

/**
 * Find the callback where param1 has the given 'pin' value.
 */
DeviceRelay::CallbackIterator DeviceRelay::findCallbackForPin(int pin)
{
    CallbackIterator last = callbacks.end();
    for (CallbackIterator it = callbacks.begin(); it != last; ++it)
    {
        if (it->param1 == pin)
            return it;
    }
    return last;
}

/**
 * Check for known function codes.
 */
bool DeviceRelay::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    uint8_t func = p.header.function_id;
    if (func == functionCodes[FUNC_SET_STATE])
    {
        if (numSwitches == 2) {
            switchOn[0] = p.fullData.payload[0] != 0;
            switchOn[1] = p.fullData.payload[1] != 0;
            return true;
        }
        switchOn[0] = p.uint16Value & 1;
        switchOn[1] = p.uint16Value & 2;
        switchOn[2] = p.uint16Value & 4;
        switchOn[3] = p.uint16Value & 8;
        stateChanged = true;
        return true;
    }

    if (func == functionCodes[FUNC_SET_SELECTED])
    {
        if (numSwitches == 2) {
            uint8_t n = p.uint8Value;
            if (--n > 1) {
                Log::log("Invalid switch number for dual relay:", n);
                return false;
            }
            switchOn[n] = p.fullData.payload[1] != 0;
            return true;
        }

        uint16_t selection = p.ushorts.value1;
        uint16_t bits      = p.ushorts.value2;

        for (int i = 0; i < numSwitches; ++i)
        {
            uint16_t bit = (1 << i);
            if ((selection & bit) != 0)
                switchOn[i] = (bits & bit) != 0;
        }
        return true;
    }

    if (func == functionCodes[FUNC_GET_STATE])
    {
        p.header.length = sizeof(p.header) + 2;

        if (numSwitches == 2) {
            p.fullData.payload[0] = switchOn[0];
            p.fullData.payload[1] = switchOn[1];
            return true;
        }

        uint16_t result = 0;
        for (int i = 0; i < numSwitches; ++i) {
            if (switchOn[i])
                result |= (1 << i);
        }
        p.uint16Value = result;
        return true;
    }

    if (func == functionCodes[FUNC_GET_MONOFLOP])
    {
        // read monoflop for dual relay
        if (numSwitches == 2)
        {
            uint8_t n = p.uint8Value;
            if (--n > 1) {
                Log::log("Invalid switch number for dual relay:", n);
                return false;
            }
            p.header.length = sizeof(p.header) + sizeof(p.monoflopResponseDualRelay);
            if (callbacks[n].active) {
                p.monoflopResponseDualRelay.state = !switchOn[n];
                p.monoflopResponseDualRelay.time  = callbacks[n].period;
                p.monoflopResponseDualRelay.time_remaining = callbacks[n].relativeStartTime + callbacks[n].period - relativeTimeMs;
            }
            else {
                p.monoflopResponseDualRelay.state = 0;
                p.monoflopResponseDualRelay.time  = 0;
                p.monoflopResponseDualRelay.time_remaining = 0;
            }
            return true;
        }

        p.header.length = sizeof(p.header) + sizeof(p.monoflopResponse);
        uint8_t pin = p.uint8Value;

        p.monoflopResponse.value = 0;
        p.monoflopResponse.time  = 0;
        p.monoflopResponse.time_remaining = 0;

        CallbackIterator it = findCallbackForPin(pin);
        if (it != callbacks.end() && it->active)
        {
            p.monoflopResponse.value = !switchOn[pin];
            p.monoflopResponse.time  = it->period;
            p.monoflopResponse.time_remaining = it->relativeStartTime + it->period - relativeTimeMs;
        }
        return true;
    }

    if (func == functionCodes[FUNC_SET_MONOFLOP])
    {
        if (numSwitches == 2)
        {
            uint8_t n = p.uint8Value;
            if (--n > 1) {
                Log::log("Invalid switch number for dual relay:", n);
                return false;
            }
            switchOn[n]         = p.fullData.payload[1] != 0;
            callbacks[n].param1 = n+1;
            callbacks[n].param2 = !switchOn[n];
            callbacks[n].period = p.monoflopResponse.time;
            callbacks[n].relativeStartTime = relativeTimeMs;
            callbacks[n].active = true;
            return true;
        }

        char msg[128];
        sprintf(msg, "SET monoflop mask %x %x, time: %dms",
                p.monoflopDefine.selection_mask, p.monoflopDefine.value_mask, p.monoflopDefine.time);
        Log::log(msg);

        for (int i = 0; i < numSwitches; ++i)
        {
            uint16_t bit = (1 << i);
            if ((p.monoflopDefine.selection_mask & bit) != 0)
            {
                switchOn[i] = p.monoflopDefine.value_mask & bit;

                bool found = false;
                CallbackIterator it = findCallbackForPin(i);
                if (it != callbacks.end())
                {
                    found = true;
                    if (p.monoflopDefine.time == 0) {
                        // clear callback
                        it->active = false;
                        break;
                    }

                    it->relativeStartTime = relativeTimeMs;
                    it->param2 = !switchOn[i];
                    it->period = p.monoflopDefine.time;
                    it->active = true;

                    std::ostringstream os;
                    os << "-> update monoflop " << it->relativeStartTime << " - " << it->period << "ms";
                    Log::log(os.str());
                    break;
                }
                if (!found)
                {
                    // callback does not exist -> logic error!!
                    Log::log("LOGIC ERROR: callback not found, PIN was", i);
                }
            }
        }
        return true;
    }
    return false;
}

/**
 * Check for monoflop callbacks.
 */
void DeviceRelay::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged)
{
    for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
    {
        if (it->mayExecute(relativeTimeMs))
        {
            // execute monoflop
            char msg[128];
            sprintf(msg, "EXECUTE monoflop switch %d value %d, time: %ums", it->param1, it->param2, it->period);
            Log::log(msg);

            it->active = false;

            // dual relay responds with 2 bytes, quad-relay with 4 (by chance)
            IOPacket packet(uid, it->callbackCode, numSwitches);

            if (numSwitches == 4)
            {
                packet.ushorts.value1 = (1 << it->param1);
                packet.ushorts.value2 = it->param2 ? (1 << it->param1) : 0;
                switchOn[it->param1] = it->param2 != 0;
            }
            else {
                packet.fullData.payload[0] = it->param1;
                packet.fullData.payload[1] = it->param2 ? 1 : 0;
                switchOn[it->param1 -1] = it->param2 != 0;
            }
            brickStack->dispatchCallback(packet);
        }
    }
}

/**
 * Default init.
 */
DeviceRemoteRelay::DeviceRemoteRelay()
  : busy(false), repeats(5), switchDoneAtMs(0)
{ }

/**
 * Check for known function codes.
 */
bool DeviceRemoteRelay::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch(p.header.function_id)
    {
    case REMOTE_SWITCH_FUNCTION_SET_REPEATS:
        if (p.uint8Value > 0)
            repeats = p.uint8Value;
        return true;

    case REMOTE_SWITCH_FUNCTION_GET_REPEATS:
        p.header.length = sizeof(p.header) + 1;
        p.uint8Value = repeats;
        return true;

    case REMOTE_SWITCH_FUNCTION_GET_SWITCHING_STATE:
        p.header.length = sizeof(p.header) + 1;
        p.uint8Value = busy;
        return true;

    case REMOTE_SWITCH_FUNCTION_SWITCH_SOCKET:
    case REMOTE_SWITCH_FUNCTION_SWITCH_SOCKET_A:
    case REMOTE_SWITCH_FUNCTION_SWITCH_SOCKET_B:
    case REMOTE_SWITCH_FUNCTION_SWITCH_SOCKET_C:
        if (busy) {
            Log::log("RemoteRelay still busy, switch command ignored..");
            return true;
        }
        busy = true;
        switchDoneAtMs = relativeTimeMs + (repeats * 100);
        return true;
    }
    return false;
}

/**
 * Check for the switchDone callbacks.
 */
void DeviceRemoteRelay::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged)
{
    if (busy && switchDoneAtMs <= relativeTimeMs)
    {
        // trigger switching done callback
        IOPacket packet(uid, REMOTE_SWITCH_CALLBACK_SWITCHING_DONE);
        brickStack->dispatchCallback(packet);
        busy = false;
    }
}
