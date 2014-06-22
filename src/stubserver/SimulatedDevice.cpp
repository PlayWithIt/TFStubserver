/*
 * SimulatedDevice.cpp
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

#include <memory>
#include <stdexcept>

#include <brick_master.h>
#include <brick_servo.h>
#include <brick_dc.h>
#include <bricklet_ambient_light.h>
#include <bricklet_industrial_digital_out_4.h>
#include <bricklet_industrial_digital_in_4.h>
#include <bricklet_industrial_quad_relay.h>
#include <bricklet_io16.h>
#include <bricklet_io4.h>
#include <bricklet_humidity.h>
#include <bricklet_moisture.h>
#include <bricklet_humidity.h>
#include <bricklet_sound_intensity.h>
#include <bricklet_temperature.h>
#include <bricklet_voltage_current.h>
#include <device_table.h>

#include <utils/Log.h>
#include <utils/utils.h>

#include "BrickStack.h"
#include "DeviceBarometer.h"
#include "DeviceButtons.h"
#include "DeviceInOut.h"
#include "DeviceLCD.h"
#include "DevicePiezoSpeaker.h"
#include "DeviceRelay.h"
#include "DeviceSensor.h"
#include "DeviceVoltageCurrent.h"

#include "SimulatedDevice.h"

using utils::Log;
using utils::Properties;
using utils::Exception;

namespace stubserver {

/**
 * Dynamically create a value provider.
 */
utils::ValueProvider* SimulatedDevice::createValueProvider(const char *options)
{
    if (!options || *options == 0)
        throw Exception(std::string("Value provider for device with UID=") + uidStr + " is mandatory!");

    utils::ValueProvider* result;
    result = utils::ValueProvider::buildFrom(options);

    if (!result)
        throw Exception(std::string("Invalid value provider options, device UID=") + uidStr + ", options=" + options);
    return result;
}

/**
 * Builds a byte array with the given number of bytes.
 */
static uint8_t* buildBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    static uint8_t result[4];
    result[0] = a;
    result[1] = b;
    result[2] = c;
    result[3] = d;
    return result;
}

/**
 * Builds a byte array with the given number of bytes.
 */
static uint32_t buildBytes(int16_t a, uint16_t b)
{
    union {
        uint32_t result;
        int16_t  minMax[2];
    };
    minMax[0] = a;
    minMax[1] = b;
    return result;
}

/**
 *
 */
DeviceFunctions *SimulatedDevice::setupFunctions()
{
    DeviceSensor *sensor;
    DoNothing *doNothing;
    GetSetRaw *getSet;

    switch (typeId) {
    case MASTER_DEVICE_IDENTIFIER:
        functions = new DeviceVoltageCurrent(MASTER_FUNCTION_GET_STACK_VOLTAGE, MASTER_FUNCTION_GET_STACK_CURRENT,
                MASTER_FUNCTION_SET_STACK_VOLTAGE_CALLBACK_PERIOD, MASTER_FUNCTION_SET_STACK_CURRENT_CALLBACK_PERIOD,
                MASTER_CALLBACK_STACK_VOLTAGE, MASTER_CALLBACK_STACK_CURRENT);
        doNothing = new DoNothing(functions, MASTER_FUNCTION_REFRESH_WIFI_STATUS);
        doNothing->addFunc(MASTER_FUNCTION_RESET);
        doNothing = new DoNothing(doNothing, MASTER_FUNCTION_GET_WIFI_STATUS, 40);
        doNothing = new DoNothing(doNothing, MASTER_FUNCTION_GET_WIFI_BUFFER_INFO, 8);
        doNothing = new DoNothing(doNothing, MASTER_FUNCTION_IS_CHIBI_PRESENT, 1);
        doNothing->addFunc(MASTER_FUNCTION_IS_WIFI_PRESENT)
                  .addFunc(MASTER_FUNCTION_IS_ETHERNET_PRESENT)
                  .addFunc(MASTER_FUNCTION_IS_RS485_PRESENT);
        functions = doNothing;
        isBrick = true;
        break;

    case SERVO_DEVICE_IDENTIFIER:
        functions = new EnableDisableBool(NULL, SERVO_FUNCTION_ENABLE, SERVO_FUNCTION_DISABLE, SERVO_FUNCTION_IS_ENABLED);
        getSet = new GetSet<uint16_t>(functions, SERVO_FUNCTION_GET_ACCELERATION, SERVO_FUNCTION_SET_ACCELERATION, 65535);
        getSet = new GetSet<int16_t>(getSet, SERVO_FUNCTION_GET_POSITION, SERVO_FUNCTION_SET_POSITION);
        getSet->setFunctionIntermediate(SERVO_FUNCTION_GET_CURRENT_POSITION);
        // min / max degree: -6000 .. 6000
        getSet = new GetSet<uint32_t>(getSet, SERVO_FUNCTION_GET_DEGREE, SERVO_FUNCTION_SET_DEGREE, buildBytes(-6000, 6000));
        getSet = new GetSet<uint16_t>(getSet, SERVO_FUNCTION_GET_VELOCITY, SERVO_FUNCTION_SET_VELOCITY, 65535);
        getSet->setFunctionIntermediate(SERVO_FUNCTION_GET_CURRENT_VELOCITY);
        // min / max pulse: 1000 .. 2000
        getSet = new GetSet<uint32_t>(getSet, SERVO_FUNCTION_GET_PULSE_WIDTH, SERVO_FUNCTION_SET_PULSE_WIDTH, buildBytes(1000, 2000));
        functions = new ArrayDevice(getSet, 7);

        functions = new EnableDisableBool(functions, SERVO_FUNCTION_ENABLE_POSITION_REACHED_CALLBACK, SERVO_FUNCTION_DISABLE_POSITION_REACHED_CALLBACK, SERVO_FUNCTION_IS_POSITION_REACHED_CALLBACK_ENABLED);
        functions = new EnableDisableBool(functions, SERVO_FUNCTION_ENABLE_VELOCITY_REACHED_CALLBACK, SERVO_FUNCTION_DISABLE_VELOCITY_REACHED_CALLBACK, SERVO_FUNCTION_IS_VELOCITY_REACHED_CALLBACK_ENABLED);
        functions = new DeviceVoltageCurrent(functions, SERVO_FUNCTION_GET_STACK_INPUT_VOLTAGE, SERVO_FUNCTION_GET_OVERALL_CURRENT);
        doNothing = new DoNothing(functions, SERVO_FUNCTION_GET_EXTERNAL_INPUT_VOLTAGE, 2, buildBytes(12340));
        doNothing = new DoNothing(doNothing, SERVO_FUNCTION_GET_SERVO_CURRENT, 2, buildBytes(241) );
        doNothing = new DoNothing(doNothing, SERVO_FUNCTION_RESET);
        doNothing->addFunc(SERVO_FUNCTION_DISABLE_POSITION_REACHED_CALLBACK)
                  .addFunc(SERVO_FUNCTION_DISABLE_VELOCITY_REACHED_CALLBACK);

        getSet = new GetSet<uint16_t>(doNothing, SERVO_FUNCTION_GET_OUTPUT_VOLTAGE, SERVO_FUNCTION_SET_OUTPUT_VOLTAGE, 5800);
        getSet = new GetSet<uint16_t>(getSet, SERVO_FUNCTION_GET_MINIMUM_VOLTAGE, SERVO_FUNCTION_SET_MINIMUM_VOLTAGE, 5200);
        getSet = new GetSet<uint16_t>(getSet, SERVO_FUNCTION_GET_PERIOD, SERVO_FUNCTION_SET_PERIOD, 19500);
        functions = getSet;
        isBrick = true;
        break;

    case DC_DEVICE_IDENTIFIER:
        functions = new DeviceVoltageCurrent(NULL, DC_FUNCTION_GET_STACK_INPUT_VOLTAGE, DC_FUNCTION_GET_CURRENT_CONSUMPTION);
        functions = new EnableDisableBool(functions, DC_FUNCTION_ENABLE, DC_FUNCTION_DISABLE, DC_FUNCTION_IS_ENABLED);
        doNothing = new DoNothing(functions, DC_FUNCTION_FULL_BRAKE);
        doNothing->addFunc(DC_FUNCTION_RESET);
        doNothing = new DoNothing(doNothing, DC_FUNCTION_GET_EXTERNAL_INPUT_VOLTAGE, 2, buildBytes(12340));
        getSet = new GetSet<uint8_t>(doNothing, DC_FUNCTION_GET_DRIVE_MODE, DC_FUNCTION_SET_DRIVE_MODE);
        getSet = new GetSet<uint16_t>(getSet, DC_FUNCTION_GET_PWM_FREQUENCY, DC_FUNCTION_SET_PWM_FREQUENCY, 19500);
        getSet = new GetSet<uint16_t>(getSet, DC_FUNCTION_GET_MINIMUM_VOLTAGE, DC_FUNCTION_SET_MINIMUM_VOLTAGE, 9500);
        getSet = new GetSet<uint16_t>(getSet, DC_FUNCTION_GET_ACCELERATION, DC_FUNCTION_SET_ACCELERATION, 65535);
        getSet = new GetSet<int16_t>(getSet, DC_FUNCTION_GET_VELOCITY, DC_FUNCTION_SET_VELOCITY);
        getSet->setFunctionIntermediate(DC_FUNCTION_GET_CURRENT_VELOCITY);
        getSet->enableCallback(DC_FUNCTION_GET_CURRENT_VELOCITY_PERIOD, DC_FUNCTION_SET_CURRENT_VELOCITY_PERIOD, DC_CALLBACK_CURRENT_VELOCITY);
        functions = getSet;
        isBrick = true;
        break;

    //----------- Bricklets -------------------------------------------------------------------------------------------

    case BAROMETER_DEVICE_IDENTIFIER:
        functions = new DeviceBarometer();
        break;

    case DUAL_RELAY_DEVICE_IDENTIFIER:
        functions = new DeviceDualRelay();
        break;

    case INDUSTRIAL_DIGITAL_IN_4_DEVICE_IDENTIFIER:
        functions = new DeviceDigitalIn(createValueProvider(getProperty("valueProvider")));
        functions = new DoNothing(functions, INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_AVAILABLE_FOR_GROUP, 1);
        functions = new DoNothing(functions, INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_GROUP, 4, buildBytes('n', 'n', 'n', 'n'));
        break;

    case INDUSTRIAL_DIGITAL_OUT_4_DEVICE_IDENTIFIER:
        functions = new DeviceDigitalOut4();
        functions = new DoNothing(functions, INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_AVAILABLE_FOR_GROUP, 1);
        functions = new DoNothing(functions, INDUSTRIAL_DIGITAL_OUT_4_FUNCTION_GET_GROUP, 4, buildBytes('n', 'n', 'n', 'n'));
        break;

    case INDUSTRIAL_QUAD_RELAY_DEVICE_IDENTIFIER:
        functions = new DeviceQuadRelay();
        functions = new DoNothing(functions, INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_AVAILABLE_FOR_GROUP, 1);
        functions = new DoNothing(functions, INDUSTRIAL_QUAD_RELAY_FUNCTION_GET_GROUP, 4, buildBytes('n', 'n', 'n', 'n'));
        break;

    case IO16_DEVICE_IDENTIFIER:
        functions = new DeviceInOut16(createValueProvider(getProperty("valueProviderA")), createValueProvider(getProperty("valueProviderB")));
        break;

    case IO4_DEVICE_IDENTIFIER:
        functions = new DeviceInOut(createValueProvider(getProperty("valueProvider")));
        break;

    case ROTARY_POTI_DEVICE_IDENTIFIER:
        functions = new DevicePotentiometer(false);
        break;

    case LINEAR_POTI_DEVICE_IDENTIFIER:
        functions = new DevicePotentiometer(true);
        break;

    case AMBIENT_LIGHT_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(
                AMBIENT_LIGHT_FUNCTION_GET_ILLUMINANCE,
                AMBIENT_LIGHT_FUNCTION_GET_ANALOG_VALUE,
                AMBIENT_LIGHT_FUNCTION_SET_ILLUMINANCE_CALLBACK_PERIOD,
                AMBIENT_LIGHT_FUNCTION_SET_ANALOG_VALUE_CALLBACK_PERIOD,
                AMBIENT_LIGHT_CALLBACK_ILLUMINANCE,
                AMBIENT_LIGHT_CALLBACK_ANALOG_VALUE);
        sensor->setRangeCallback(AMBIENT_LIGHT_FUNCTION_SET_ILLUMINANCE_CALLBACK_THRESHOLD,
                AMBIENT_LIGHT_FUNCTION_GET_ILLUMINANCE_CALLBACK_THRESHOLD,
                AMBIENT_LIGHT_FUNCTION_SET_DEBOUNCE_PERIOD,
                AMBIENT_LIGHT_FUNCTION_GET_DEBOUNCE_PERIOD,
                AMBIENT_LIGHT_CALLBACK_ILLUMINANCE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=2500,step=5,interval=300")));
        functions = sensor;
        break;

    case HUMIDITY_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(
                HUMIDITY_FUNCTION_GET_HUMIDITY,
                HUMIDITY_FUNCTION_GET_ANALOG_VALUE,
                HUMIDITY_FUNCTION_SET_HUMIDITY_CALLBACK_PERIOD,
                HUMIDITY_FUNCTION_SET_ANALOG_VALUE_CALLBACK_PERIOD,
                HUMIDITY_CALLBACK_HUMIDITY,
                HUMIDITY_CALLBACK_ANALOG_VALUE);
        sensor->setRangeCallback(HUMIDITY_FUNCTION_SET_HUMIDITY_CALLBACK_THRESHOLD,
                HUMIDITY_FUNCTION_GET_HUMIDITY_CALLBACK_THRESHOLD,
                HUMIDITY_FUNCTION_SET_DEBOUNCE_PERIOD,
                HUMIDITY_FUNCTION_GET_DEBOUNCE_PERIOD,
                HUMIDITY_CALLBACK_HUMIDITY_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=100,max=700,step=5,interval=300")));
        functions = sensor;
        break;

    case MOISTURE_DEVICE_IDENTIFIER:
        // has no analog value
        sensor = new DeviceSensor(MOISTURE_FUNCTION_GET_MOISTURE_VALUE,
                                  MOISTURE_FUNCTION_SET_MOISTURE_CALLBACK_PERIOD, MOISTURE_CALLBACK_MOISTURE);
        sensor->setRangeCallback(MOISTURE_FUNCTION_SET_MOISTURE_CALLBACK_THRESHOLD,
                                 MOISTURE_FUNCTION_GET_MOISTURE_CALLBACK_THRESHOLD,
                                 MOISTURE_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 MOISTURE_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 MOISTURE_CALLBACK_MOISTURE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=-500,max=2800,step=5,interval=300")));
        functions = new GetSet<uint8_t>(sensor, MOISTURE_FUNCTION_GET_MOVING_AVERAGE, MOISTURE_FUNCTION_GET_MOVING_AVERAGE, 100);
        break;

    case TEMPERATURE_DEVICE_IDENTIFIER:
        // has no analog value
        sensor = new DeviceSensor(TEMPERATURE_FUNCTION_GET_TEMPERATURE,
                                  TEMPERATURE_FUNCTION_SET_TEMPERATURE_CALLBACK_PERIOD, TEMPERATURE_CALLBACK_TEMPERATURE);
        sensor->setRangeCallback(TEMPERATURE_FUNCTION_SET_TEMPERATURE_CALLBACK_THRESHOLD,
                                 TEMPERATURE_FUNCTION_GET_TEMPERATURE_CALLBACK_THRESHOLD,
                                 TEMPERATURE_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 TEMPERATURE_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 TEMPERATURE_CALLBACK_TEMPERATURE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=-500,max=2800,step=5,interval=300")));
        functions = sensor;
        break;

    case SOUND_INTENSITY_DEVICE_IDENTIFIER:
        // has no analog value
        sensor = new DeviceSensor(SOUND_INTENSITY_FUNCTION_GET_INTENSITY,
                                  SOUND_INTENSITY_FUNCTION_SET_INTENSITY_CALLBACK_PERIOD, SOUND_INTENSITY_CALLBACK_INTENSITY);
        sensor->setRangeCallback(SOUND_INTENSITY_FUNCTION_SET_INTENSITY_CALLBACK_THRESHOLD,
                                 SOUND_INTENSITY_FUNCTION_GET_INTENSITY_CALLBACK_THRESHOLD,
                                 SOUND_INTENSITY_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 SOUND_INTENSITY_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 SOUND_INTENSITY_CALLBACK_INTENSITY_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=2800,step=5,interval=300")));
        functions = sensor;
        break;

    case LCD_20X4_DEVICE_IDENTIFIER:
        functions = new DeviceLCD(20, 4);
        break;

    case MULTI_TOUCH_DEVICE_IDENTIFIER:
        functions = new DeviceTouchPad(12, createValueProvider(getProperty("valueProvider")));
        break;

    case PIEZO_SPEAKER_DEVICE_IDENTIFIER:
        functions = new DevicePiezoSpeaker();
        break;

    case REMOTE_SWITCH_DEVICE_IDENTIFIER:
        functions = new DeviceRemoteRelay();
        break;

    case VOLTAGE_CURRENT_DEVICE_IDENTIFIER:
        {
            DeviceVoltageCurrent *vc = new DeviceVoltageCurrent();
            const char *vp = getProperty("valueProviderV");
            if (*vp)
                vc->setVoltageValueProvider(createValueProvider(vp));
            vp = getProperty("valueProviderC");
            if (*vp)
                vc->setCurrentValueProvider(createValueProvider(vp));
            functions  = vc;
        }
        break;
    }

    return functions;
}


SimulatedDevice::SimulatedDevice(BrickStack *_brickStack, const char *_uidStr, unsigned int _typeId)
    : brickStack(_brickStack)
    , properties(NULL)
    , functions(NULL)
    , deviceMutex()
    , uid(utils::base58Decode(_uidStr))
    , typeId(_typeId)
    , position(0)
    , isBrick(false)
    , visibleStateChange(false)
    , uidStr(_uidStr)
{
    setupFunctions();
}

/**
 * Init the device with a given UID. If the properties contains '<i>uid</i>.properties'
 * another properties file is read which defines the properties for this device.
 * Otherwise all properties must be defined in the input properties, prefixed with the UID.
 */
SimulatedDevice::SimulatedDevice(BrickStack *_brickStack, const char *_uidStr, const Properties &props)
    : brickStack(_brickStack)
    , properties(NULL)
    , functions(NULL)
    , deviceMutex()
    , uid(utils::base58Decode(_uidStr))
    , typeId(0)
    , position(0)
    , isBrick(false)
    , visibleStateChange(false)
    , uidStr(_uidStr)
{
    char msg[128];

    std::string key(uidStr + ".properties");
    const char *str = props.get(key);
    if (str && *str)
    {
        // open another properties file with common settings for that device
        properties = new Properties(str);
    }
    else {
        properties = new Properties();
    }

    properties->put(props);
    str = getProperty("type", 2);

    // find type code (int-value)
    for (int i = 0; gAllDeviceIdentifiers[i].deviceIdentifier > 0 && typeId == 0; ++i)
    {
        if (strcmp(gAllDeviceIdentifiers[i].name, str) == 0)
            typeId = gAllDeviceIdentifiers[i].deviceIdentifier;
    }
    if (typeId == 0) {
        sprintf(msg, "Unkown device type '%s' for uid %s", str, _uidStr);
        throw Exception(msg);
    }

    str = getProperty("firmwareVersion", 3);
    firmwareVersion[0] = str[0];
    firmwareVersion[1] = str[1];
    firmwareVersion[2] = str[2];
    str = getProperty("hardwareVersion", 3);
    hardwareVersion[0] = str[0];
    hardwareVersion[1] = str[1];
    hardwareVersion[2] = str[2];
    str = getProperty("position", 1);

    char p = str[0];
    if ((p >= 'a' && p <= 'd') || (p >= '0' && p <= '7'))
        position = str[0];
    else {
        sprintf(msg, "Invalid position char '%c' for uid %s", p, _uidStr);
        throw Exception(msg);
    }

    // Main brick which is not connected to another brick has parent id '0'
    str = getProperty("connectedTo", 0);
    if (str[0] == 0)
        connectedUidStr = "0";
    else
        connectedUidStr = str;

    // connect to a parent device
    if (connectedUidStr.compare("0") != 0) {
        SimulatedDevice *parent = brickStack->getDevice(connectedUidStr);
        if (parent == NULL) {
            Log::getStream() << "ERROR: parent device with uid " << str
                    << " not found! (I am " << getUidStr() << "), check order of config file\n";
        }
        else
            parent->connect(this);
    }

    setupFunctions();
}

/** clear objects */
SimulatedDevice::~SimulatedDevice()
{
    delete properties;
    delete functions;
}


/**
 * Read a property from the properties: first with UID prefix and if
 * this doesn't exist without prefix. Check for min expected length.
 *
 * @return a non-NULL value: if the key doesn't exist a zero length string is returned
 */
const char *SimulatedDevice::getProperty(const std::string &key, int minLength)
{
    const char *res = properties->get(uidStr + "." + key);
    if (res == NULL || *res == 0)
        res = properties->get(key);
    if (res == NULL) {
        if (minLength <= 0)
            return "";
    }
    if (res == NULL || (int)strlen(res) < minLength)
    {
        char msg[128];
        if (!res)
            sprintf(msg, "Property '%s' for uid %s does not exist, check properties",
                    key.c_str(), uidStr.c_str());
        else
            sprintf(msg, "Property '%s' for uid %s must have length %d, but has %d",
                    key.c_str(), uidStr.c_str(), minLength, res ? (int) strlen(res) : 0);
        throw Exception(msg);
    }
    return res;
}

const char *SimulatedDevice::getProperty(const std::string &key, const char *defaultValue)
{
    const char *res = getProperty(key);
    if (res == NULL || *res == 0)
        res = defaultValue;
    return res;
}

/**
 * Check if any of the callbacks should trigger a response.
 */
void SimulatedDevice::checkCallbacks()
{
    MutexLock lock(deviceMutex);
    if (functions)
        functions->checkCallbacks(brickStack->getRelativeTimeMs(), uid, brickStack, visibleStateChange);
}

/**
 * Connect a child with this device: the child is located at a
 * specific port (A-D) or position in the stack (0..7) if the child
 * is another brick.
 */
void SimulatedDevice::connect(SimulatedDevice* child)
{
    char msg[128];
    bool positions[128];
    unsigned index;

    bzero(positions, sizeof(positions));

    for (auto it : children)
    {
        if (it->uid == child->uid) {
            sprintf(msg, "Device with uid %s already connected!", it->getUidStr().c_str());
            throw std::logic_error(msg);
        }
        index = it->position;
        positions[index] = true;
    }
    index = child->position;
    if (index > 'd') {
        sprintf(msg, "Device with uid %s uses position '%c' which is an invalid value!",
                child->getUidStr().c_str(), index);
        throw std::logic_error(msg);
    }
    if (positions[index]) {
        sprintf(msg, "Device with uid %s uses position '%c' which is already connected!",
                child->getUidStr().c_str(), index);
        throw std::logic_error(msg);
    }
    children.push_back(child);
}

/**
 * Consumes a packet with a given function code. This might modify
 * the packet contents, if the result is 'true': in this case, the
 * packet should be returned to the client where the packet came
 * from. If the result is false, the packet was consumed, but there
 * is no response to the client.
 * <P>
 * In case of error, an exception is thrown.
 */
bool SimulatedDevice::consumePacket(IOPacket &p, bool responseExpected)
{
    MutexLock lock(deviceMutex);

    // if there is a function associated and consumed -> all OK
    // if not: if responseExpected=false, the packet is just consumed with a warning
    uint64_t time = brickStack->getRelativeTimeMs();
    if (functions && functions->consumeCommand(time, p, visibleStateChange))
        return true;

    if (!responseExpected) {
        char msg[256];
        sprintf(msg, "Consume function %u for device %s due to responseExpected=false", p.header.function_id, this->getUidStr().c_str());
        Log::log(msg);
        return true;
    }

    //--------------------------------------------------------------------
    //---- functions that are not used so often
    //--------------------------------------------------------------------
    uint8_t func = p.header.function_id;
    if (MASTER_FUNCTION_GET_IDENTITY == func)
    {
        // GET_IDENTITY function
        p.header.length = sizeof(p.header) + sizeof(p.identity);
        strcpy(p.identity.uid, uidStr.c_str());
        strcpy(p.identity.connected_uid, connectedUidStr.c_str());
        p.identity.position            = position;
        p.identity.hardware_version[0] = hardwareVersion[0] - '0';
        p.identity.hardware_version[1] = hardwareVersion[1] - '0';
        p.identity.hardware_version[2] = hardwareVersion[2] - '0';
        p.identity.firmware_version[0] = firmwareVersion[0] - '0';
        p.identity.firmware_version[1] = firmwareVersion[1] - '0';
        p.identity.firmware_version[2] = firmwareVersion[2] - '0';
        p.identity.device_identifier   = typeId;
        return true;
    }
    if (MASTER_FUNCTION_GET_CHIP_TEMPERATURE == func && isBrick)
    {
        // CHIP_TEMPERATURE is available in all bricks and has the same function-id in all bricks
        p.header.length = sizeof(p.header) + sizeof(int16_t);
        p.int16Value = 320 + (time % 10);
        return true;
    }
    if (MASTER_FUNCTION_GET_PROTOCOL1_BRICKLET_NAME == func)
    {
        // get protocol1 name
        // simulate protocol 2 for all devices/ports: return zero-bytes
        p.header.length = sizeof(p.header) + sizeof(p.protocol1Response);
        bzero(&p.protocol1Response, sizeof(p.protocol1Response));
        return true;
    }

    return false;
}

} /* namespace stubserver */