/*
 * DeviceFunctions.h
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


#ifndef DEVICEFUNCTIONS_H_
#define DEVICEFUNCTIONS_H_

#include <vector>
#include <mutex>
#include <string.h>
#include <utils/ValueProvider.h>

#include "CallbackData.h"
#include "VisualizationClient.h"

typedef std::lock_guard<std::mutex> MutexLock;

//-----------------------------------------------------------------------
// functions defined in ip_connection.py but not in the cpp version
//-----------------------------------------------------------------------
#define FUNCTION_DISCONNECT_PROBE      128
#define FUNCTION_GET_IDENTITY          255
#define FUNCTION_ENUMERATE             254
#define FUNCTION_ADC_CALIBRATE         251
#define FUNCTION_GET_ADC_CALIBRATION   250
#define FUNCTION_READ_BRICKLET_UID     249
#define FUNCTION_WRITE_BRICKLET_UID    248
#define FUNCTION_READ_BRICKLET_PLUGIN  247
#define FUNCTION_WRITE_BRICKLET_PLUGIN 246

#define FUNCTION_BRICKLET_RESET        243
#define FUNCTION_GET_CHIP_TEMPERATURE  242
#define FUNCTION_GET_STATUS_LED_CONFIG 240
#define FUNCTION_SET_STATUS_LED_CONFIG 239
#define FUNCTION_GET_BOOTLOADER_MODE   236
#define FUNCTION_SET_BOOTLOADER_MODE   235



namespace stubserver {

using utils::ValueProvider;

// forward decl
class BrickStack;
class GetSetRaw;

/**
 * Struct to hold some predefined return data.
 */
struct ResponseData
{
    uint8_t  bytes[8];
    uint8_t *extData;
    unsigned size;

    /**
     * Copy initBytes into buffer and use this for IO.
     */
    ResponseData(unsigned _size, const uint8_t *initBytes = 0);

    /**
     * Use external data storage for transferred data.
     */
    ResponseData(uint8_t *extData, unsigned _size);

    /**
     * Read out some binary data from the array, only the first bytes
     * are used, since T is something like int32_t or unit16_t.
     */
    template<class T> T get() const {
        T* result = (T*) (extData ? extData : bytes);
        return *result;
    }

    /**
     * Set some binary data from the array.
     */
    template<class T> void set(T n) {
        T* result = (T*) (extData ? extData : bytes);
        *result = n;
    }
};


/**
 * Builds a byte array with the given type.
 */
template<class T> uint8_t* buildBytes(const T v)
{
    static union {
        uint8_t result[sizeof(T)];
        T       value;
    };
    value = v;
    return result;
}


/**
 * Class that is used as base for delegation and composition of functionality.
 * The major functionality must be implemented in {@link #consumeCommand} and
 * {@link #checkCallbacks}
 */
class DeviceFunctions
{
protected:
    DeviceFunctions* other;

    // not allowed
    DeviceFunctions& operator=(const DeviceFunctions& o) = delete;

    void triggerCallbackShort(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, BasicCallback &cb, int16_t value);
    void triggerCallbackInt  (uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, BasicCallback &cb, int32_t value);

public:
    DeviceFunctions();
    DeviceFunctions(DeviceFunctions* _other);
    DeviceFunctions(const DeviceFunctions& o);
    virtual ~DeviceFunctions();

    void setOther(DeviceFunctions* o) {
        other = o;
    }

    /** derived classes should override this: this method throws an exception by default! */
    virtual DeviceFunctions* clone() const;

    /**
     * This method is called by the BrickStack if a client command comes in.
     */
    virtual bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient) = 0;

    /**
     * This method is called periodically (about each ms) in order to check if a callback
     * must be triggered.
     */
    virtual void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient) = 0;
};

/**
 * Since we have quite a lot of derived classes: use a macro here to simplify interface changes.
 */
#define DECLARE_OWN_DEVICE_CALLBACKS \
   virtual bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient) override; \
   virtual void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient) override;


/**
 * A simple function which makes nothing and just responds to a request with a
 * packet that has zero-filled data or some predefines bytes. The length of the
 * response is one argument of the constructor.
 * <P>
 * If the object has another function object, this is called first and if the
 * other object can handle this request, nothing else is done.
 */
class DoNothing : public DeviceFunctions
{
    std::vector<uint8_t> functionCodes;
    ResponseData response;

    DoNothing& operator=(const DoNothing& rhs) = delete;

public:
    /**
     * Returns the header plus 'retSize' zero-bytes if the requested function equals 'funcCode'
     */
    DoNothing(DeviceFunctions *other, uint8_t funcCode, unsigned retSize = 0, const uint8_t *returnBytes = NULL);
    DoNothing(uint8_t funcCode, unsigned retSize = 0, const uint8_t *returnBytes = NULL);
    DoNothing(const DoNothing& rhs);
    ~DoNothing();

    /**
     * Respond to this function also with the same response as the initial one.
     */
    DoNothing& addFunc(uint8_t funcCode) {
        functionCodes.push_back(funcCode);
        return *this;
    }

    DeviceFunctions* clone() const override;

    DECLARE_OWN_DEVICE_CALLBACKS
};


/**
 * A function that serves get/set for one value with a specific data size given in
 * bytes. An initial value with max 8 bytes can be passed too.
 * <P>
 * The function also allows to handle callbacks for periodical value gets.
 */
class GetSetRaw : public DeviceFunctions
{
protected:
    uint8_t  getFunction;
    uint8_t  getFunctionIntermediate;
    uint8_t  setFunction;
    ResponseData response;

    // time in ms to the the new value, within that time intermediate values are calculated
    BasicCallback getValueCb;

    GetSetRaw& operator=(const GetSetRaw& rhs) = delete;

    void triggerCallback(unsigned int uid, BrickStack *brickStack);

    /**
     * Update the 'response' (current status) with new incoming data from the IOPaket.
     * The normal 'get' would return the response unchanged.
     */
    virtual void changeTargetValue(const IOPacket &p);

    /**
     * Calculate an intermediate value if necessary. This method is called in
     * each callback cycle, so that "getIntermediate' always might return the
     * correct value.
     */
    virtual void updateIntermediateValue();

public:
    /**
     * Returns the header plus 'retSize' zero-bytes if the requested function equals 'funcCode'
     */
    GetSetRaw(DeviceFunctions *other, uint8_t getFunc, uint8_t setFunc, unsigned retSize, uint8_t *initBytes = NULL);
    GetSetRaw(uint8_t getFunc, uint8_t setFunc, unsigned retSize, uint8_t *initBytes = NULL);
    GetSetRaw(const GetSetRaw& rhs);
    ~GetSetRaw();

    int getInt() const {
        return response.get<int>();
    }

    short getShort() const {
        return response.get<short>();
    }

    /**
     * Enables a callback that periodically triggers the current value back to the client
     */
    void enableCallback(uint8_t getCallbackFuncCode, uint8_t setCallbackFuncCode, uint8_t callbackCode);
    void setFunctionIntermediate(uint8_t f) {
        getFunctionIntermediate = f;
    }

    DECLARE_OWN_DEVICE_CALLBACKS
};

/**
 * Just a helper to set the initial value and size of the raw data in
 * a more type safe way.
 */
template<typename T> class GetSet : public GetSetRaw
{
    T        targetValue;
    int      fraction;
    bool     targetReached;
    unsigned counter;

protected:

    /**
     * If the device needs time to change a value to a target value: keep old
     * value and save the target value
     */
    virtual void changeTargetValue(const IOPacket &p) override
    {
        T oldValue = response.get<T>();                             // save old
        memcpy(response.bytes, p.fullData.payload, response.size);  // set new

        if (getFunctionIntermediate != 0) {  // check if new value has changed
            T newValue = response.get<T>();
            if (newValue != oldValue) {
                targetValue = newValue;
                double f = (double)(newValue - oldValue) / 400.0;   // should take 2000ms to change
                if (newValue > oldValue) {
                    // increment
                    fraction = (f < 1.0 ? 1 : f);
                }
                else {
                    // decrement
                    fraction = (f > -1.0 ? -1 : f);
                }
                counter = 0;
                targetReached = false;
                response.set(oldValue);
            }
        }
    }

    /**
     * Calculates intermediate values that change max 250 times per second if this
     * method is called each millisecond. The faction is 1/400 of the delta, so
     * it will take about 1.6 seconds to reach the new target.
     */
    virtual void updateIntermediateValue() override
    {
        // change max. 250 times per second
        if (targetReached || (++counter % 4) != 0)
            return;

        T value = response.get<T>();           // save old
        if (  (fraction > 0 && static_cast<int>(targetValue - value) <= fraction)
           || (fraction < 0 && static_cast<int>(targetValue - value) >= fraction)
           || counter > 3000)
        {
            value = targetValue;
            targetReached = true;
        }
        else {
            value += fraction;
        }

        response.set(value);
    }

public:
    GetSet(DeviceFunctions *other, uint8_t getFunc, uint8_t setFunc, T value = 0)
       : GetSetRaw(other, getFunc, setFunc, sizeof(T), buildBytes(value))
       , targetValue(value), fraction(0), targetReached(true), counter(0)
    { }

    GetSet(uint8_t getFunc, uint8_t setFunc, T value = 0)
       : GetSetRaw(getFunc, setFunc, sizeof(T), buildBytes(value))
       , targetValue(value), fraction(0), targetReached(true), counter(0)
    { }

    /** create a deep copy of this device plus all child devices! */
    DeviceFunctions* clone() const override
    {
        DeviceFunctions* result = new GetSet<T>(*this);
        return result;
    }
};



/**
 * This is a virtual device which has an array index in the packet (e.g. for
 * the ServoBrick. The brick has a channel index for each servo, this is handled
 * by the ArrayDevice: define a device without the channel and use this multiple
 * time in the ArrayDevice.
 */
class ArrayDevice : public DeviceFunctions
{
    std::vector<DeviceFunctions*> others;

    ArrayDevice& operator=(const ArrayDevice& rhs) = delete;
    ArrayDevice(const ArrayDevice& rhs) = delete;

public:
    /**
     * Init with one instance which will be cloned.
     */
    ArrayDevice(DeviceFunctions *_other, unsigned numDevices);
    ~ArrayDevice();

    DECLARE_OWN_DEVICE_CALLBACKS
};



/**
 * An enable / disable command which holds one boolean. The commands supports
 * a 'get', but no direct 'set': instead an enable / disable must be used.
 */
class EnableDisableBool : public DeviceFunctions
{
    uint8_t  enableFunction;
    uint8_t  disableFunction;
    uint8_t  getFunction;
    bool     enabled;

    EnableDisableBool& operator=(const EnableDisableBool& rhs) = delete;

public:
    EnableDisableBool(DeviceFunctions *other, uint8_t enableFunction, uint8_t disableFunction, uint8_t getFunction);
    EnableDisableBool(const EnableDisableBool& rhs);
    ~EnableDisableBool();

    /** create a deep copy of this device plus all child devices! */
    DeviceFunctions* clone() const override;

    DECLARE_OWN_DEVICE_CALLBACKS
};



/**
 * The V2 device is for all devices that have a chip on the bricklet and
 * support led status and chip temperature status functions.
 */
class V2Device : public DeviceFunctions
{
    V2Device& operator=(const V2Device& rhs) = delete;
    V2Device(const V2Device& rhs) = delete;

protected:
    VisibleDeviceState *visibleDeviceState;

public:
    V2Device(DeviceFunctions *other, VisibleDeviceState *state);
    ~V2Device();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* DEVICEFUNCTIONS_H_ */
