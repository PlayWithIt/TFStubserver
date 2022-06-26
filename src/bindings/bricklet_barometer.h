/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICKLET_BAROMETER_H
#define BRICKLET_BAROMETER_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickletBarometer Barometer Bricklet
 */

/**
 * \ingroup BrickletBarometer
 *
 * Measures air pressure and altitude changes
 */
typedef Device Barometer;

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_AIR_PRESSURE 1

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_ALTITUDE 2

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_AIR_PRESSURE_CALLBACK_PERIOD 3

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_AIR_PRESSURE_CALLBACK_PERIOD 4

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_ALTITUDE_CALLBACK_PERIOD 5

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_ALTITUDE_CALLBACK_PERIOD 6

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_AIR_PRESSURE_CALLBACK_THRESHOLD 7

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_AIR_PRESSURE_CALLBACK_THRESHOLD 8

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_ALTITUDE_CALLBACK_THRESHOLD 9

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_ALTITUDE_CALLBACK_THRESHOLD 10

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_DEBOUNCE_PERIOD 11

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_DEBOUNCE_PERIOD 12

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_REFERENCE_AIR_PRESSURE 13

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_CHIP_TEMPERATURE 14

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_REFERENCE_AIR_PRESSURE 19

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_AVERAGING 20

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_AVERAGING 21

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_SET_I2C_MODE 22

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_I2C_MODE 23

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickletBarometer
 *
 * Signature: \code void callback(int32_t air_pressure, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link barometer_set_air_pressure_callback_period}. The parameter is the air
 * pressure of the air pressure sensor.
 * 
 * The {@link BAROMETER_CALLBACK_AIR_PRESSURE} callback is only triggered if the air pressure has
 * changed since the last triggering.
 */
#define BAROMETER_CALLBACK_AIR_PRESSURE 15

/**
 * \ingroup BrickletBarometer
 *
 * Signature: \code void callback(int32_t altitude, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link barometer_set_altitude_callback_period}. The parameter is the altitude of
 * the air pressure sensor.
 * 
 * The {@link BAROMETER_CALLBACK_ALTITUDE} callback is only triggered if the altitude has changed since
 * the last triggering.
 */
#define BAROMETER_CALLBACK_ALTITUDE 16

/**
 * \ingroup BrickletBarometer
 *
 * Signature: \code void callback(int32_t air_pressure, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link barometer_set_air_pressure_callback_threshold} is reached.
 * The parameter is the air pressure of the air pressure sensor.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link barometer_set_debounce_period}.
 */
#define BAROMETER_CALLBACK_AIR_PRESSURE_REACHED 17

/**
 * \ingroup BrickletBarometer
 *
 * Signature: \code void callback(int32_t altitude, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link barometer_set_altitude_callback_threshold} is reached.
 * The parameter is the altitude of the air pressure sensor.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link barometer_set_debounce_period}.
 */
#define BAROMETER_CALLBACK_ALTITUDE_REACHED 18


/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_THRESHOLD_OPTION_OFF 'x'

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_THRESHOLD_OPTION_OUTSIDE 'o'

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_THRESHOLD_OPTION_INSIDE 'i'

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_THRESHOLD_OPTION_SMALLER '<'

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_THRESHOLD_OPTION_GREATER '>'

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_I2C_MODE_FAST 0

/**
 * \ingroup BrickletBarometer
 */
#define BAROMETER_I2C_MODE_SLOW 1

/**
 * \ingroup BrickletBarometer
 *
 * This constant is used to identify a Barometer Bricklet.
 *
 * The {@link barometer_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define BAROMETER_DEVICE_IDENTIFIER 221

/**
 * \ingroup BrickletBarometer
 *
 * This constant represents the display name of a Barometer Bricklet.
 */
#define BAROMETER_DEVICE_DISPLAY_NAME "Barometer Bricklet"

/**
 * \ingroup BrickletBarometer
 *
 * Creates the device object \c barometer with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void barometer_create(Barometer *barometer, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickletBarometer
 *
 * Removes the device object \c barometer from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void barometer_destroy(Barometer *barometer);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the barometer_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int barometer_get_response_expected(Barometer *barometer, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickletBarometer
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
int barometer_set_response_expected(Barometer *barometer, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickletBarometer
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int barometer_set_response_expected_all(Barometer *barometer, bool response_expected);

/**
 * \ingroup BrickletBarometer
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void barometer_register_callback(Barometer *barometer, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int barometer_get_api_version(Barometer *barometer, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the air pressure of the air pressure sensor.
 * 
 * If you want to get the air pressure periodically, it is recommended to use the
 * {@link BAROMETER_CALLBACK_AIR_PRESSURE} callback and set the period with
 * {@link barometer_set_air_pressure_callback_period}.
 */
int barometer_get_air_pressure(Barometer *barometer, int32_t *ret_air_pressure);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the relative altitude of the air pressure sensor. The value is
 * calculated based on the difference between the current air pressure
 * and the reference air pressure that can be set with {@link barometer_set_reference_air_pressure}.
 * 
 * If you want to get the altitude periodically, it is recommended to use the
 * {@link BAROMETER_CALLBACK_ALTITUDE} callback and set the period with
 * {@link barometer_set_altitude_callback_period}.
 */
int barometer_get_altitude(Barometer *barometer, int32_t *ret_altitude);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the period with which the {@link BAROMETER_CALLBACK_AIR_PRESSURE} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * The {@link BAROMETER_CALLBACK_AIR_PRESSURE} callback is only triggered if the air pressure has
 * changed since the last triggering.
 */
int barometer_set_air_pressure_callback_period(Barometer *barometer, uint32_t period);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the period as set by {@link barometer_set_air_pressure_callback_period}.
 */
int barometer_get_air_pressure_callback_period(Barometer *barometer, uint32_t *ret_period);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the period with which the {@link BAROMETER_CALLBACK_ALTITUDE} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * The {@link BAROMETER_CALLBACK_ALTITUDE} callback is only triggered if the altitude has changed since
 * the last triggering.
 */
int barometer_set_altitude_callback_period(Barometer *barometer, uint32_t period);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the period as set by {@link barometer_set_altitude_callback_period}.
 */
int barometer_get_altitude_callback_period(Barometer *barometer, uint32_t *ret_period);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the thresholds for the {@link BAROMETER_CALLBACK_AIR_PRESSURE_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the air pressure is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the air pressure is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the air pressure is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the air pressure is greater than the min value (max is ignored)"
 * \endverbatim
 */
int barometer_set_air_pressure_callback_threshold(Barometer *barometer, char option, int32_t min, int32_t max);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the threshold as set by {@link barometer_set_air_pressure_callback_threshold}.
 */
int barometer_get_air_pressure_callback_threshold(Barometer *barometer, char *ret_option, int32_t *ret_min, int32_t *ret_max);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the thresholds for the {@link BAROMETER_CALLBACK_ALTITUDE_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the altitude is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the altitude is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the altitude is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the altitude is greater than the min value (max is ignored)"
 * \endverbatim
 */
int barometer_set_altitude_callback_threshold(Barometer *barometer, char option, int32_t min, int32_t max);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the threshold as set by {@link barometer_set_altitude_callback_threshold}.
 */
int barometer_get_altitude_callback_threshold(Barometer *barometer, char *ret_option, int32_t *ret_min, int32_t *ret_max);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the period with which the threshold callbacks
 * 
 * * {@link BAROMETER_CALLBACK_AIR_PRESSURE_REACHED},
 * * {@link BAROMETER_CALLBACK_ALTITUDE_REACHED}
 * 
 * are triggered, if the thresholds
 * 
 * * {@link barometer_set_air_pressure_callback_threshold},
 * * {@link barometer_set_altitude_callback_threshold}
 * 
 * keep being reached.
 */
int barometer_set_debounce_period(Barometer *barometer, uint32_t debounce);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the debounce period as set by {@link barometer_set_debounce_period}.
 */
int barometer_get_debounce_period(Barometer *barometer, uint32_t *ret_debounce);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the reference air pressure for the altitude calculation.
 * Setting the reference to the current air pressure results in a calculated
 * altitude of 0cm. Passing 0 is a shortcut for passing the current air pressure as
 * reference.
 * 
 * Well known reference values are the Q codes
 * `QNH <https://en.wikipedia.org/wiki/QNH>`__ and
 * `QFE <https://en.wikipedia.org/wiki/Mean_sea_level_pressure#Mean_sea_level_pressure>`__
 * used in aviation.
 */
int barometer_set_reference_air_pressure(Barometer *barometer, int32_t air_pressure);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the temperature of the air pressure sensor.
 * 
 * This temperature is used internally for temperature compensation of the air
 * pressure measurement. It is not as accurate as the temperature measured by the
 * :ref:`temperature_bricklet` or the :ref:`temperature_ir_bricklet`.
 */
int barometer_get_chip_temperature(Barometer *barometer, int16_t *ret_temperature);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the reference air pressure as set by {@link barometer_set_reference_air_pressure}.
 */
int barometer_get_reference_air_pressure(Barometer *barometer, int32_t *ret_air_pressure);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the different averaging parameters. It is possible to set
 * the length of a normal averaging for the temperature and pressure,
 * as well as an additional length of a
 * `moving average <https://en.wikipedia.org/wiki/Moving_average>`__
 * for the pressure. The moving average is calculated from the normal
 * averages.  There is no moving average for the temperature.
 * 
 * Setting the all three parameters to 0 will turn the averaging
 * completely off. If the averaging is off, there is lots of noise
 * on the data, but the data is without delay. Thus we recommend
 * to turn the averaging off if the Barometer Bricklet data is
 * to be used for sensor fusion with other sensors.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Plugin)
 */
int barometer_set_averaging(Barometer *barometer, uint8_t moving_average_pressure, uint8_t average_pressure, uint8_t average_temperature);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the averaging configuration as set by {@link barometer_set_averaging}.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Plugin)
 */
int barometer_get_averaging(Barometer *barometer, uint8_t *ret_moving_average_pressure, uint8_t *ret_average_pressure, uint8_t *ret_average_temperature);

/**
 * \ingroup BrickletBarometer
 *
 * Sets the I2C mode. Possible modes are:
 * 
 * * 0: Fast (400kHz)
 * * 1: Slow (100kHz)
 * 
 * If you have problems with obvious outliers in the
 * Barometer Bricklet measurements, they may be caused by EMI issues.
 * In this case it may be helpful to lower the I2C speed.
 * 
 * It is however not recommended to lower the I2C speed in applications where
 * a high throughput needs to be achieved.
 * 
 * .. versionadded:: 2.0.3$nbsp;(Plugin)
 */
int barometer_set_i2c_mode(Barometer *barometer, uint8_t mode);

/**
 * \ingroup BrickletBarometer
 *
 * Returns the I2C mode as set by {@link barometer_set_i2c_mode}.
 * 
 * .. versionadded:: 2.0.3$nbsp;(Plugin)
 */
int barometer_get_i2c_mode(Barometer *barometer, uint8_t *ret_mode);

/**
 * \ingroup BrickletBarometer
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
int barometer_get_identity(Barometer *barometer, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
