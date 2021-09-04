/*
 * DeviceRelay.cpp
 *
 * Copyright (C) 2013-2021 Holger Grosenick
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
#include <stdexcept>

#include <bricklet_industrial_quad_relay.h>
#include <bricklet_industrial_quad_relay_v2.h>
#include <bricklet_industrial_digital_out_4.h>
#include <bricklet_industrial_digital_out_4_v2.h>
#include <bricklet_dual_relay.h>
#include <bricklet_remote_switch.h>
#include <bricklet_solid_state_relay.h>

#include <utils/Exceptions.h>
#include <utils/Log.h>

#include "BrickStack.h"
#include "DeviceRelay.h"

namespace stubserver {

using utils::Log;

static const uint8_t NO4[4]  = {'n', 'n', 'n', 'n' };
static const uint8_t TRUE[1] = { 1 };


/**
 * Just basic init.
 */
DeviceRelay::DeviceRelay(unsigned n, bool _bitSwitches, bool isV2)
    : V2Device(NULL, this, isV2)
    , RelayState(n)
    , bitSwitches(_bitSwitches)
    , callbacks()
{
    memset(functionCodes, 0, sizeof(functionCodes));
}

/**
 * Check for known function codes.
 */
bool DeviceRelay::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    uint8_t func = p.header.function_id;
    if (func == functionCodes[FUNC_SET_STATE])
    {
        if (!bitSwitches)
        {
            for (unsigned x = 0; x < numSwitches; ++x)
                setSwitch(x, p.fullData.payload[x] != 0);
            notify(visualizationClient, VALUE_CHANGE);
            return true;
        }

        unsigned bitMask = 1;
        for (unsigned x = 0; x < numSwitches; ++x)
        {
            setSwitch(x, p.uint16Value & bitMask);
            bitMask <<= 1;
        }
        notify(visualizationClient, VALUE_CHANGE);
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
            setSwitch(n, p.fullData.payload[1] != 0);
            notify(visualizationClient, VALUE_CHANGE);
            return true;
        }

        uint16_t selection = p.ushorts.value1;
        uint16_t bits      = p.ushorts.value2;

        for (unsigned i = 0; i < numSwitches; ++i)
        {
            uint16_t bit = (1 << i);
            if ((selection & bit) != 0)
                setSwitch(i, (bits & bit) != 0);
        }
        notify(visualizationClient, VALUE_CHANGE);
        return true;
    }

    if (func == functionCodes[FUNC_GET_STATE])
    {
        p.header.length = sizeof(p.header) + 2;

        if (!bitSwitches)
        {
            for (unsigned x = 0; x < numSwitches; ++x)
                p.fullData.payload[x] = isOn(x);
            return true;
        }

        p.uint16Value = getSwitchStates();
        return true;
    }

    if (func == functionCodes[FUNC_GET_MONOFLOP])
    {
        // read monoflop for dual relay
        if (!bitSwitches)
        {
            unsigned n = p.uint8Value;
            if (--n > numSwitches) {
                Log() << "GetMonoflop: invalid relay switch number: " << static_cast<unsigned>(p.uint8Value);
                return false;
            }
            p.header.length = sizeof(p.header) + sizeof(p.monoflopResponseDualRelay);
            if (callbacks[n].active) {
                p.monoflopResponseDualRelay.state = isOn(n);
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
        unsigned pin = p.uint8Value;
        if (pin >= numSwitches) {
            Log() << "WARN - GetMonoflop: invalid pin number for relay " << pin << " (might be group query)";
            p.monoflopResponse.value = 0;
            p.monoflopResponse.time  = 0;
            p.monoflopResponse.time_remaining = 0;
            return true;
        }

        if (callbacks[pin].active) {
            p.monoflopResponse.value = isOn(pin);
            p.monoflopResponse.time  = callbacks[pin].period;
            p.monoflopResponse.time_remaining = callbacks[pin].relativeStartTime + callbacks[pin].period - relativeTimeMs;
        }
        else {
            p.monoflopResponse.value = 0;
            p.monoflopResponse.time  = 0;
            p.monoflopResponse.time_remaining = 0;
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
            //stateChanged = true;
            setSwitch(n, p.fullData.payload[1] != 0);
            callbacks[n].update(relativeTimeMs, p.monoflopResponse.time, n+1, !isOn(n));
            notify(visualizationClient, VALUE_CHANGE);
            return true;
        }

        Log() << "SET monoflop mask 0x" << std::hex << p.monoflopDefine.selection_mask << ' '
              << p.monoflopDefine.value_mask << ", time: " << std::dec << p.monoflopDefine.time << "ms";

        for (unsigned i = 0; i < numSwitches; ++i)
        {
            uint16_t bit = (1 << i);
            if ((p.monoflopDefine.selection_mask & bit) != 0)
            {
                //stateChanged = true;
                setSwitch(i, p.monoflopDefine.value_mask & bit);
                callbacks[i].update(relativeTimeMs, p.monoflopDefine.time, i, !isOn(i));
            }
        }
        notify(visualizationClient, VALUE_CHANGE);
        return true;
    }

    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}

/**
 * Check for monoflop callbacks.
 */
void DeviceRelay::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
    {
        if (it->mayExecute(relativeTimeMs))
        {
            // execute monoflop
            Log() << "EXECUTE monoflop switch " << it->param1
                  << " value " << it->param2 << ", time: " << it->period << "ms\n";
            it->active = false;

            // dual relay responds with 2 bytes, quad-relay with 4 (by chance)
            IOPacket packet(uid, it->callbackCode, numSwitches);

            if (bitSwitches)
            {
                packet.ushorts.value1 = (1 << it->param1);
                packet.ushorts.value2 = it->param2 ? (1 << it->param1) : 0;
                setSwitch(it->param1, it->param2 != 0);
            }
            else {
                packet.fullData.payload[0] = it->param1;
                packet.fullData.payload[1] = it->param2 ? 1 : 0;
                setSwitch(it->param1 - 1, it->param2 != 0);
            }
            //stateChanged = true;
            brickStack->dispatchCallback(packet);
            notify(visualizationClient, VALUE_CHANGE);
        }
    }

    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
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
 * Init solid state relay with get/set and monoflop.
 */
DeviceSolidStateRelay::DeviceSolidStateRelay()
  : DeviceRelay(1, false, false)
{
    functionCodes[FUNC_SET_STATE] = SOLID_STATE_RELAY_FUNCTION_SET_STATE;
    functionCodes[FUNC_SET_SELECTED] = 0;
    functionCodes[FUNC_GET_STATE] = SOLID_STATE_RELAY_FUNCTION_GET_STATE;
    functionCodes[FUNC_SET_MONOFLOP] = SOLID_STATE_RELAY_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = SOLID_STATE_RELAY_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(SOLID_STATE_RELAY_CALLBACK_MONOFLOP_DONE);
}

/**
 * Check for known function codes.
 */
bool DeviceSolidStateRelay::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    uint8_t func = p.header.function_id;
    if (func == functionCodes[FUNC_SET_STATE])
    {
        setSwitch(0, p.boolValue);
        return true;
    }

    if (func == functionCodes[FUNC_GET_STATE])
    {
        p.header.length = sizeof(p.header) + 1;
        p.boolValue = isOn(0);
        return true;
    }

    if (func == functionCodes[FUNC_GET_MONOFLOP])
    {
        // read monoflop
        p.header.length = sizeof(p.header) + sizeof(p.monoflopResponseDualRelay);
        if (callbacks[0].active) {
            p.monoflopResponseDualRelay.state = isOn(0);
            p.monoflopResponseDualRelay.time  = callbacks[0].period;
            p.monoflopResponseDualRelay.time_remaining = callbacks[0].relativeStartTime + callbacks[0].period - relativeTimeMs;
        }
        else {
            p.monoflopResponseDualRelay.state = 0;
            p.monoflopResponseDualRelay.time  = 0;
            p.monoflopResponseDualRelay.time_remaining = 0;
        }
        return true;
    }

    if (func == functionCodes[FUNC_SET_MONOFLOP])
    {
        setSwitch(0, p.boolValue);
        callbacks[0].update(relativeTimeMs, p.monoflopResponseDualRelay.time, 1, !p.boolValue);
        return true;
    }
    return false;
}

/**
 * Check callbacks for solid state relay.
 */
void DeviceSolidStateRelay::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
    {
        if (it->mayExecute(relativeTimeMs))
        {
            // execute monoflop
            Log() << "EXECUTE solid state monoflop, value " << it->param2 << ", time: " << it->period << "ms\n";
            it->active = false;

            // responds with 1 extra byte for the current state
            IOPacket packet(uid, it->callbackCode, 1);
            packet.boolValue = it->param2;

            setSwitch(0, packet.boolValue);
            brickStack->dispatchCallback(packet);
        }
    }
}

/**
 * Init dual relay with get/set and monoflop.
 */
DeviceDualRelay::DeviceDualRelay()
  : DeviceRelay(2, false, false)
{
    functionCodes[FUNC_SET_STATE] = DUAL_RELAY_FUNCTION_SET_STATE;
    functionCodes[FUNC_SET_SELECTED] = DUAL_RELAY_FUNCTION_SET_SELECTED_STATE;
    functionCodes[FUNC_GET_STATE] = DUAL_RELAY_FUNCTION_GET_STATE;
    functionCodes[FUNC_SET_MONOFLOP] = DUAL_RELAY_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = DUAL_RELAY_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(DUAL_RELAY_CALLBACK_MONOFLOP_DONE);
}

/**
 * Returns a label for the switch: this can be just the switch number
 * of 'SW1' or the remote switch code.
 */
std::string DeviceDualRelay::getLabel(unsigned switchNo) const
{
    if (switchNo > numSwitches)
        throw std::out_of_range("DeviceDualRelay::getLabel: 'switchNo' is too high");

    char buf[16];
    sprintf(buf, "SW%u", switchNo + 1);
    return std::string(buf);
}

/**
 * Init quad relay with get/set and monoflop.
 */
DeviceQuadRelay::DeviceQuadRelay()
  : DeviceRelay(4, true, false)
{
    functionCodes[FUNC_SET_STATE] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_VALUE;
    functionCodes[FUNC_SET_SELECTED] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_SELECTED_VALUES;
    functionCodes[FUNC_GET_STATE] = INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_VALUE;
    functionCodes[FUNC_SET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(INDUSTRIAL_QUAD_RELAY_CALLBACK_MONOFLOP_DONE);

    // if it is available for group, the group might no be queried
    other = new DoNothing(INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_AVAILABLE_FOR_GROUP, 1, TRUE);
    other = new DoNothing(other, INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_GROUP, 4, NO4);
}

/**
 * Init quad relay with get/set and monoflop.
 */
DeviceQuadRelayV2::DeviceQuadRelayV2()
  : DeviceRelay(4, false, true)
{
    ledState[0] = INDUSTRIAL_QUAD_RELAY_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
    ledState[1] = INDUSTRIAL_QUAD_RELAY_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
    ledState[2] = INDUSTRIAL_QUAD_RELAY_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
    ledState[3] = INDUSTRIAL_QUAD_RELAY_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;

    functionCodes[FUNC_SET_STATE] = INDUSTRIAL_QUAD_RELAY_V2_FUNCTION_SET_VALUE;
    functionCodes[FUNC_SET_SELECTED] = INDUSTRIAL_QUAD_RELAY_V2_FUNCTION_SET_SELECTED_VALUE;
    functionCodes[FUNC_GET_STATE] = INDUSTRIAL_QUAD_RELAY_V2_FUNCTION_GET_VALUE;
    functionCodes[FUNC_SET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_V2_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = INDUSTRIAL_QUAD_RELAY_V2_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(INDUSTRIAL_QUAD_RELAY_V2_CALLBACK_MONOFLOP_DONE);
    other = nullptr;
}

/**
 * Check for known function codes.
 */
bool DeviceQuadRelayV2::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);
    uint8_t func = p.header.function_id;

    if (func == functionCodes[FUNC_GET_MONOFLOP])
    {
        uint8_t n = p.uint8Value;
        if (n >= numSwitches) {
            Log() << "DeviceQuadRelayV2: invalid relay switch number: " << static_cast<unsigned>(p.uint8Value);
            return false;
        }
        p.header.length = sizeof(p.header) + sizeof(p.monoflopResponseDualRelay);
        if (callbacks[n].active) {
            p.monoflopResponseDualRelay.state = isOn(n);
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

    if (func == functionCodes[FUNC_SET_MONOFLOP])
    {
        uint8_t n = p.uint8Value;
        if (n >= numSwitches) {
            Log::log("Invalid switch number for dual relay:", n);
            return false;
        }

        Log() << "SET QuadRelayV2 monoflop channel " << static_cast<unsigned>(n) << " value "
              << static_cast<unsigned>(p.fullData.payload[1]) << ", time: " << std::dec << p.monoflopDefine.time << "ms";

        //stateChanged = true;
        setSwitch(n, p.fullData.payload[1] != 0);
        callbacks[n].update(relativeTimeMs, p.monoflopResponse.time, n, !isOn(n));
        notify(visualizationClient, VALUE_CHANGE);
        return true;
    }

    if (func == functionCodes[FUNC_SET_SELECTED])
    {
        uint8_t n = p.uint8Value;
        if (n >= numSwitches) {
            Log::log("Invalid switch number for dual relay:", n);
            return false;
        }
        setSwitch(n, p.fullData.payload[1] != 0);
        notify(visualizationClient, VALUE_CHANGE);
        return true;
    }

    if (func == functionCodes[FUNC_GET_STATE])
    {
        p.header.length = sizeof(p.header) + 1;
        p.uint8Value = getSwitchStates();
        return true;
    }

    if (DeviceRelay::consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;

    if (func == INDUSTRIAL_QUAD_RELAY_V2_FUNCTION_GET_CHANNEL_LED_CONFIG)
    {
        p.header.length += 1;
        uint8_t n = p.fullData.payload[0];
        if (n < 4)
            p.fullData.payload[0] = ledState[n];
        return true;
    }

    if (func == INDUSTRIAL_QUAD_RELAY_V2_FUNCTION_SET_CHANNEL_LED_CONFIG)
    {
        uint8_t n = p.fullData.payload[0];
        if (n < 4)
            ledState[n] = p.fullData.payload[1];
        notify(visualizationClient, VALUE_CHANGE);
        return true;
    }

    return false;
}

/**
 * Check for monoflop callbacks.
 */
void DeviceQuadRelayV2::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
    {
        if (it->mayExecute(relativeTimeMs))
        {
            // execute monoflop
            Log() << "EXECUTE QuadRelayV2 monoflop switch " << it->param1
                  << " value " << it->param2 << ", time: " << it->period << "ms\n";
            it->active = false;

            // callback has channel + current state
            IOPacket packet(uid, it->callbackCode, 2);
            packet.fullData.payload[0] = it->param1;
            packet.fullData.payload[1] = it->param2 ? 1 : 0;
            setSwitch(it->param1, it->param2 != 0);

            //stateChanged = true;
            brickStack->dispatchCallback(packet);
            notify(visualizationClient, VALUE_CHANGE);
        }
    }

    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
}

/**
 * Init digital out 4 with get/set and monoflop: behaves like a relay.
 */
DeviceDigitalOut4::DeviceDigitalOut4(bool isV2)
  : DeviceRelay(4, ! isV2, isV2)
{
    if (isV2) {
        throw std::runtime_error("DeviceDigitalOut4 V2 not implemented");
    }
    else {
        functionCodes[FUNC_SET_STATE] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_VALUE;
        functionCodes[FUNC_SET_SELECTED] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_SELECTED_VALUES;
        functionCodes[FUNC_GET_STATE] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_VALUE;
        functionCodes[FUNC_SET_MONOFLOP] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_SET_MONOFLOP;
        functionCodes[FUNC_GET_MONOFLOP] = INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_MONOFLOP;
        initMonoflopCallbacks(INDUSTRIAL_DIGITAL_OUT_4_CALLBACK_MONOFLOP_DONE);

        other = new DoNothing(INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_AVAILABLE_FOR_GROUP, 1);
        other = new DoNothing(other, INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_GROUP, 4, NO4);
    }
}



/**
 * Default init.
 */
DeviceRemoteRelay::DeviceRemoteRelay()
  : RelayState(0)
  , busy(false)
  , repeats(5)
  , switchDoneAtMs(0)
{ }

void DeviceRemoteRelay::updateRelay(const char *id, uint8_t state)
{
    for (unsigned i = 0; i < numSwitches; ++i)
    {
        if (codes[i].compare(id) == 0)
        {
            setSwitch(i, state != REMOTE_SWITCH_SWITCH_TO_OFF);
            return;
        }
    }
    if (numSwitches < 16)
    {
        // add one more relay
        codes[numSwitches] = id;
        ++numSwitches;
        setSwitch(numSwitches - 1, state != REMOTE_SWITCH_SWITCH_TO_OFF);
    }
    else {
        Log::error("DeviceRemoteRelay: relay code array overflow!");
    }
}

/**
 * Check for known function codes.
 */
bool DeviceRemoteRelay::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    char buf[32];
    const char *fmt;

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
    case REMOTE_SWITCH_FUNCTION_SWITCH_SOCKET_C:
        if (busy) {
            Log::log("RemoteRelay still busy, switch command ignored..");
            return true;
        }
        busy = true;
        switchDoneAtMs = relativeTimeMs + (repeats * 100);

        fmt = p.header.function_id == REMOTE_SWITCH_FUNCTION_SWITCH_SOCKET_C ? "Type C:\nSC: %c\nDC: %d" : "Type A:\nHC: 0x%X\nRC: 0x%X";
        sprintf(buf, fmt, p.fullData.payload[0], p.fullData.payload[1]);
        updateRelay(buf, p.fullData.payload[2]);
        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case REMOTE_SWITCH_FUNCTION_SWITCH_SOCKET_B:
        if (busy) {
            Log::log("RemoteRelay still busy, switch command ignored..");
            return true;
        }
        busy = true;
        switchDoneAtMs = relativeTimeMs + (repeats * 100);

        sprintf(buf, "Type B:\nA: %u\nU: %u",
                p.uint32Value, p.fullData.payload[4]);
        updateRelay(buf, p.fullData.payload[5] ? true : false);
        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case REMOTE_SWITCH_FUNCTION_DIM_SOCKET_B:
        if (busy) {
            Log::log("RemoteRelay still busy, switch command ignored..");
            return true;
        }
        busy = true;
        switchDoneAtMs = relativeTimeMs + (repeats * 100);

        //notify(visualizationClient, VALUE_CHANGE);
        return true;
    }

    if (other)
        return other->consumeCommand(relativeTimeMs, p, visualizationClient);
    return false;
}

/**
 * Check for the switchDone callbacks.
 */
void DeviceRemoteRelay::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (busy && switchDoneAtMs <= relativeTimeMs)
    {
        // trigger switching done callback
        IOPacket packet(uid, REMOTE_SWITCH_CALLBACK_SWITCHING_DONE);
        brickStack->dispatchCallback(packet);
        busy = false;
    }
}


/**
 * Returns a label for the switch: this can be just the switch number
 * of 'SW1' or the remote switch code.
 */
std::string DeviceRemoteRelay::getLabel(unsigned switchNo) const
{
    if (switchNo > numSwitches)
        throw std::out_of_range("DeviceRemoteRelay::getLabel: 'switchNo' is too high");

    return codes[switchNo];
}

} /* namespace stubserver */
