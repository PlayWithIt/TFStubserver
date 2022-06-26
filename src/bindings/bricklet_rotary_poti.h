/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICKLET_ROTARY_POTI_H
#define BRICKLET_ROTARY_POTI_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickletRotaryPoti Rotary Poti Bricklet
 */

/**
 * \ingroup BrickletRotaryPoti
 *
 * 300° rotary potentiometer
 */
typedef Device RotaryPoti;

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_POSITION 1

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_ANALOG_VALUE 2

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_SET_POSITION_CALLBACK_PERIOD 3

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_POSITION_CALLBACK_PERIOD 4

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_SET_ANALOG_VALUE_CALLBACK_PERIOD 5

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_ANALOG_VALUE_CALLBACK_PERIOD 6

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_SET_POSITION_CALLBACK_THRESHOLD 7

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_POSITION_CALLBACK_THRESHOLD 8

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_SET_ANALOG_VALUE_CALLBACK_THRESHOLD 9

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_ANALOG_VALUE_CALLBACK_THRESHOLD 10

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_SET_DEBOUNCE_PERIOD 11

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_DEBOUNCE_PERIOD 12

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickletRotaryPoti
 *
 * Signature: \code void callback(int16_t position, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link rotary_poti_set_position_callback_period}. The parameter is the position of
 * the rotary potentiometer.
 * 
 * The {@link ROTARY_POTI_CALLBACK_POSITION} callback is only triggered if the position has changed since
 * the last triggering.
 */
#define ROTARY_POTI_CALLBACK_POSITION 13

/**
 * \ingroup BrickletRotaryPoti
 *
 * Signature: \code void callback(uint16_t value, void *user_data) \endcode
 * 
 * This callback is triggered periodically with the period that is set by
 * {@link rotary_poti_set_analog_value_callback_period}. The parameter is the
 * analog value of the rotary potentiometer.
 * 
 * The {@link ROTARY_POTI_CALLBACK_ANALOG_VALUE} callback is only triggered if the position has changed
 * since the last triggering.
 */
#define ROTARY_POTI_CALLBACK_ANALOG_VALUE 14

/**
 * \ingroup BrickletRotaryPoti
 *
 * Signature: \code void callback(int16_t position, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link rotary_poti_set_position_callback_threshold} is reached.
 * The parameter is the position of the rotary potentiometer.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link rotary_poti_set_debounce_period}.
 */
#define ROTARY_POTI_CALLBACK_POSITION_REACHED 15

/**
 * \ingroup BrickletRotaryPoti
 *
 * Signature: \code void callback(uint16_t value, void *user_data) \endcode
 * 
 * This callback is triggered when the threshold as set by
 * {@link rotary_poti_set_analog_value_callback_threshold} is reached.
 * The parameter is the analog value of the rotary potentiometer.
 * 
 * If the threshold keeps being reached, the callback is triggered periodically
 * with the period as set by {@link rotary_poti_set_debounce_period}.
 */
#define ROTARY_POTI_CALLBACK_ANALOG_VALUE_REACHED 16


/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_THRESHOLD_OPTION_OFF 'x'

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_THRESHOLD_OPTION_OUTSIDE 'o'

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_THRESHOLD_OPTION_INSIDE 'i'

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_THRESHOLD_OPTION_SMALLER '<'

/**
 * \ingroup BrickletRotaryPoti
 */
#define ROTARY_POTI_THRESHOLD_OPTION_GREATER '>'

/**
 * \ingroup BrickletRotaryPoti
 *
 * This constant is used to identify a Rotary Poti Bricklet.
 *
 * The {@link rotary_poti_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define ROTARY_POTI_DEVICE_IDENTIFIER 215

/**
 * \ingroup BrickletRotaryPoti
 *
 * This constant represents the display name of a Rotary Poti Bricklet.
 */
#define ROTARY_POTI_DEVICE_DISPLAY_NAME "Rotary Poti Bricklet"

/**
 * \ingroup BrickletRotaryPoti
 *
 * Creates the device object \c rotary_poti with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void rotary_poti_create(RotaryPoti *rotary_poti, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Removes the device object \c rotary_poti from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void rotary_poti_destroy(RotaryPoti *rotary_poti);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the rotary_poti_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int rotary_poti_get_response_expected(RotaryPoti *rotary_poti, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickletRotaryPoti
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
int rotary_poti_set_response_expected(RotaryPoti *rotary_poti, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int rotary_poti_set_response_expected_all(RotaryPoti *rotary_poti, bool response_expected);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void rotary_poti_register_callback(RotaryPoti *rotary_poti, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int rotary_poti_get_api_version(RotaryPoti *rotary_poti, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the position of the rotary potentiometer. The value is
 * between -150° (turned left) and 150° (turned right).
 * 
 * If you want to get the position periodically, it is recommended to use the
 * {@link ROTARY_POTI_CALLBACK_POSITION} callback and set the period with
 * {@link rotary_poti_set_position_callback_period}.
 */
int rotary_poti_get_position(RotaryPoti *rotary_poti, int16_t *ret_position);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the value as read by a 12-bit analog-to-digital converter.
 * 
 * \note
 *  The value returned by {@link rotary_poti_get_position} is averaged over several samples
 *  to yield less noise, while {@link rotary_poti_get_analog_value} gives back raw
 *  unfiltered analog values. The only reason to use {@link rotary_poti_get_analog_value} is,
 *  if you need the full resolution of the analog-to-digital converter.
 * 
 * If you want the analog value periodically, it is recommended to use the
 * {@link ROTARY_POTI_CALLBACK_ANALOG_VALUE} callback and set the period with
 * {@link rotary_poti_set_analog_value_callback_period}.
 */
int rotary_poti_get_analog_value(RotaryPoti *rotary_poti, uint16_t *ret_value);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Sets the period with which the {@link ROTARY_POTI_CALLBACK_POSITION} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * The {@link ROTARY_POTI_CALLBACK_POSITION} callback is only triggered if the position has changed since
 * the last triggering.
 */
int rotary_poti_set_position_callback_period(RotaryPoti *rotary_poti, uint32_t period);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the period as set by {@link rotary_poti_set_position_callback_period}.
 */
int rotary_poti_get_position_callback_period(RotaryPoti *rotary_poti, uint32_t *ret_period);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Sets the period with which the {@link ROTARY_POTI_CALLBACK_ANALOG_VALUE} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * The {@link ROTARY_POTI_CALLBACK_ANALOG_VALUE} callback is only triggered if the analog value has
 * changed since the last triggering.
 */
int rotary_poti_set_analog_value_callback_period(RotaryPoti *rotary_poti, uint32_t period);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the period as set by {@link rotary_poti_set_analog_value_callback_period}.
 */
int rotary_poti_get_analog_value_callback_period(RotaryPoti *rotary_poti, uint32_t *ret_period);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Sets the thresholds for the {@link ROTARY_POTI_CALLBACK_POSITION_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the position is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the position is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the position is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the position is greater than the min value (max is ignored)"
 * \endverbatim
 */
int rotary_poti_set_position_callback_threshold(RotaryPoti *rotary_poti, char option, int16_t min, int16_t max);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the threshold as set by {@link rotary_poti_set_position_callback_threshold}.
 */
int rotary_poti_get_position_callback_threshold(RotaryPoti *rotary_poti, char *ret_option, int16_t *ret_min, int16_t *ret_max);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Sets the thresholds for the {@link ROTARY_POTI_CALLBACK_ANALOG_VALUE_REACHED} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Callback is turned off"
 *  "'o'",    "Callback is triggered when the analog value is *outside* the min and max values"
 *  "'i'",    "Callback is triggered when the analog value is *inside* the min and max values"
 *  "'<'",    "Callback is triggered when the analog value is smaller than the min value (max is ignored)"
 *  "'>'",    "Callback is triggered when the analog value is greater than the min value (max is ignored)"
 * \endverbatim
 */
int rotary_poti_set_analog_value_callback_threshold(RotaryPoti *rotary_poti, char option, uint16_t min, uint16_t max);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the threshold as set by {@link rotary_poti_set_analog_value_callback_threshold}.
 */
int rotary_poti_get_analog_value_callback_threshold(RotaryPoti *rotary_poti, char *ret_option, uint16_t *ret_min, uint16_t *ret_max);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Sets the period with which the threshold callbacks
 * 
 * * {@link ROTARY_POTI_CALLBACK_POSITION_REACHED},
 * * {@link ROTARY_POTI_CALLBACK_ANALOG_VALUE_REACHED}
 * 
 * are triggered, if the thresholds
 * 
 * * {@link rotary_poti_set_position_callback_threshold},
 * * {@link rotary_poti_set_analog_value_callback_threshold}
 * 
 * keep being reached.
 */
int rotary_poti_set_debounce_period(RotaryPoti *rotary_poti, uint32_t debounce);

/**
 * \ingroup BrickletRotaryPoti
 *
 * Returns the debounce period as set by {@link rotary_poti_set_debounce_period}.
 */
int rotary_poti_get_debounce_period(RotaryPoti *rotary_poti, uint32_t *ret_debounce);

/**
 * \ingroup BrickletRotaryPoti
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
int rotary_poti_get_identity(RotaryPoti *rotary_poti, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
