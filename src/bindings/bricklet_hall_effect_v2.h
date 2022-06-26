/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICKLET_HALL_EFFECT_V2_H
#define BRICKLET_HALL_EFFECT_V2_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickletHallEffectV2 Hall Effect Bricklet 2.0
 */

/**
 * \ingroup BrickletHallEffectV2
 *
 * Measures magnetic flux density between -7mT and +7mT
 */
typedef Device HallEffectV2;

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_MAGNETIC_FLUX_DENSITY 1

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_SET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION 2

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION 3

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_COUNTER 5

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_SET_COUNTER_CONFIG 6

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_COUNTER_CONFIG 7

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_SET_COUNTER_CALLBACK_CONFIGURATION 8

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_COUNTER_CALLBACK_CONFIGURATION 9

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_SPITFP_ERROR_COUNT 234

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_SET_BOOTLOADER_MODE 235

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_BOOTLOADER_MODE 236

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_SET_WRITE_FIRMWARE_POINTER 237

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_WRITE_FIRMWARE 238

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_SET_STATUS_LED_CONFIG 239

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_STATUS_LED_CONFIG 240

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_CHIP_TEMPERATURE 242

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_RESET 243

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_WRITE_UID 248

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_READ_UID 249

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickletHallEffectV2
 *
 * Signature: \code void callback(int16_t magnetic_flux_density, void *user_data) \endcode
 * 
 * This callback is triggered periodically according to the configuration set by
 * {@link hall_effect_v2_set_magnetic_flux_density_callback_configuration}.
 * 
 * The parameter is the same as {@link hall_effect_v2_get_magnetic_flux_density}.
 */
#define HALL_EFFECT_V2_CALLBACK_MAGNETIC_FLUX_DENSITY 4

/**
 * \ingroup BrickletHallEffectV2
 *
 * Signature: \code void callback(uint32_t count, void *user_data) \endcode
 * 
 * This callback is triggered periodically according to the configuration set by
 * {@link hall_effect_v2_set_counter_callback_configuration}.
 * 
 * The count is the same as you would get with {@link hall_effect_v2_get_counter}.
 */
#define HALL_EFFECT_V2_CALLBACK_COUNTER 10


/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_THRESHOLD_OPTION_OFF 'x'

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_THRESHOLD_OPTION_OUTSIDE 'o'

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_THRESHOLD_OPTION_INSIDE 'i'

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_THRESHOLD_OPTION_SMALLER '<'

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_THRESHOLD_OPTION_GREATER '>'

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_MODE_BOOTLOADER 0

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_MODE_FIRMWARE 1

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_STATUS_OK 0

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_STATUS_INVALID_MODE 1

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_STATUS_NO_CHANGE 2

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_BOOTLOADER_STATUS_CRC_MISMATCH 5

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_STATUS_LED_CONFIG_OFF 0

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_STATUS_LED_CONFIG_ON 1

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2

/**
 * \ingroup BrickletHallEffectV2
 */
#define HALL_EFFECT_V2_STATUS_LED_CONFIG_SHOW_STATUS 3

/**
 * \ingroup BrickletHallEffectV2
 *
 * This constant is used to identify a Hall Effect Bricklet 2.0.
 *
 * The {@link hall_effect_v2_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define HALL_EFFECT_V2_DEVICE_IDENTIFIER 2132

/**
 * \ingroup BrickletHallEffectV2
 *
 * This constant represents the display name of a Hall Effect Bricklet 2.0.
 */
#define HALL_EFFECT_V2_DEVICE_DISPLAY_NAME "Hall Effect Bricklet 2.0"

/**
 * \ingroup BrickletHallEffectV2
 *
 * Creates the device object \c hall_effect_v2 with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void hall_effect_v2_create(HallEffectV2 *hall_effect_v2, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Removes the device object \c hall_effect_v2 from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void hall_effect_v2_destroy(HallEffectV2 *hall_effect_v2);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the hall_effect_v2_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int hall_effect_v2_get_response_expected(HallEffectV2 *hall_effect_v2, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickletHallEffectV2
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
int hall_effect_v2_set_response_expected(HallEffectV2 *hall_effect_v2, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int hall_effect_v2_set_response_expected_all(HallEffectV2 *hall_effect_v2, bool response_expected);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void hall_effect_v2_register_callback(HallEffectV2 *hall_effect_v2, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int hall_effect_v2_get_api_version(HallEffectV2 *hall_effect_v2, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the `magnetic flux density (magnetic induction) <https://en.wikipedia.org/wiki/Magnetic_flux>`__.
 * 
 * 
 * If you want to get the value periodically, it is recommended to use the
 * {@link HALL_EFFECT_V2_CALLBACK_MAGNETIC_FLUX_DENSITY} callback. You can set the callback configuration
 * with {@link hall_effect_v2_set_magnetic_flux_density_callback_configuration}.
 */
int hall_effect_v2_get_magnetic_flux_density(HallEffectV2 *hall_effect_v2, int16_t *ret_magnetic_flux_density);

/**
 * \ingroup BrickletHallEffectV2
 *
 * The period is the period with which the {@link HALL_EFFECT_V2_CALLBACK_MAGNETIC_FLUX_DENSITY} callback is triggered
 * periodically. A value of 0 turns the callback off.
 * 
 * If the `value has to change`-parameter is set to true, the callback is only
 * triggered after the value has changed. If the value didn't change
 * within the period, the callback is triggered immediately on change.
 * 
 * If it is set to false, the callback is continuously triggered with the period,
 * independent of the value.
 * 
 * It is furthermore possible to constrain the callback with thresholds.
 * 
 * The `option`-parameter together with min/max sets a threshold for the {@link HALL_EFFECT_V2_CALLBACK_MAGNETIC_FLUX_DENSITY} callback.
 * 
 * The following options are possible:
 * 
 * \verbatim
 *  "Option", "Description"
 * 
 *  "'x'",    "Threshold is turned off"
 *  "'o'",    "Threshold is triggered when the value is *outside* the min and max values"
 *  "'i'",    "Threshold is triggered when the value is *inside* or equal to the min and max values"
 *  "'<'",    "Threshold is triggered when the value is smaller than the min value (max is ignored)"
 *  "'>'",    "Threshold is triggered when the value is greater than the min value (max is ignored)"
 * \endverbatim
 * 
 * If the option is set to 'x' (threshold turned off) the callback is triggered with the fixed period.
 */
int hall_effect_v2_set_magnetic_flux_density_callback_configuration(HallEffectV2 *hall_effect_v2, uint32_t period, bool value_has_to_change, char option, int16_t min, int16_t max);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the callback configuration as set by {@link hall_effect_v2_set_magnetic_flux_density_callback_configuration}.
 */
int hall_effect_v2_get_magnetic_flux_density_callback_configuration(HallEffectV2 *hall_effect_v2, uint32_t *ret_period, bool *ret_value_has_to_change, char *ret_option, int16_t *ret_min, int16_t *ret_max);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the current value of the counter.
 * 
 * You can configure the low/high thresholds and the debounce time
 * with {@link hall_effect_v2_set_counter_config}.
 * 
 * If you set reset counter to *true*, the count is set back to 0
 * directly after it is read.
 * 
 * If you want to get the count periodically, it is recommended to use the
 * {@link HALL_EFFECT_V2_CALLBACK_COUNTER} callback. You can set the callback configuration
 * with {@link hall_effect_v2_set_counter_callback_configuration}.
 */
int hall_effect_v2_get_counter(HallEffectV2 *hall_effect_v2, bool reset_counter, uint32_t *ret_count);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Sets a high and a low threshold as well as a debounce time.
 * 
 * If the measured magnetic flux density goes above the high threshold or
 * below the low threshold, the count of the counter is increased by 1.
 * 
 * The debounce time is the minimum time between two count increments.
 */
int hall_effect_v2_set_counter_config(HallEffectV2 *hall_effect_v2, int16_t high_threshold, int16_t low_threshold, uint32_t debounce);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the counter config as set by {@link hall_effect_v2_set_counter_config}.
 */
int hall_effect_v2_get_counter_config(HallEffectV2 *hall_effect_v2, int16_t *ret_high_threshold, int16_t *ret_low_threshold, uint32_t *ret_debounce);

/**
 * \ingroup BrickletHallEffectV2
 *
 * The period is the period with which the {@link HALL_EFFECT_V2_CALLBACK_COUNTER}
 * callback is triggered periodically. A value of 0 turns the callback off.
 * 
 * If the `value has to change`-parameter is set to true, the callback is only
 * triggered after at least one of the values has changed. If the values didn't
 * change within the period, the callback is triggered immediately on change.
 * 
 * If it is set to false, the callback is continuously triggered with the period,
 * independent of the value.
 */
int hall_effect_v2_set_counter_callback_configuration(HallEffectV2 *hall_effect_v2, uint32_t period, bool value_has_to_change);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the callback configuration as set by
 * {@link hall_effect_v2_set_counter_callback_configuration}.
 */
int hall_effect_v2_get_counter_callback_configuration(HallEffectV2 *hall_effect_v2, uint32_t *ret_period, bool *ret_value_has_to_change);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the error count for the communication between Brick and Bricklet.
 * 
 * The errors are divided into
 * 
 * * ACK checksum errors,
 * * message checksum errors,
 * * framing errors and
 * * overflow errors.
 * 
 * The errors counts are for errors that occur on the Bricklet side. All
 * Bricks have a similar function that returns the errors on the Brick side.
 */
int hall_effect_v2_get_spitfp_error_count(HallEffectV2 *hall_effect_v2, uint32_t *ret_error_count_ack_checksum, uint32_t *ret_error_count_message_checksum, uint32_t *ret_error_count_frame, uint32_t *ret_error_count_overflow);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Sets the bootloader mode and returns the status after the requested
 * mode change was instigated.
 * 
 * You can change from bootloader mode to firmware mode and vice versa. A change
 * from bootloader mode to firmware mode will only take place if the entry function,
 * device identifier and CRC are present and correct.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int hall_effect_v2_set_bootloader_mode(HallEffectV2 *hall_effect_v2, uint8_t mode, uint8_t *ret_status);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the current bootloader mode, see {@link hall_effect_v2_set_bootloader_mode}.
 */
int hall_effect_v2_get_bootloader_mode(HallEffectV2 *hall_effect_v2, uint8_t *ret_mode);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Sets the firmware pointer for {@link hall_effect_v2_write_firmware}. The pointer has
 * to be increased by chunks of size 64. The data is written to flash
 * every 4 chunks (which equals to one page of size 256).
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int hall_effect_v2_set_write_firmware_pointer(HallEffectV2 *hall_effect_v2, uint32_t pointer);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Writes 64 Bytes of firmware at the position as written by
 * {@link hall_effect_v2_set_write_firmware_pointer} before. The firmware is written
 * to flash every 4 chunks.
 * 
 * You can only write firmware in bootloader mode.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int hall_effect_v2_write_firmware(HallEffectV2 *hall_effect_v2, uint8_t data[64], uint8_t *ret_status);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Sets the status LED configuration. By default the LED shows
 * communication traffic between Brick and Bricklet, it flickers once
 * for every 10 received data packets.
 * 
 * You can also turn the LED permanently on/off or show a heartbeat.
 * 
 * If the Bricklet is in bootloader mode, the LED is will show heartbeat by default.
 */
int hall_effect_v2_set_status_led_config(HallEffectV2 *hall_effect_v2, uint8_t config);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the configuration as set by {@link hall_effect_v2_set_status_led_config}
 */
int hall_effect_v2_get_status_led_config(HallEffectV2 *hall_effect_v2, uint8_t *ret_config);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the temperature as measured inside the microcontroller. The
 * value returned is not the ambient temperature!
 * 
 * The temperature is only proportional to the real temperature and it has bad
 * accuracy. Practically it is only useful as an indicator for
 * temperature changes.
 */
int hall_effect_v2_get_chip_temperature(HallEffectV2 *hall_effect_v2, int16_t *ret_temperature);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Calling this function will reset the Bricklet. All configurations
 * will be lost.
 * 
 * After a reset you have to create new device objects,
 * calling functions on the existing ones will result in
 * undefined behavior!
 */
int hall_effect_v2_reset(HallEffectV2 *hall_effect_v2);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Writes a new UID into flash. If you want to set a new UID
 * you have to decode the Base58 encoded UID string into an
 * integer first.
 * 
 * We recommend that you use Brick Viewer to change the UID.
 */
int hall_effect_v2_write_uid(HallEffectV2 *hall_effect_v2, uint32_t uid);

/**
 * \ingroup BrickletHallEffectV2
 *
 * Returns the current UID as an integer. Encode as
 * Base58 to get the usual string version.
 */
int hall_effect_v2_read_uid(HallEffectV2 *hall_effect_v2, uint32_t *ret_uid);

/**
 * \ingroup BrickletHallEffectV2
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
int hall_effect_v2_get_identity(HallEffectV2 *hall_effect_v2, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
