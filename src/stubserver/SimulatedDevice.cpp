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
#include <bricklet_accelerometer.h>
#include <bricklet_analog_in.h>
#include <bricklet_analog_in_v2.h>
#include <bricklet_analog_out.h>
#include <bricklet_analog_out_v2.h>
#include <bricklet_ambient_light.h>
#include <bricklet_ambient_light_v2.h>
#include <bricklet_co2.h>
#include <bricklet_color.h>
#include <bricklet_distance_ir.h>
#include <bricklet_distance_us.h>
#include <bricklet_dust_detector.h>
#include <bricklet_humidity.h>
#include <bricklet_industrial_digital_out_4.h>
#include <bricklet_industrial_digital_in_4.h>
#include <bricklet_industrial_quad_relay.h>
#include <bricklet_industrial_dual_analog_in.h>
#include <bricklet_io16.h>
#include <bricklet_io4.h>
#include <bricklet_joystick.h>
#include <bricklet_line.h>
#include <bricklet_load_cell.h>
#include <bricklet_moisture.h>
#include <bricklet_humidity.h>
#include <bricklet_ptc.h>
#include <bricklet_rgb_led_button.h>
#include <bricklet_sound_intensity.h>
#include <bricklet_temperature.h>
#include <bricklet_temperature_ir.h>
#include <bricklet_uv_light.h>
#include <bricklet_voltage_current.h>
#include <device_table.h>

#include <utils/Log.h>
#include <utils/utils.h>

#include "BrickStack.h"
#include "DeviceBarometer.h"
#include "DeviceBrick.h"
#include "DeviceDualButton.h"
#include "DeviceInOut.h"
#include "DeviceHallEffect.h"
#include "DeviceLCD.h"
#include "DeviceLedButton.h"
#include "DeviceLedStrip.h"
#include "DeviceMotionDetector.h"
#include "DeviceMultiSensor.h"
#include "DeviceOled.h"
#include "DeviceOutdoorWeather.h"
#include "DevicePiezoSpeaker.h"
#include "DeviceRelay.h"
#include "DeviceRTC.h"
#include "DeviceSensor.h"
#include "DeviceTilt.h"
#include "DeviceTouchPad.h"

#include "SimulatedDevice.h"

using utils::Log;
using utils::Properties;
using utils::Exception;


namespace stubserver {


//=======================================================================================

static VisualizationClient dummyVisualisationInstance;


//=======================================================================================


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

    switch (deviceTypeId) {
    case MASTER_DEVICE_IDENTIFIER:
        functions = new DeviceBrick(deviceTypeId, MASTER_FUNCTION_GET_STACK_VOLTAGE, MASTER_FUNCTION_GET_STACK_CURRENT,
                                    MASTER_FUNCTION_SET_STACK_VOLTAGE_CALLBACK_PERIOD, MASTER_FUNCTION_SET_STACK_CURRENT_CALLBACK_PERIOD,
                                    MASTER_CALLBACK_STACK_VOLTAGE, MASTER_CALLBACK_STACK_CURRENT);
        isBrick = true;
        label = "Voltage mV";
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
        functions = new DeviceBrick(deviceTypeId, functions, SERVO_FUNCTION_GET_STACK_INPUT_VOLTAGE, SERVO_FUNCTION_GET_OVERALL_CURRENT);
        doNothing = new DoNothing(functions, SERVO_FUNCTION_GET_EXTERNAL_INPUT_VOLTAGE, 2, buildBytes(12340));
        doNothing = new DoNothing(doNothing, SERVO_FUNCTION_GET_SERVO_CURRENT, 2, buildBytes(241) );
        doNothing = new DoNothing(doNothing, SERVO_FUNCTION_DISABLE_POSITION_REACHED_CALLBACK);
        doNothing->addFunc(SERVO_FUNCTION_DISABLE_VELOCITY_REACHED_CALLBACK);

        getSet = new GetSet<uint16_t>(doNothing, SERVO_FUNCTION_GET_OUTPUT_VOLTAGE, SERVO_FUNCTION_SET_OUTPUT_VOLTAGE, 5800);
        getSet = new GetSet<uint16_t>(getSet, SERVO_FUNCTION_GET_MINIMUM_VOLTAGE, SERVO_FUNCTION_SET_MINIMUM_VOLTAGE, 5200);
        getSet = new GetSet<uint16_t>(getSet, SERVO_FUNCTION_GET_PERIOD, SERVO_FUNCTION_SET_PERIOD, 19500);
        functions = getSet;
        isBrick = true;
        label = "Voltage mV";
        break;

    case DC_DEVICE_IDENTIFIER:
        functions = new DeviceBrick(deviceTypeId, NULL, DC_FUNCTION_GET_STACK_INPUT_VOLTAGE, DC_FUNCTION_GET_CURRENT_CONSUMPTION);
        functions = new EnableDisableBool(functions, DC_FUNCTION_ENABLE, DC_FUNCTION_DISABLE, DC_FUNCTION_IS_ENABLED);
        doNothing = new DoNothing(functions, DC_FUNCTION_FULL_BRAKE);
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
        label = "Voltage mV";
        break;

    //----------- Bricklets -------------------------------------------------------------------------------------------
/*
    // ACCELEROMETER needs 3 value providers with 16Bit each
    case ACCELEROMETER_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(ACCELEROMETER_FUNCTION_GET_ACCELERATION,
                                  ACCELEROMETER_FUNCTION_SET_ACCELERATION_CALLBACK_PERIOD,
                                  ACCELEROMETER_CALLBACK_ACCELERATION);
        sensor->setRangeCallback(AMBIENT_LIGHT_FUNCTION_SET_ILLUMINANCE_CALLBACK_THRESHOLD,
                AMBIENT_LIGHT_FUNCTION_GET_ILLUMINANCE_CALLBACK_THRESHOLD,
                AMBIENT_LIGHT_FUNCTION_SET_DEBOUNCE_PERIOD,
                AMBIENT_LIGHT_FUNCTION_GET_DEBOUNCE_PERIOD,
                AMBIENT_LIGHT_CALLBACK_ILLUMINANCE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=2500,step=5,interval=300")));
        sensor->setMinMax(0, 9000);
        functions = sensor;
        label = "X/Y/Z";
        break;
*/
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
        sensor->setMinMax(0, 9000);
        functions = sensor;
        label = "Lux/10";
        break;

    case AMBIENT_LIGHT_V2_DEVICE_IDENTIFIER:
        functions = new GetSet<uint16_t>(AMBIENT_LIGHT_V2_FUNCTION_GET_CONFIGURATION, AMBIENT_LIGHT_V2_FUNCTION_SET_CONFIGURATION, 0);
        sensor = new DeviceSensor(
                AMBIENT_LIGHT_V2_FUNCTION_GET_ILLUMINANCE,
                AMBIENT_LIGHT_V2_FUNCTION_SET_ILLUMINANCE_CALLBACK_PERIOD,
                AMBIENT_LIGHT_V2_CALLBACK_ILLUMINANCE);
        sensor->setRangeCallback(AMBIENT_LIGHT_V2_FUNCTION_SET_ILLUMINANCE_CALLBACK_THRESHOLD,
                AMBIENT_LIGHT_V2_FUNCTION_GET_ILLUMINANCE_CALLBACK_THRESHOLD,
                AMBIENT_LIGHT_V2_FUNCTION_SET_DEBOUNCE_PERIOD,
                AMBIENT_LIGHT_V2_FUNCTION_GET_DEBOUNCE_PERIOD,
                AMBIENT_LIGHT_V2_CALLBACK_ILLUMINANCE_REACHED);
        sensor->setValueSize(4);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=3200000,step=800,interval=200")));
        sensor->setMinMax(0, 6400000);
        sensor->setOther(functions);
        functions = sensor;
        label = "Lux/100";
        break;

    case ANALOG_IN_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(
                ANALOG_IN_FUNCTION_GET_VOLTAGE,
                ANALOG_IN_FUNCTION_GET_ANALOG_VALUE,
                ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD,
                ANALOG_IN_FUNCTION_SET_ANALOG_VALUE_CALLBACK_PERIOD,
                ANALOG_IN_CALLBACK_VOLTAGE,
                ANALOG_IN_CALLBACK_ANALOG_VALUE);
        sensor->setRangeCallback(ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_THRESHOLD,
                ANALOG_IN_FUNCTION_GET_VOLTAGE_CALLBACK_THRESHOLD,
                ANALOG_IN_FUNCTION_SET_DEBOUNCE_PERIOD,
                ANALOG_IN_FUNCTION_GET_DEBOUNCE_PERIOD,
                ANALOG_IN_CALLBACK_VOLTAGE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=3000,step=5,interval=300")));
        functions = sensor;
        label = "mV";
        break;

    case ANALOG_IN_V2_DEVICE_IDENTIFIER:
        functions = new GetSet<uint8_t>(ANALOG_IN_V2_FUNCTION_GET_MOVING_AVERAGE, ANALOG_IN_V2_FUNCTION_SET_MOVING_AVERAGE, 50);
        sensor = new DeviceSensor(
                ANALOG_IN_V2_FUNCTION_GET_VOLTAGE,
                ANALOG_IN_V2_FUNCTION_GET_ANALOG_VALUE,
                ANALOG_IN_V2_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD,
                ANALOG_IN_V2_FUNCTION_SET_ANALOG_VALUE_CALLBACK_PERIOD,
                ANALOG_IN_V2_CALLBACK_VOLTAGE,
                ANALOG_IN_V2_CALLBACK_ANALOG_VALUE);
        sensor->setRangeCallback(ANALOG_IN_V2_FUNCTION_SET_VOLTAGE_CALLBACK_THRESHOLD,
                ANALOG_IN_V2_FUNCTION_GET_VOLTAGE_CALLBACK_THRESHOLD,
                ANALOG_IN_V2_FUNCTION_SET_DEBOUNCE_PERIOD,
                ANALOG_IN_V2_FUNCTION_GET_DEBOUNCE_PERIOD,
                ANALOG_IN_V2_CALLBACK_VOLTAGE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=3000,step=5,interval=300")));
        sensor->setOther(functions);
        functions = sensor;
        label = "mV";
        break;

    case ANALOG_OUT_DEVICE_IDENTIFIER:
        functions = new GetSet<uint8_t>(ANALOG_OUT_FUNCTION_GET_MODE, ANALOG_OUT_FUNCTION_SET_MODE, 1);
        functions = new GetSet<uint16_t>(functions, ANALOG_OUT_FUNCTION_GET_VOLTAGE, ANALOG_OUT_FUNCTION_SET_VOLTAGE, 0);
        label = "mV";
        break;

    case BAROMETER_DEVICE_IDENTIFIER:
        functions = new DeviceBarometer(createValueProvider(getProperty("valueProvider")));
        label = "mbar * 1000";
        break;

    case CO2_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(CO2_FUNCTION_GET_CO2_CONCENTRATION,
                                  CO2_FUNCTION_SET_CO2_CONCENTRATION_CALLBACK_PERIOD,
                                  CO2_CALLBACK_CO2_CONCENTRATION);
        sensor->setRangeCallback(CO2_FUNCTION_SET_CO2_CONCENTRATION_CALLBACK_THRESHOLD,
                                 CO2_FUNCTION_GET_CO2_CONCENTRATION_CALLBACK_THRESHOLD,
                                 CO2_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 CO2_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 CO2_CALLBACK_CO2_CONCENTRATION_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=1200,max=2500,step=5,interval=300")));
        sensor->setMinMax(0, 10000);
        functions = sensor;
        label = "ppm";
        break;

    case COLOR_DEVICE_IDENTIFIER:
        functions = new GetSet<uint16_t>(COLOR_FUNCTION_GET_CONFIG, COLOR_FUNCTION_SET_CONFIG, 0x0303);
        sensor = new DeviceSensor(COLOR_FUNCTION_GET_ILLUMINANCE, COLOR_FUNCTION_SET_ILLUMINANCE_CALLBACK_PERIOD, COLOR_CALLBACK_ILLUMINANCE);
        sensor->setValueSize(4);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=100000,step=500,interval=250")));
        sensor->setMinMax(0, 100000);
        sensor->setOther(functions);
        functions = sensor;

        // TODO: this will not work ... there are 4 independent values, 16bit each
        // -> need 4 valueProviders or other mechanism, also for the callback.
        sensor = new DeviceSensor(COLOR_FUNCTION_GET_COLOR, COLOR_FUNCTION_SET_COLOR_CALLBACK_PERIOD, COLOR_CALLBACK_COLOR);
        sensor->setRangeCallback(COLOR_FUNCTION_SET_COLOR_CALLBACK_THRESHOLD,
                COLOR_FUNCTION_GET_COLOR_CALLBACK_THRESHOLD,
                COLOR_FUNCTION_SET_DEBOUNCE_PERIOD,
                COLOR_FUNCTION_GET_DEBOUNCE_PERIOD,
                COLOR_CALLBACK_COLOR_REACHED);

        sensor->enableStatusLed(COLOR_FUNCTION_IS_LIGHT_ON, COLOR_FUNCTION_LIGHT_ON, COLOR_FUNCTION_LIGHT_OFF);
        sensor->setValueSize(8);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=3200000,step=800,interval=200")));
        sensor->setMinMax(0, 6400000);
        sensor->setOther(functions);
        functions = sensor;
        label = "Lux/100";
        break;

    case DISTANCE_IR_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(
                DISTANCE_IR_FUNCTION_GET_DISTANCE,
                DISTANCE_IR_FUNCTION_GET_ANALOG_VALUE,
                DISTANCE_IR_FUNCTION_SET_DISTANCE_CALLBACK_PERIOD,
                DISTANCE_IR_FUNCTION_SET_ANALOG_VALUE_CALLBACK_PERIOD,
                DISTANCE_IR_CALLBACK_DISTANCE,
                DISTANCE_IR_CALLBACK_ANALOG_VALUE);
        sensor->setRangeCallback(DISTANCE_IR_FUNCTION_SET_DISTANCE_CALLBACK_THRESHOLD,
                DISTANCE_IR_FUNCTION_GET_DISTANCE_CALLBACK_THRESHOLD,
                DISTANCE_IR_FUNCTION_SET_DEBOUNCE_PERIOD,
                DISTANCE_IR_FUNCTION_GET_DEBOUNCE_PERIOD,
                DISTANCE_IR_CALLBACK_DISTANCE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=100,max=800,step=5,interval=300")));
        sensor->setMinMax(100, 800);
        functions = sensor;
        label = "Distance mm";  // default
        break;

    case DISTANCE_US_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(
                DISTANCE_US_FUNCTION_GET_DISTANCE_VALUE,
                DISTANCE_US_FUNCTION_SET_DISTANCE_CALLBACK_PERIOD,
                DISTANCE_US_CALLBACK_DISTANCE);
        sensor->setRangeCallback(DISTANCE_US_FUNCTION_SET_DISTANCE_CALLBACK_THRESHOLD,
                DISTANCE_US_FUNCTION_GET_DISTANCE_CALLBACK_THRESHOLD,
                DISTANCE_US_FUNCTION_SET_DEBOUNCE_PERIOD,
                DISTANCE_US_FUNCTION_GET_DEBOUNCE_PERIOD,
                DISTANCE_US_CALLBACK_DISTANCE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=4095,step=20,interval=100")));
        sensor->setMinMax(0, 4095);
        functions = sensor;
        break;

    case DUAL_BUTTON_DEVICE_IDENTIFIER:
        functions = new DeviceDualButton(createValueProvider(getProperty("valueProvider")));
        break;

    case RGB_LED_BUTTON_DEVICE_IDENTIFIER:
        functions = new DeviceLedButton(createValueProvider(getProperty("valueProvider")));
        break;

    case DUAL_RELAY_DEVICE_IDENTIFIER:
        functions = new DeviceDualRelay();
        break;

    case DUST_DETECTOR_DEVICE_IDENTIFIER:
        functions = new GetSet<uint8_t>(DUST_DETECTOR_FUNCTION_GET_MOVING_AVERAGE, DUST_DETECTOR_FUNCTION_SET_MOVING_AVERAGE, 100);
        sensor = new DeviceSensor(
                DUST_DETECTOR_FUNCTION_GET_DUST_DENSITY,
                DUST_DETECTOR_FUNCTION_SET_DUST_DENSITY_CALLBACK_PERIOD,
                DUST_DETECTOR_CALLBACK_DUST_DENSITY);
        sensor->setRangeCallback(DUST_DETECTOR_FUNCTION_SET_DUST_DENSITY_CALLBACK_THRESHOLD,
                DUST_DETECTOR_FUNCTION_GET_DUST_DENSITY_CALLBACK_THRESHOLD,
                DUST_DETECTOR_FUNCTION_SET_DEBOUNCE_PERIOD,
                DUST_DETECTOR_FUNCTION_GET_DEBOUNCE_PERIOD,
                DUST_DETECTOR_CALLBACK_DUST_DENSITY_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=500,step=1,interval=100")));
        sensor->setMinMax(0, 500);
        sensor->setOther(functions);
        functions = sensor;
        label ="µg/m³";
        break;

    case HALL_EFFECT_DEVICE_IDENTIFIER:
        functions = new DeviceHallEffect(createValueProvider(getProperty("valueProvider")));
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
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=100,max=650,step=5,interval=500")));
        sensor->setMinMax(0, 1000);
        functions = sensor;
        label = "% rHum * 10";
        break;

    case INDUSTRIAL_DIGITAL_IN_4_DEVICE_IDENTIFIER:
        functions = new DeviceDigitalIn(createValueProvider(getProperty("valueProvider")));
        functions = new DoNothing(functions, INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_AVAILABLE_FOR_GROUP, 1);
        functions = new DoNothing(functions, INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_GROUP, 4, buildBytes('n', 'n', 'n', 'n'));
        break;

    case INDUSTRIAL_DIGITAL_OUT_4_DEVICE_IDENTIFIER:
        functions = new DeviceDigitalOut4();
        break;

    case INDUSTRIAL_DUAL_ANALOG_IN_DEVICE_IDENTIFIER:
        functions = new DeviceDualAnalogIn(createValueProvider(getProperty("valueProvider1")), createValueProvider(getProperty("valueProvider2")));
        break;

    case INDUSTRIAL_QUAD_RELAY_DEVICE_IDENTIFIER:
        functions = new DeviceQuadRelay();
        break;

    case IO16_DEVICE_IDENTIFIER:
        functions = new DeviceInOut16(createValueProvider(getProperty("valueProviderA")), createValueProvider(getProperty("valueProviderB")));
        break;

    case IO4_DEVICE_IDENTIFIER:
        functions = new DeviceInOut(createValueProvider(getProperty("valueProvider")));
        break;
/*
    case JOYSTICK_DEVICE_IDENTIFIER:
        functions = new GetSet<uint16_t>(COLOR_FUNCTION_GET_CONFIG, COLOR_FUNCTION_SET_CONFIG, 0x0303);
        sensor = new DeviceSensor(JOYSTICK_FUNCTION_GET_POSITION, COLOR_FUNCTION_SET_ILLUMINANCE_CALLBACK_PERIOD, COLOR_CALLBACK_ILLUMINANCE);
        sensor->setValueSize(4);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=100000,step=500,interval=250")));
        sensor->setMinMax(0, 100000);
        sensor->setOther(functions);
        functions = sensor;

        // TODO: this will not work ... there are 4 independent values, 16bit each
        // -> need 4 valueProviders or other mechanism, also for the callback.
        sensor = new DeviceSensor(COLOR_FUNCTION_GET_COLOR, COLOR_FUNCTION_SET_COLOR_CALLBACK_PERIOD, COLOR_CALLBACK_COLOR);
        sensor->setRangeCallback(COLOR_FUNCTION_SET_COLOR_CALLBACK_THRESHOLD,
                COLOR_FUNCTION_GET_COLOR_CALLBACK_THRESHOLD,
                COLOR_FUNCTION_SET_DEBOUNCE_PERIOD,
                COLOR_FUNCTION_GET_DEBOUNCE_PERIOD,
                COLOR_CALLBACK_COLOR_REACHED);

        sensor->enableStatusLed(COLOR_FUNCTION_IS_LIGHT_ON, COLOR_FUNCTION_LIGHT_ON, COLOR_FUNCTION_LIGHT_OFF);
        sensor->setValueSize(8);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=3200000,step=800,interval=200")));
        sensor->setMinMax(0, 6400000);
        sensor->setOther(functions);
        functions = sensor;
        label = "X / Y";
        break;
*/
    case LINEAR_POTI_DEVICE_IDENTIFIER:
        functions = new DevicePotentiometer(true);
        break;

    case LINE_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(
                LINE_FUNCTION_GET_REFLECTIVITY,
                LINE_FUNCTION_SET_REFLECTIVITY_CALLBACK_PERIOD,
                LINE_CALLBACK_REFLECTIVITY);
        sensor->setRangeCallback(LINE_FUNCTION_SET_REFLECTIVITY_CALLBACK_THRESHOLD,
                LINE_FUNCTION_GET_REFLECTIVITY_CALLBACK_THRESHOLD,
                LINE_FUNCTION_SET_DEBOUNCE_PERIOD,
                LINE_FUNCTION_GET_DEBOUNCE_PERIOD,
                LINE_CALLBACK_REFLECTIVITY_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=4000,step=5,interval=300")));
        functions = sensor;
        label = "Reflectivity";  // default
        break;

    case LCD_20X4_DEVICE_IDENTIFIER:
        functions = new DeviceLCD(20, 4);
        break;

    case LED_STRIP_DEVICE_IDENTIFIER:
        functions = new DeviceLedStrip(getProperty("numLeds"));
        break;

    case LOAD_CELL_DEVICE_IDENTIFIER:
        functions = new DoNothing(LOAD_CELL_FUNCTION_CALIBRATE);
        functions = new GetSet<uint8_t>(functions, LOAD_CELL_FUNCTION_GET_MOVING_AVERAGE, LOAD_CELL_FUNCTION_SET_MOVING_AVERAGE, 4);
        functions = new GetSet<uint16_t>(functions, LOAD_CELL_FUNCTION_GET_CONFIGURATION, LOAD_CELL_FUNCTION_SET_CONFIGURATION, 0);
        sensor = new DeviceSensor(
                LOAD_CELL_FUNCTION_GET_WEIGHT,
                LOAD_CELL_FUNCTION_SET_WEIGHT_CALLBACK_PERIOD,
                LOAD_CELL_CALLBACK_WEIGHT);
        sensor->setRangeCallback(LOAD_CELL_FUNCTION_SET_WEIGHT_CALLBACK_THRESHOLD,
                LOAD_CELL_FUNCTION_GET_WEIGHT_CALLBACK_THRESHOLD,
                LOAD_CELL_FUNCTION_SET_DEBOUNCE_PERIOD,
                LOAD_CELL_FUNCTION_GET_DEBOUNCE_PERIOD,
                LOAD_CELL_CALLBACK_WEIGHT_REACHED);
        sensor->enableStatusLed(LOAD_CELL_FUNCTION_IS_LED_ON, LOAD_CELL_FUNCTION_LED_ON, LOAD_CELL_FUNCTION_LED_OFF);
        sensor->setCalibrateZeroFunc(LOAD_CELL_FUNCTION_TARE);
        sensor->setValueSize(4);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=2000,step=500,interval=5000")));
        sensor->setMinMax(0, 5000);
        sensor->setOther(functions);
        functions = sensor;
        label = "Gramm";
        break;

    case MOISTURE_DEVICE_IDENTIFIER:
        // has no analog value
        functions = new GetSet<uint8_t>(MOISTURE_FUNCTION_GET_MOVING_AVERAGE, MOISTURE_FUNCTION_GET_MOVING_AVERAGE, 100);
        sensor = new DeviceSensor(MOISTURE_FUNCTION_GET_MOISTURE_VALUE,
                                  MOISTURE_FUNCTION_SET_MOISTURE_CALLBACK_PERIOD, MOISTURE_CALLBACK_MOISTURE);
        sensor->setRangeCallback(MOISTURE_FUNCTION_SET_MOISTURE_CALLBACK_THRESHOLD,
                                 MOISTURE_FUNCTION_GET_MOISTURE_CALLBACK_THRESHOLD,
                                 MOISTURE_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 MOISTURE_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 MOISTURE_CALLBACK_MOISTURE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=2800,step=5,interval=300")));
        sensor->setOther(functions);
        functions = sensor;
        break;

    case MOTION_DETECTOR_DEVICE_IDENTIFIER:
        functions = new DeviceMotionDetector(createValueProvider(getProperty("valueProvider")));
        break;

    case MULTI_TOUCH_DEVICE_IDENTIFIER:
        functions = new DeviceTouchPad(12, createValueProvider(getProperty("valueProvider")));
        break;

    case PIEZO_SPEAKER_DEVICE_IDENTIFIER:
        functions = new DevicePiezoSpeaker();
        break;

    case LCD_128X64_DEVICE_IDENTIFIER:
        functions = new DeviceOled(true, 128, 64);
        break;

    case OLED_128X64_DEVICE_IDENTIFIER:
        functions = new DeviceOled(128, 64);
        break;

    case OLED_64X48_DEVICE_IDENTIFIER:
        functions = new DeviceOled(64, 48);
        break;

    case OUTDOOR_WEATHER_DEVICE_IDENTIFIER: {
        DeviceOutdoorWeather *w = new DeviceOutdoorWeather(createValueProvider(getProperty("valueProvider1")), getIntProperty("sensorId1"));
        for (int i = 2; i < DeviceOutdoorWeather::MAX_SENSORS; ++i) {
            char name[32];
            sprintf(name, "valueProvider%d", i);
            const char *vp = getProperty(name, "");

            if (vp[0] == 0)  // not set
                break;

            sprintf(name, "sensorId%d", i);
            int id = getIntProperty(name);

            w->addSensor(createValueProvider(vp), id);
        }
        functions = w;
    }
    break;

    case PTC_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(PTC_FUNCTION_GET_TEMPERATURE, PTC_FUNCTION_SET_TEMPERATURE_CALLBACK_PERIOD, PTC_CALLBACK_TEMPERATURE);
        sensor->setRangeCallback(PTC_FUNCTION_SET_TEMPERATURE_CALLBACK_THRESHOLD,
                                 PTC_FUNCTION_GET_TEMPERATURE_CALLBACK_THRESHOLD,
                                 PTC_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 PTC_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 PTC_CALLBACK_TEMPERATURE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=-500,max=2800,step=5,interval=300")));
        sensor->setMinMax(-4000, 12500);
        functions = sensor;
        break;

    case REAL_TIME_CLOCK_DEVICE_IDENTIFIER:
        functions = new DeviceRTC();
        break;

    case REMOTE_SWITCH_DEVICE_IDENTIFIER:
        functions = new DeviceRemoteRelay();
        break;

    case ROTARY_POTI_DEVICE_IDENTIFIER:
        functions = new DevicePotentiometer(false);
        break;

    case SOLID_STATE_RELAY_DEVICE_IDENTIFIER:
        functions = new DeviceSolidStateRelay();
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
        sensor->setMinMax(-4000, 12500);
        functions = sensor;
        label = "°C * 100";
        break;

    case TEMPERATURE_IR_DEVICE_IDENTIFIER:
        // has no analog value
        functions = new GetSet<uint16_t>(functions, TEMPERATURE_IR_FUNCTION_GET_EMISSIVITY, TEMPERATURE_IR_FUNCTION_SET_EMISSIVITY, 65535);
        sensor = new DeviceSensor(TEMPERATURE_IR_FUNCTION_GET_AMBIENT_TEMPERATURE,
                                  TEMPERATURE_IR_FUNCTION_SET_AMBIENT_TEMPERATURE_CALLBACK_PERIOD, TEMPERATURE_IR_CALLBACK_AMBIENT_TEMPERATURE);
        sensor->setRangeCallback(TEMPERATURE_IR_FUNCTION_SET_AMBIENT_TEMPERATURE_CALLBACK_THRESHOLD,
                                 TEMPERATURE_IR_FUNCTION_GET_AMBIENT_TEMPERATURE_CALLBACK_THRESHOLD,
                                 TEMPERATURE_IR_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 TEMPERATURE_IR_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 TEMPERATURE_IR_CALLBACK_AMBIENT_TEMPERATURE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProviderA", "linear min=-500,max=2800,step=5,interval=300")));
        sensor->setMinMax(-4000, 12500);
        sensor->setOther(functions);
        sensor->setInternalSensorNo(1);
        functions = sensor;

        sensor = new DeviceSensor(TEMPERATURE_IR_FUNCTION_GET_OBJECT_TEMPERATURE,
                                  TEMPERATURE_IR_FUNCTION_SET_OBJECT_TEMPERATURE_CALLBACK_PERIOD, TEMPERATURE_IR_CALLBACK_OBJECT_TEMPERATURE);
        sensor->setRangeCallback(TEMPERATURE_IR_FUNCTION_SET_OBJECT_TEMPERATURE_CALLBACK_THRESHOLD,
                                 TEMPERATURE_IR_FUNCTION_GET_OBJECT_TEMPERATURE_CALLBACK_THRESHOLD,
                                 TEMPERATURE_IR_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 TEMPERATURE_IR_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 TEMPERATURE_IR_CALLBACK_OBJECT_TEMPERATURE_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProviderO", "linear min=-500,max=4500,step=1,interval=5")));
        sensor->setMinMax(-4000, 12500);
        sensor->setOther(functions);
        functions = sensor;
        label = "Temperature IR\n°C * 100";
        break;

    case TILT_DEVICE_IDENTIFIER:
        functions = new DeviceTilt(createValueProvider(getProperty("valueProvider")));
        break;

    case UV_LIGHT_DEVICE_IDENTIFIER:
        sensor = new DeviceSensor(UV_LIGHT_FUNCTION_GET_UV_LIGHT,
                                  UV_LIGHT_FUNCTION_SET_UV_LIGHT_CALLBACK_PERIOD,
                                  UV_LIGHT_CALLBACK_UV_LIGHT);
        sensor->setRangeCallback(UV_LIGHT_FUNCTION_SET_UV_LIGHT_CALLBACK_THRESHOLD,
                                 UV_LIGHT_FUNCTION_GET_UV_LIGHT_CALLBACK_THRESHOLD,
                                 UV_LIGHT_FUNCTION_SET_DEBOUNCE_PERIOD,
                                 UV_LIGHT_FUNCTION_GET_DEBOUNCE_PERIOD,
                                 UV_LIGHT_CALLBACK_UV_LIGHT_REACHED);
        sensor->setValueProvider(createValueProvider(getProperty("valueProvider", "linear min=0,max=300,step=2,interval=100")));
        sensor->setMinMax(0, 328);
        sensor->setValueSize(4);
        functions = sensor;
        label = "mW/cm²";
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
            label = "Voltage mV";
        }
        break;
    }

    return functions;
}


SimulatedDevice::SimulatedDevice(BrickStack *_brickStack, const char *_uidStr, unsigned int _typeId)
    : brickStack(_brickStack)
    , properties(NULL)
    , functions(NULL)
    , visualizationClient(&dummyVisualisationInstance)
    , deviceMutex()
    , uidStr(_uidStr)
    , uid(utils::base58Decode(_uidStr))
    , deviceTypeId(_typeId)
    , position(0)
    , isBrick(false)
{
    setupFunctions();
}

/**
 * Release resources: used in case of exception in constructor.
 */
void SimulatedDevice::cleanup()
{
    delete properties;
    delete functions;
    properties = NULL;
    functions  = NULL;
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
    , visualizationClient(&dummyVisualisationInstance)
    , deviceMutex()
    , uidStr(_uidStr)
    , uid(utils::base58Decode(_uidStr))
    , deviceTypeId(0)
    , position(0)
    , isBrick(false)
{
    char msg[200];

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
    for (int i = 0; gAllDeviceIdentifiers[i].deviceIdentifier > 0 && deviceTypeId == 0; ++i)
    {
        if (strcmp(gAllDeviceIdentifiers[i].name, str) == 0)
            deviceTypeId = gAllDeviceIdentifiers[i].deviceIdentifier;
    }
    if (deviceTypeId == 0) {
        cleanup();
        sprintf(msg, "Unkown device type '%s' for uid %s", str, _uidStr);
        throw Exception(msg);
    }

    label = getProperty("label");
    deviceTypeName = str;

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
    if (p >= 'a' && p <= 'd') {
        position = str[0];
    }
    else if (p >= '0' && p <= '9') {
        position = str[0];
        brickStack->addBrick(p, uidStr);
    }
    else {
        cleanup();
        sprintf(msg, "Invalid position char '%c' for uid %s", p, _uidStr);
        throw Exception(msg);
    }

    // Main brick which is not connected to another brick has parent id '0'
    str = getProperty("connectedTo", 0);
    if (str[0] == 0)
        connectedUidStr = "0";
    else
        connectedUidStr = str;

    // connect to a parent device ?
    if (! isMainBrick() != 0)
    {
        SimulatedDevice *parent = brickStack->getDevice(connectedUidStr);
        if (parent == NULL) {
            Log() << "ERROR: parent device with uid " << str
                  << " not found! (I am " << getUidStr() << "), check order of config file";
        }
        else
            parent->connect(this);
    }

    // if setupFunctions throws an exception, one device might not be released if the
    // 'new' was not assigned to 'functions'
    try {
        setupFunctions();

        if (false == isBrick && (position < 'a' || position > 'd')) {
            sprintf(msg, "ERROR: invalid position char '%c' (%d) for BRICKLET %s (must be a..d)", position, position, uidStr.c_str());
            throw utils::Exception(msg);
        }
        if (true == isBrick && (position < '0' || position > '9')) {
            sprintf(msg, "ERROR: invalid position char '%c' (%d) for BRICK %s (must be 0..9)", position, position, uidStr.c_str());
            throw utils::Exception(msg);
        }
    }
    catch (const std::exception &e) {
        cleanup();
        throw;
    }
}

/** clear objects */
SimulatedDevice::~SimulatedDevice()
{
    visualizationClient->notify(VisibleDeviceState(VisibleDeviceState::DISCONNECT));
    cleanup();
}


/**
 * Set one VisualizationClient which must exist as long as it is registered here!
 */
void SimulatedDevice::setVisualizationClient(VisualizationClient &client) const {
    const_cast<SimulatedDevice*>(this)->visualizationClient = &client;

    VisibleDeviceState *state = dynamic_cast<VisibleDeviceState*>(functions);
    if (state) {
        state->notify(client, VisibleDeviceState::CONNECTED);
    }
    else {
        Log() << "Device " << getDeviceTypeName() << ' ' << getUidStr() << " has no VisibleDeviceState base class, cannot use notify(CONNECTED)";
    }
}

/**
 * Clear a VisualizationClient set before using {@link setVisualizationClient(VisualizationClient)}.
 */
void SimulatedDevice::clearVisualizationClient() const {
    const_cast<SimulatedDevice*>(this)->visualizationClient = &dummyVisualisationInstance;
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
    if (res == NULL || (int) strlen(res) < minLength)
    {
        char msg[128];
        if (!res)
            sprintf(msg, "Property '%s' for uid %s does not exist, check properties",
                    key.c_str(), uidStr.c_str());
        else
            sprintf(msg, "Property '%s' for uid %s must have length %u, but has %d",
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

int SimulatedDevice::getIntProperty(const std::string &key)
{
	const char *v = getProperty(key, 1);
	return atoi(v);
}

/**
 * Returns true if this bricklet is in the stack where the given brickId is a
 * parent of this bricklet.
 */
bool SimulatedDevice::isChildOf(const std::string &brickUid) const
{
    SimulatedDevice *brick = brickStack->getDevice(brickUid);
    if (!brick)
        return false;

    if (this == brick)
        return true;

    for (auto it : brick->children)
    {
        if (it == this)
            return true;
        if (it->isBrick && isChildOf(it->uidStr))
            return true;
    }
    return false;
}

/**
 * Check if any of the callbacks should trigger a response.
 */
void SimulatedDevice::checkCallbacks()
{
    MutexLock lock(deviceMutex);
    if (functions)
        functions->checkCallbacks(brickStack->getRelativeTimeMs(), uid, brickStack, *visualizationClient);
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
    if (functions && functions->consumeCommand(time, p, *visualizationClient))
        return true;

    uint8_t func = p.header.function_id;
    if (MASTER_FUNCTION_RESET == func)
    {
        // enumerate all bricklets
        if (isBrick) {
            Log() << "Got a reset event for brick " << getUidStr();
        }
        else {
            Log() << "Got a reset event for bricklet " << getUidStr();
        }
        brickStack->initEnumerate(IPCON_ENUMERATION_TYPE_AVAILABLE);
        return true;
    }
    if (FUNCTION_WRITE_BRICKLET_UID == func)
    {
        // Called for a brick and byte 9 is the port number of the requested bricklet
        p.header.length = sizeof(p.header);
        char    port = p.group[0];
        unsigned uid = p.channelRequest.value1;
        std::string uidStr = utils::base58Encode(uid);

        for (SimulatedDevice* child : children) {
            if (child->position == port) {
                child->uid = uid;
                child->uidStr = uidStr;

                Log() << "Update bricklet UID on port " << port << " to " << uidStr;
                return true;
            }
        }

        Log(Log::ERROR) << "ERROR: bricklet on position " << port << " of brick " << uidStr << " not found";
        p.setErrorCode(IOPacket::ErrorCode::INVALID_PARAMETER);
        return true;
    }

    if (!responseExpected) {
        Log() << "Consume not implemented function " << (int) p.header.function_id
              << " for device " << getUidStr() << " due to responseExpected=false";
        return true;
    }

    //--------------------------------------------------------------------
    //---- functions that are not used so often - WITH response data
    //--------------------------------------------------------------------
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
        p.identity.device_identifier   = deviceTypeId;
        return true;
    }
    if (MASTER_FUNCTION_GET_CHIP_TEMPERATURE == func)
    {
        // CHIP_TEMPERATURE is available in all bricks and some bricklets and has the same function-id 242
        p.header.length = sizeof(p.header) + sizeof(int16_t);
        p.int16Value = 320 + (time % 10);
        return true;
    }
    if (MASTER_FUNCTION_GET_PROTOCOL1_BRICKLET_NAME == func)
    {
        // get protocol name
        // simulate protocol 2 for all devices/ports: return zero-bytes
        p.header.length = sizeof(p.header) + sizeof(p.protocol1Response);
        bzero(&p.protocol1Response, sizeof(p.protocol1Response));
        return true;
    }
    if (FUNCTION_READ_BRICKLET_UID == func && isBrick)
    {
        // Called for a brick and byte 9 is the port number of the requested bricklet
        p.header.length = sizeof(p.header) + sizeof(int32_t);
        char port = p.group[0];

        // clear return value AFTER port was read from packet
        p.int32Value = 0;

        for (SimulatedDevice* child : children) {
            if (child->position == port) {
                p.int32Value = child->uid;
                return true;
            }
        }

        Log(Log::ERROR) << "ERROR: bricklet on position " << port << " of brick " << uidStr << " not found";
        p.setErrorCode(IOPacket::ErrorCode::INVALID_PARAMETER);
        return true;
    }

    return false;
}

} /* namespace stubserver */
