/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICKLET_VOLTAGE_CURRENT_H
#define BRICKLET_VOLTAGE_CURRENT_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickletVoltageCurrent Voltage/Current Bricklet
 */

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Measures power, DC voltage and DC current up to 720W/36V/20A
 */
typedef Device VoltageCurrent;

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_CURRENT 1

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_VOLTAGE 2

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_POWER 3

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_CONFIGURATION 4

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_CONFIGURATION 5

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_CALIBRATION 6

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_CALIBRATION 7

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_CURRENT_CALLBACK_PERIOD 8

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_CURRENT_CALLBACK_PERIOD 9

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD 10

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_VOLTAGE_CALLBACK_PERIOD 11

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_POWER_CALLBACK_PERIOD 12

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_POWER_CALLBACK_PERIOD 13

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_CURRENT_CALLBACK_THRESHOLD 14

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_CURRENT_CALLBACK_THRESHOLD 15

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_VOLTAGE_CALLBACK_THRESHOLD 16

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_VOLTAGE_CALLBACK_THRESHOLD 17

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_POWER_CALLBACK_THRESHOLD 18

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_POWER_CALLBACK_THRESHOLD 19

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_SET_DEBOUNCE_PERIOD 20

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_DEBOUNCE_PERIOD 21

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Signature: \code void callback(int32_t current, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link voltage_current_set_current_callback_period}. The parameter is the current of the
 * sensor.
 * 
 * The {@link VOLTAGE_CURRENT_CALLBACK_CURRENT} callback is only triggered if the current has changed since
 * the last triggering.
 */
#define VOLTAGE_CURRENT_CALLBACK_CURRENT 22

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Signature: \code void callback(int32_t voltage, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link voltage_current_set_voltage_callback_period}. The parameter is the voltage of
 * the sensor.
 * 
 * The {@link VOLTAGE_CURRENT_CALLBACK_VOLTAGE} callback is only triggered if the voltage has changed since
 * the last triggering.
 */
#define VOLTAGE_CURRENT_CALLBACK_VOLTAGE 23

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Signature: \code void callback(int32_t power, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link voltage_current_set_power_callback_period}. The parameter is the power of the
 * sensor.
 * 
 * The {@link VOLTAGE_CURRENT_CALLBACK_POWER} callback is only triggered if the power has changed since the
 * last triggering.
 */
#define VOLTAGE_CURRENT_CALLBACK_POWER 24

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Signature: \code void callback(int32_t current, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link voltage_current_set_current_callback_threshold} is reached.
 * The parameter is the current of the sensor.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link voltage_current_set_debounce_period}.
 */
#define VOLTAGE_CURRENT_CALLBACK_CURRENT_REACHED 25

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Signature: \code void callback(int32_t voltage, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link voltage_current_set_voltage_callback_threshold} is reached.
 * The parameter is the voltage of the sensor.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link voltage_current_set_debounce_period}.
 */
#define VOLTAGE_CURRENT_CALLBACK_VOLTAGE_REACHED 26

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Signature: \code void callback(int32_t power, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link voltage_current_set_power_callback_threshold} is reached.
 * The parameter is the power of the sensor.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link voltage_current_set_debounce_period}.
 */
#define VOLTAGE_CURRENT_CALLBACK_POWER_REACHED 27


/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_1 0

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_4 1

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_16 2

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_64 3

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_128 4

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_256 5

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_512 6

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_AVERAGING_1024 7

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_THRESHOLD_OPTION_OFF 'x'

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_THRESHOLD_OPTION_OUTSIDE 'o'

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_THRESHOLD_OPTION_INSIDE 'i'

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_THRESHOLD_OPTION_SMALLER '<'

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_THRESHOLD_OPTION_GREATER '>'

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_140US 0

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_204US 1

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_332US 2

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_588US 3

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_1_1MS 4

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_2_116MS 5

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_4_156MS 6

/**
 * \ingroup BrickletVoltageCurrent
 */
#define VOLTAGE_CURRENT_CONVERSION_TIME_8_244MS 7

/**
 * \ingroup BrickletVoltageCurrent
 *
 * This constant is used to identify a Voltage/Current Bricklet.
 *
 * The {@link voltage_current_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define VOLTAGE_CURRENT_DEVICE_IDENTIFIER 227

/**
 * \ingroup BrickletVoltageCurrent
 *
 * This constant represents the display name of a Voltage/Current Bricklet.
 */
#define VOLTAGE_CURRENT_DEVICE_DISPLAY_NAME "Voltage/Current Bricklet"

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Creates the device object \c voltage_current with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void voltage_current_create(VoltageCurrent *voltage_current, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Removes the device object \c voltage_current from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void voltage_current_destroy(VoltageCurrent *voltage_current);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the voltage_current_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int voltage_current_get_response_expected(VoltageCurrent *voltage_current, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Changes the response expected flag of the function specified by the
 * \c function_id parameter. This flag can only be changed for setter
 * (default value: *false*) and callback configuration functions
 * (default value: *true*). For getter functions it is always enabled.
 *
 * Enabling the response expected flag for a setter function allows to detect
 * timeouts and other error conditions calls of this setter as well. The device
 * will then send a response for this purpose. If this flag is disabled for a
 * setter function then no response is sent and errors are silently ignored,
 * because they cannot be detected.
 */
int voltage_current_set_response_expected(VoltageCurrent *voltage_current, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int voltage_current_set_response_expected_all(VoltageCurrent *voltage_current, bool response_expected);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void voltage_current_register_callback(VoltageCurrent *voltage_current, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int voltage_current_get_api_version(VoltageCurrent *voltage_current, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the current.
 * 
 * If you want to get the current periodically, it is recommended to use the
 * {@link VOLTAGE_CURRENT_CALLBACK_CURRENT} callback and set the period with
 * {@link voltage_current_set_current_callback_period}.
 */
int voltage_current_get_current(VoltageCurrent *voltage_current, int32_t *ret_current);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the voltage.
 * 
 * If you want to get the voltage periodically, it is recommended to use the
 * {@link VOLTAGE_CURRENT_CALLBACK_VOLTAGE} callback and set the period with
 * {@link voltage_current_set_voltage_callback_period}.
 */
int voltage_current_get_voltage(VoltageCurrent *voltage_current, int32_t *ret_voltage);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the power.
 * 
 * If you want to get the power periodically, it is recommended to use the
 * {@link VOLTAGE_CURRENT_CALLBACK_POWER} callback and set the period with
 * {@link voltage_current_set_power_callback_period}.
 */
int voltage_current_get_power(VoltageCurrent *voltage_current, int32_t *ret_power);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the configuration of the Voltage/Current Bricklet. It is
 * possible to configure number of averages as well as
 * voltage and current conversion time.
 */
int voltage_current_set_configuration(VoltageCurrent *voltage_current, uint8_t averaging, uint8_t voltage_conversion_time, uint8_t current_conversion_time);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the configuration as set by {@link voltage_current_set_configuration}.
 */
int voltage_current_get_configuration(VoltageCurrent *voltage_current, uint8_t *ret_averaging, uint8_t *ret_voltage_conversion_time, uint8_t *ret_current_conversion_time);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Since the shunt resistor that is used to measure the current is not
 * perfectly precise, it needs to be calibrated by a multiplier and
 * divisor if a very precise reading is needed.
 * 
 * For example, if you are expecting a measurement of 1000mA and you
 * are measuring 1023mA, you can calibrate the Voltage/Current Bricklet
 * by setting the multiplier to 1000 and the divisor to 1023.
 */
int voltage_current_set_calibration(VoltageCurrent *voltage_current, uint16_t gain_multiplier, uint16_t gain_divisor);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the calibration as set by {@link voltage_current_set_calibration}.
 */
int voltage_current_get_calibration(VoltageCurrent *voltage_current, uint16_t *ret_gain_multiplier, uint16_t *ret_gain_divisor);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the period with which the {@link VOLTAGE_CURRENT_CALLBACK_CURRENT} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * The {@link VOLTAGE_CURRENT_CALLBACK_CURRENT} callback is only triggered if the current has changed since
 * the last triggering.
 */
int voltage_current_set_current_callback_period(VoltageCurrent *voltage_current, uint32_t period);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the period as set by {@link voltage_current_set_current_callback_period}.
 */
int voltage_current_get_current_callback_period(VoltageCurrent *voltage_current, uint32_t *ret_period);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the period with which the {@link VOLTAGE_CURRENT_CALLBACK_VOLTAGE} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * The {@link VOLTAGE_CURRENT_CALLBACK_VOLTAGE} callback is only triggered if the voltage has changed since
 * the last triggering.
 */
int voltage_current_set_voltage_callback_period(VoltageCurrent *voltage_current, uint32_t period);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the period as set by {@link voltage_current_set_voltage_callback_period}.
 */
int voltage_current_get_voltage_callback_period(VoltageCurrent *voltage_current, uint32_t *ret_period);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the period with which the {@link VOLTAGE_CURRENT_CALLBACK_POWER} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * The {@link VOLTAGE_CURRENT_CALLBACK_POWER} callback is only triggered if the power has changed since the
 * last triggering.
 */
int voltage_current_set_power_callback_period(VoltageCurrent *voltage_current, uint32_t period);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the period as set by {@link voltage_current_get_power_callback_period}.
 */
int voltage_current_get_power_callback_period(VoltageCurrent *voltage_current, uint32_t *ret_period);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the thresholds for the {@link VOLTAGE_CURRENT_CALLBACK_CURRENT_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the current is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the current is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the current is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the current is greater than the min value (max is ignored)"
 * \endverbatim
 */
int voltage_current_set_current_callback_threshold(VoltageCurrent *voltage_current, char option, int32_t min, int32_t max);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the threshold as set by {@link voltage_current_set_current_callback_threshold}.
 */
int voltage_current_get_current_callback_threshold(VoltageCurrent *voltage_current, char *ret_option, int32_t *ret_min, int32_t *ret_max);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the thresholds for the {@link VOLTAGE_CURRENT_CALLBACK_VOLTAGE_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the voltage is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the voltage is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the voltage is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the voltage is greater than the min value (max is ignored)"
 * \endverbatim
 */
int voltage_current_set_voltage_callback_threshold(VoltageCurrent *voltage_current, char option, int32_t min, int32_t max);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the threshold as set by {@link voltage_current_set_voltage_callback_threshold}.
 */
int voltage_current_get_voltage_callback_threshold(VoltageCurrent *voltage_current, char *ret_option, int32_t *ret_min, int32_t *ret_max);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the thresholds for the {@link VOLTAGE_CURRENT_CALLBACK_POWER_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the power is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the power is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the power is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the power is greater than the min value (max is ignored)"
 * \endverbatim
 */
int voltage_current_set_power_callback_threshold(VoltageCurrent *voltage_current, char option, int32_t min, int32_t max);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the threshold as set by {@link voltage_current_set_power_callback_threshold}.
 */
int voltage_current_get_power_callback_threshold(VoltageCurrent *voltage_current, char *ret_option, int32_t *ret_min, int32_t *ret_max);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Sets the period with which the threshold callbacks
 * 
 * * {@link VOLTAGE_CURRENT_CALLBACK_CURRENT_REACHED},
 * * {@link VOLTAGE_CURRENT_CALLBACK_VOLTAGE_REACHED},
 * * {@link VOLTAGE_CURRENT_CALLBACK_POWER_REACHED}
 * 
 * are triggered, if the thresholds
 * 
 * * {@link voltage_current_set_current_callback_threshold},
 * * {@link voltage_current_set_voltage_callback_threshold},
 * * {@link voltage_current_set_power_callback_threshold}
 * 
 * keep being reached.
 */
int voltage_current_set_debounce_period(VoltageCurrent *voltage_current, uint32_t debounce);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the debounce period as set by {@link voltage_current_set_debounce_period}.
 */
int voltage_current_get_debounce_period(VoltageCurrent *voltage_current, uint32_t *ret_debounce);

/**
 * \ingroup BrickletVoltageCurrent
 *
 * Returns the UID, the UID where the Bricklet is connected to,
 * the position, the hardware and firmware version as well as the
 * device identifier.
 * 
 * The position can be 'a', 'b', 'c', 'd', 'e', 'f', 'g' or 'h' (Bricklet Port).
 * A Bricklet connected to an :ref:`Isolator Bricklet <isolator_bricklet>` is always at
 * position 'z'.
 * 
 * The device identifier numbers can be found :ref:`here <device_identifier>`.
 * |device_identifier_constant|
 */
int voltage_current_get_identity(VoltageCurrent *voltage_current, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
