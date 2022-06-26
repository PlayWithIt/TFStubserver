/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICKLET_DISTANCE_US_H
#define BRICKLET_DISTANCE_US_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickletDistanceUS Distance US Bricklet
 */

/**
 * \ingroup BrickletDistanceUS
 *
 * Measures distance between 2cm and 400cm with ultrasound
 */
typedef Device DistanceUS;

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_GET_DISTANCE_VALUE 1

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_SET_DISTANCE_CALLBACK_PERIOD 2

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_GET_DISTANCE_CALLBACK_PERIOD 3

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_SET_DISTANCE_CALLBACK_THRESHOLD 4

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_GET_DISTANCE_CALLBACK_THRESHOLD 5

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_SET_DEBOUNCE_PERIOD 6

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_GET_DEBOUNCE_PERIOD 7

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_SET_MOVING_AVERAGE 10

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_GET_MOVING_AVERAGE 11

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickletDistanceUS
 *
 * Signature: \code void callback(uint16_t distance, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link distance_us_set_distance_callback_period}. The parameter is the distance value
 * of the sensor.
 * 
 * The {@link DISTANCE_US_CALLBACK_DISTANCE} callback is only triggered if the distance value has changed
 * since the last triggering.
 */
#define DISTANCE_US_CALLBACK_DISTANCE 8

/**
 * \ingroup BrickletDistanceUS
 *
 * Signature: \code void callback(uint16_t distance, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link distance_us_set_distance_callback_threshold} is reached.
 * The parameter is the distance value of the sensor.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link distance_us_set_debounce_period}.
 */
#define DISTANCE_US_CALLBACK_DISTANCE_REACHED 9


/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_THRESHOLD_OPTION_OFF 'x'

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_THRESHOLD_OPTION_OUTSIDE 'o'

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_THRESHOLD_OPTION_INSIDE 'i'

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_THRESHOLD_OPTION_SMALLER '<'

/**
 * \ingroup BrickletDistanceUS
 */
#define DISTANCE_US_THRESHOLD_OPTION_GREATER '>'

/**
 * \ingroup BrickletDistanceUS
 *
 * This constant is used to identify a Distance US Bricklet.
 *
 * The {@link distance_us_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define DISTANCE_US_DEVICE_IDENTIFIER 229

/**
 * \ingroup BrickletDistanceUS
 *
 * This constant represents the display name of a Distance US Bricklet.
 */
#define DISTANCE_US_DEVICE_DISPLAY_NAME "Distance US Bricklet"

/**
 * \ingroup BrickletDistanceUS
 *
 * Creates the device object \c distance_us with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void distance_us_create(DistanceUS *distance_us, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickletDistanceUS
 *
 * Removes the device object \c distance_us from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void distance_us_destroy(DistanceUS *distance_us);

/**
 * \ingroup BrickletDistanceUS
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the distance_us_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int distance_us_get_response_expected(DistanceUS *distance_us, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickletDistanceUS
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
int distance_us_set_response_expected(DistanceUS *distance_us, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickletDistanceUS
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int distance_us_set_response_expected_all(DistanceUS *distance_us, bool response_expected);

/**
 * \ingroup BrickletDistanceUS
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void distance_us_register_callback(DistanceUS *distance_us, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickletDistanceUS
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int distance_us_get_api_version(DistanceUS *distance_us, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickletDistanceUS
 *
 * Returns the current distance value measured by the sensor.
 * A small value corresponds to a small distance, a big
 * value corresponds to a big distance. The relation between the measured distance
 * value and the actual distance is affected by the 5V supply voltage (deviations
 * in the supply voltage result in deviations in the distance values) and is
 * non-linear (resolution is bigger at close range).
 * 
 * If you want to get the distance value periodically, it is recommended to
 * use the {@link DISTANCE_US_CALLBACK_DISTANCE} callback and set the period with
 * {@link distance_us_set_distance_callback_period}.
 */
int distance_us_get_distance_value(DistanceUS *distance_us, uint16_t *ret_distance);

/**
 * \ingroup BrickletDistanceUS
 *
 * Sets the period with which the {@link DISTANCE_US_CALLBACK_DISTANCE} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * Der {@link DISTANCE_US_CALLBACK_DISTANCE} callback is only triggered if the distance value has changed
 * since the last triggering.
 */
int distance_us_set_distance_callback_period(DistanceUS *distance_us, uint32_t period);

/**
 * \ingroup BrickletDistanceUS
 *
 * Returns the period as set by {@link distance_us_set_distance_callback_period}.
 */
int distance_us_get_distance_callback_period(DistanceUS *distance_us, uint32_t *ret_period);

/**
 * \ingroup BrickletDistanceUS
 *
 * Sets the thresholds for the {@link DISTANCE_US_CALLBACK_DISTANCE_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the distance value is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the distance value is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the distance value is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the distance value is greater than the min value (max is ignored)"
 * \endverbatim
 */
int distance_us_set_distance_callback_threshold(DistanceUS *distance_us, char option, uint16_t min, uint16_t max);

/**
 * \ingroup BrickletDistanceUS
 *
 * Returns the threshold as set by {@link distance_us_set_distance_callback_threshold}.
 */
int distance_us_get_distance_callback_threshold(DistanceUS *distance_us, char *ret_option, uint16_t *ret_min, uint16_t *ret_max);

/**
 * \ingroup BrickletDistanceUS
 *
 * Sets the period with which the threshold callbacks
 * 
 * * {@link DISTANCE_US_CALLBACK_DISTANCE_REACHED},
 * 
 * are triggered, if the thresholds
 * 
 * * {@link distance_us_set_distance_callback_threshold},
 * 
 * keep being reached.
 */
int distance_us_set_debounce_period(DistanceUS *distance_us, uint32_t debounce);

/**
 * \ingroup BrickletDistanceUS
 *
 * Returns the debounce period as set by {@link distance_us_set_debounce_period}.
 */
int distance_us_get_debounce_period(DistanceUS *distance_us, uint32_t *ret_debounce);

/**
 * \ingroup BrickletDistanceUS
 *
 * Sets the length of a `moving averaging <https://en.wikipedia.org/wiki/Moving_average>`__
 * for the distance value.
 * 
 * Setting the length to 0 will turn the averaging completely off. With less
 * averaging, there is more noise on the data.
 */
int distance_us_set_moving_average(DistanceUS *distance_us, uint8_t average);

/**
 * \ingroup BrickletDistanceUS
 *
 * Returns the length moving average as set by {@link distance_us_set_moving_average}.
 */
int distance_us_get_moving_average(DistanceUS *distance_us, uint8_t *ret_average);

/**
 * \ingroup BrickletDistanceUS
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
int distance_us_get_identity(DistanceUS *distance_us, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
