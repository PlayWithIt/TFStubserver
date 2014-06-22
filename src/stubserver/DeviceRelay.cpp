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
#include <stdexcept>

#include <bricklet_industrial_quad_relay.h>
#include <bricklet_industrial_digital_out_4.h>
#include <bricklet_dual_relay.h>
#include <bricklet_remote_switch.h>

#include <utils/Exceptions.h>
#include <utils/Log.h>

#include "BrickStack.h"
#include "DeviceRelay.h"

namespace stubserver {

using utils::Log;

/**
 * Just basic init.
 */
DeviceRelay::DeviceRelay(unsigned n, bool _bitSwitches)
  : DeviceFunctions(), numSwitches(n), bitSwitches(_bitSwitches), callbacks()
{
    if (n > 16)
        throw std::invalid_argument("numSwitches must be <= 16, but is larger");
    bzero(switchOn, sizeof(switchOn));
    bzero(functionCodes, sizeof(functionCodes));
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
        stateChanged = true;

        if (!bitSwitches)
        {
            for (unsigned x = 0; x < numSwitches; ++x)
                switchOn[x] = p.fullData.payload[x] != 0;
            return true;
        }

        unsigned bitMask = 1;
        for (unsigned x = 0; x < numSwitches; ++x)
        {
            switchOn[x] = p.uint16Value & bitMask;
            bitMask <<= 1;
        }
        return true;
    }

    if (func == functionCodes[FUNC_SET_SELECTED])
    {
        if (!bitSwitches)
        {
            uint8_t n = p.uint8Value;
            if (--n > numSwitches) {
                Log::log("Invalid switch number for dual relay:", n);
                return false;
            }
            switchOn[n] = p.fullData.payload[1] != 0;
            return true;
        }

        uint16_t selection = p.ushorts.value1;
        uint16_t bits      = p.ushorts.value2;

        for (unsigned i = 0; i < numSwitches; ++i)
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

        if (!bitSwitches)
        {
            for (unsigned x = 0; x < numSwitches; ++x)
                p.fullData.payload[x] = switchOn[x];
            return true;
        }

        uint16_t result = 0;
        for (unsigned i = 0; i < numSwitches; ++i) {
            if (switchOn[i])
                result |= (1 << i);
        }
        p.uint16Value = result;
        return true;
    }

    if (func == functionCodes[FUNC_GET_MONOFLOP])
    {
        // read monoflop for dual relay
        if (!bitSwitches)
        {
            uint8_t n = p.uint8Value;
            if (--n > numSwitches) {
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
        if (!bitSwitches)
        {
            uint8_t n = p.uint8Value;
            if (--n > numSwitches) {
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

        for (unsigned i = 0; i < numSwitches; ++i)
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
                    Log::log("LOGIC ERROR: callback not found, PIN was", (int)i);
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

            if (bitSwitches)
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
 * Init the callback array with one callback per switch.
 */
void DeviceRelay::initMonoflopCallbacks(uint8_t callbackCode)
{
    for (unsigned i = 0; i < numSwitches; ++i)
    {
        BasicCallback cb(0, 0, callbackCode, i);
        callbacks.push_back(cb);
    }
}

/**
 * Init dual relay with get/set and monoflop.
 */
DeviceDualRelay::DeviceDualRelay()
  : DeviceRelay(2, false)
{
    functionCodes[FUNC_SET_STATE] = DUAL_RELAY_FUNCTION_SET_STATE;
    functionCodes[FUNC_SET_SELECTED] = DUAL_RELAY_FUNCTION_SET_SELECTED_STATE;
    functionCodes[FUNC_GET_STATE] = DUAL_RELAY_FUNCTION_GET_STATE;
    functionCodes[FUNC_SET_MONOFLOP] = DUAL_RELAY_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = DUAL_RELAY_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(DUAL_RELAY_CALLBACK_MONOFLOP_DONE);
}

/**
 * Init quad relay with get/set and monoflop.
 */
DeviceQuadRelay::DeviceQuadRelay()
  : DeviceRelay(4, true)
{
    functionCodes[FUNC_SET_STATE] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_VALUE;
    functionCodes[FUNC_SET_SELECTED] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_SELECTED_VALUES;
    functionCodes[FUNC_GET_STATE] = INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_VALUE;
    functionCodes[FUNC_SET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(INDUSTRIAL_QUAD_RELAY_CALLBACK_MONOFLOP_DONE);
}

/**
 * Init digital out 4 with get/set and monoflop: behaves like a relay.
 */
DeviceDigitalOut4::DeviceDigitalOut4()
  : DeviceRelay(4, true)
{
    functionCodes[FUNC_SET_STATE] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_VALUE;
    functionCodes[FUNC_SET_SELECTED] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_SELECTED_VALUES;
    functionCodes[FUNC_GET_STATE] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_VALUE;
    functionCodes[FUNC_SET_MONOFLOP] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(INDUSTRIAL_DIGITAL_OUT_4_CALLBACK_MONOFLOP_DONE);
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

} /* namespace stubserver */
