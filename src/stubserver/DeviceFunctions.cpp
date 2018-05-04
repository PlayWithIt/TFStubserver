/*
 * DeviceFunctions.cpp
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

#include <memory.h>

#include <bindings/bricklet_rgb_led_button.h>

#include <utils/Log.h>
#include <utils/Exceptions.h>

#include <BrickStack.h>
#include "DeviceFunctions.h"

namespace stubserver {

using utils::Log;


ResponseData::ResponseData(unsigned _size, const uint8_t *initBytes)
  : extData(NULL)
  , size(_size)
{
    bzero(bytes, sizeof(bytes));
    if (_size > 0)
    {
        if (_size > sizeof(bytes))
            throw utils::OutOfRange("ResponseData::size too large", _size, sizeof(bytes));
        if (initBytes)
            memcpy(bytes, initBytes, _size);
    }
}

ResponseData::ResponseData(uint8_t *_extData, unsigned _size)
  : extData(_extData)
  , size(_size)
{
    bzero(bytes, sizeof(bytes));
    if (_size <= 0)
        throw utils::OutOfRange("ResponseData::size must be > 0");

    // generally not supported to use more than 8 bytes
    if (_size > sizeof(bytes))
        throw utils::OutOfRange("ResponseData::size too large", _size, sizeof(bytes));
}

//-----------------------------------------------------------------------------
//----- DeviceFunctions
//-----------------------------------------------------------------------------
DeviceFunctions::DeviceFunctions()
  : other(NULL)
{ }

DeviceFunctions::DeviceFunctions(DeviceFunctions *o)
  : other(o)
{ }

DeviceFunctions::DeviceFunctions(const DeviceFunctions& o)
  : other(NULL)
{
    // Do not really copy anything!
    // The callbacks are private and the mutex too...
}

DeviceFunctions* DeviceFunctions::clone() const
{
    throw utils::Exception("Raw DeviceFunctions is not cloneable!");
    return NULL;
}

DeviceFunctions::~DeviceFunctions()
{
    delete other;
}


/**
 * Trigger a callback where the init16 value is set.
 */
void DeviceFunctions::triggerCallbackShort(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, BasicCallback &cb, int16_t value)
{
    IOPacket packet(uid, cb.callbackCode, 2);
    packet.int16Value = value;
    cb.relativeStartTime = relativeTimeMs;
    brickStack->dispatchCallback(packet);
}

/**
 * Trigger a callback where the init32 value is set.
 */
void DeviceFunctions::triggerCallbackInt(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, BasicCallback &cb, int32_t value)
{
    IOPacket packet(uid, cb.callbackCode, 4);
    packet.int32Value = value;
    cb.relativeStartTime = relativeTimeMs;
    brickStack->dispatchCallback(packet);
}

//-----------------------------------------------------------------------------
//----- ArrayDevice
//-----------------------------------------------------------------------------

/**
 * Init with one instance which will be cloned.
 */
ArrayDevice::ArrayDevice(DeviceFunctions *_other, unsigned numDevices)
  : others(numDevices)
{
    others[0] = _other;
    for (size_t i = 1; i < numDevices; ++i)
        others[i] = _other->clone();
}

/** delete all objects in the vector */
ArrayDevice::~ArrayDevice()
{
    for (auto it : others)
        delete it;
}


bool ArrayDevice::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    uint8_t itemNo = p.uint8Value;

    memmove(p.fullData.payload, p.fullData.payload + 1, sizeof(p.fullData.payload) - 1);
    if (itemNo < others.size())
        return others[itemNo]->consumeCommand(relativeTimeMs, p, visualizationClient);

    if (itemNo == 255)
    {
        printf("Delegate to ALL other devices\n");
        bool result = false;
        for (auto it : others)
            result = it->consumeCommand(relativeTimeMs, p, visualizationClient);
        return result;
    }

    Log::log("Array index out of bounds:", itemNo);
    p.header.length = sizeof(p.header);
    p.setErrorCode(IOPacket::ErrorCode::INVALID_PARAMETER);
    return true;
}


/**
 * Trigger all other objects.
 */
void ArrayDevice::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    for (auto it : others) {
        it->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
    }
}


//-----------------------------------------------------------------------------
//----- DoNothing
//-----------------------------------------------------------------------------

/**
 * Base init, nothing special
 */
DoNothing::DoNothing(DeviceFunctions *_other, uint8_t funcCode, unsigned retSize, const uint8_t *initBytes)
  : DeviceFunctions(_other)
  , functionCodes()
  , response(retSize, initBytes)
{
    functionCodes.push_back(funcCode);
}


DoNothing::DoNothing(uint8_t funcCode, unsigned retSize, const uint8_t *initBytes)
  : functionCodes()
  , response(retSize, initBytes)
{
    functionCodes.push_back(funcCode);
}

DoNothing::DoNothing(const DoNothing& rhs)
  : functionCodes(rhs.functionCodes)
  , response(rhs.response)
{
    if (rhs.other)
        other = rhs.other->clone();
}


DoNothing::~DoNothing()
{
}

/**
 * First check if the other function supports this packet, if not
 * check for this function and ignore it.
 */
bool DoNothing::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    for (auto it : functionCodes)
    {
        if (it == p.header.function_id) {
            p.header.length = sizeof(p.header) + response.size;
            bzero(p.fullData.payload, sizeof(p.fullData.payload));
            if (response.size > 0) {
                memcpy(p.fullData.payload, response.bytes, response.size);
            }
            return true;
        }
    }

    if (other && other->consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;

    return false;
}

DeviceFunctions* DoNothing::clone() const
{
    DoNothing* result = new DoNothing(*this);
    return result;
}

/**
 * Just delegate to other function set since here we have no callbacks.
 */
void DoNothing::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
}


//-----------------------------------------------------------------------------
//----- GetSetRaw
//-----------------------------------------------------------------------------

/**
 * Base init, nothing special
 */
GetSetRaw::GetSetRaw(DeviceFunctions *_other, uint8_t getFunc, uint8_t setFunc, unsigned retSize, uint8_t *initBytes)
  : DeviceFunctions(_other)
  , getFunction(getFunc)
  , getFunctionIntermediate(0)
  , setFunction(setFunc)
  , response(retSize, initBytes)
  , getValueCb(0, 0, 0, 0)
{
}


GetSetRaw::GetSetRaw(uint8_t getFunc, uint8_t setFunc, unsigned retSize, uint8_t *initBytes)
  : getFunction(getFunc)
  , getFunctionIntermediate(0)
  , setFunction(setFunc)
  , response(retSize, initBytes)
  , getValueCb(0, 0, 0, 0)
{
}

/**
 * Copy constructor
 */
GetSetRaw::GetSetRaw(const GetSetRaw& rhs)
    : getFunction(rhs.getFunction)
    , getFunctionIntermediate(rhs.getFunctionIntermediate)
    , setFunction(rhs.setFunction)
    , response(rhs.response)
    , getValueCb(rhs.getValueCb)
{
    if (rhs.other)
        other = rhs.other->clone();
}


GetSetRaw::~GetSetRaw()
{
}

void GetSetRaw::enableCallback(uint8_t getCallbackFuncCode, uint8_t setCallbackFuncCode, uint8_t callbackCode)
{
    getValueCb.getPeriodFunc = getCallbackFuncCode;
    getValueCb.setPeriodFunc = setCallbackFuncCode;
    getValueCb.callbackCode = callbackCode;
}

/**
 * First check if the other function supports this packet, if not
 * check for this function and ignore it.
 */
bool GetSetRaw::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    p.header.length = sizeof(p.header);

    // since the intermediate value is calculated each millisecond, the result is always up to date.
    if (getFunction == p.header.function_id || getFunctionIntermediate == p.header.function_id)
    {
        p.header.length += response.size;
        bzero(p.fullData.payload, sizeof(p.fullData.payload));
        if (response.size > 0) {
            memcpy(p.fullData.payload, response.bytes, response.size);
        }
        return true;
    }

    if (setFunction == p.header.function_id) {
        // call is virtual, may also update intermediate value
        changeTargetValue(p);
        return true;
    }
    if (getValueCb.getPeriodFunc == p.header.function_id) {
        p.header.length += sizeof(p.int32Value);
        p.int32Value = getValueCb.period;
        return true;
    }
    if (getValueCb.setPeriodFunc == p.header.function_id) {
        getValueCb.period = p.int32Value;
        getValueCb.relativeStartTime = relativeTimeMs;
        getValueCb.active = getValueCb.period > 0;
        return true;
    }

    if (other && other->consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;

    return false;
}

/**
 * Check for the getCurrentValue callback and call other callbacks.
 */
void GetSetRaw::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (getFunctionIntermediate != 0)
        updateIntermediateValue();

    if (getValueCb.mayExecute(relativeTimeMs)) {
        triggerCallback(uid, brickStack);
    }
    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
}

/**
 * Update the 'response' (current status) with new incoming data from the IOPaket.
 * The normal 'get' would return the response unchanged.
 */
void GetSetRaw::changeTargetValue(const IOPacket &p)
{
    memcpy(response.bytes, p.fullData.payload, response.size);
}

/**
 * Check for the getCurrentValue callback and call other callbacks.
 */
void GetSetRaw::triggerCallback(unsigned int uid, BrickStack *brickStack)
{
    Log() << "EXECUTE callback " << (unsigned) getValueCb.callbackCode << " time: " <<  getValueCb.period << "ms";

    IOPacket packet(uid, getValueCb.callbackCode, response.size);
    if (response.size > 0) {
        memcpy(packet.fullData.payload, response.bytes, response.size);
    }
    getValueCb.relativeStartTime += getValueCb.period;
    brickStack->dispatchCallback(packet);
}

/**
 * Must be implemented in derived classes if the specific method
 * supports intermediate values. Otherwise these methods should
 * not be called from the internal logic anyway.
 */
void GetSetRaw::updateIntermediateValue()
{
    if (getFunctionIntermediate == 0)
        return;

    char msg[256];
    sprintf(msg, "GetSetRaw::updateIntermediateValue() called, but not implemented, funcCode %u", getFunctionIntermediate);
    throw std::logic_error(msg);
}


//-----------------------------------------------------------------------------
//----- GetSetRaw
//-----------------------------------------------------------------------------

/**
 * Base init, nothing special
 */
EnableDisableBool::EnableDisableBool(DeviceFunctions *_other, uint8_t _enableFunction, uint8_t _disableFunction, uint8_t _getFunction)
  : DeviceFunctions(_other)
  , enableFunction(_enableFunction)
  , disableFunction(_disableFunction)
  , getFunction(_getFunction)
  , enabled(false)
{
}

EnableDisableBool::EnableDisableBool(const EnableDisableBool& rhs)
  : enableFunction(rhs.enableFunction)
  , disableFunction(rhs.disableFunction)
  , getFunction(rhs.getFunction)
  , enabled(rhs.enabled)
{
    if (rhs.other)
        other = rhs.other->clone();
}


EnableDisableBool::~EnableDisableBool()
{
}

DeviceFunctions* EnableDisableBool::clone() const
{
    DeviceFunctions* result = new EnableDisableBool(*this);
    return result;
}

/**
 * First check if the other function supports this packet, if not
 * check for this function and ignore it.
 */
bool EnableDisableBool::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    p.header.length = sizeof(p.header);
    if (getFunction == p.header.function_id) {
        ++p.header.length;
        p.boolValue = enabled;
        return true;
    }
    if (enableFunction == p.header.function_id) {
        enabled = true;
        return true;
    }
    if (disableFunction == p.header.function_id) {
        enabled = false;
        return true;
    }

    if (other && other->consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;

    return false;
}

/**
 * Just delegate to other function set since here we have no callbacks.
 */
void EnableDisableBool::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
}

//-----------------------------------------------------------------------------
//----- V2Devcice
//-----------------------------------------------------------------------------

/**
 * Base init, nothing special
 */
V2Device::V2Device(DeviceFunctions *_other, VisibleDeviceState *state)
  : DeviceFunctions(_other)
  , visibleDeviceState(state)
{
}


V2Device::~V2Device()
{
}

/**
 * First check if the other function supports this packet, if not
 * check for this function and ignore it.
 */
bool V2Device::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    p.header.length = sizeof(p.header);

    switch (p.header.function_id) {
    case FUNCTION_SET_STATUS_LED_CONFIG:
        if (visibleDeviceState) {
            visibleDeviceState->setStatusLedConfig(p.fullData.payload[0]);
            visibleDeviceState->notify(visualizationClient, SensorState::LED_CHANGE);
        }
        return true;

    case FUNCTION_GET_STATUS_LED_CONFIG:
        if (visibleDeviceState)
            p.fullData.payload[0] = visibleDeviceState->getStatusLedConfig();
        else
            p.fullData.payload[0] = STATUS_LED_HEARTBEAT;
        p.header.length += 1;
        return true;

    case FUNCTION_GET_BOOTLOADER_MODE:
        p.uint8Value = RGB_LED_BUTTON_BOOTLOADER_MODE_FIRMWARE;                   // default: firmware mode
        p.header.length += 1;
        return true;

    case FUNCTION_SET_BOOTLOADER_MODE:
        // ignore
        return true;

    case FUNCTION_BRICKLET_RESET:
        return true;
    }

    if (other && other->consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;
    return false;
}

/**
 * Just delegate to other function set since here we have no callbacks.
 */
void V2Device::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
}

} /* namespace stubserver */
