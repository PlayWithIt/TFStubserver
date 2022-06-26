/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICKLET_ANALOG_IN_V3_H
#define BRICKLET_ANALOG_IN_V3_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickletAnalogInV3 Analog In Bricklet 3.0
 */

/**
 * \ingroup BrickletAnalogInV3
 *
 * Measures DC voltage between 0V and 42V
 */
typedef Device AnalogInV3;

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_VOLTAGE 1

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_SET_VOLTAGE_CALLBACK_CONFIGURATION 2

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_VOLTAGE_CALLBACK_CONFIGURATION 3

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_SET_OVERSAMPLING 5

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_OVERSAMPLING 6

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_SET_CALIBRATION 7

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_CALIBRATION 8

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_SPITFP_ERROR_COUNT 234

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_SET_BOOTLOADER_MODE 235

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_BOOTLOADER_MODE 236

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_SET_WRITE_FIRMWARE_POINTER 237

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_WRITE_FIRMWARE 238

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_SET_STATUS_LED_CONFIG 239

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_STATUS_LED_CONFIG 240

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_CHIP_TEMPERATURE 242

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_RESET 243

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_WRITE_UID 248

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_READ_UID 249

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickletAnalogInV3
 *
 * Signature: \code void callback(uint16_t voltage, void *user_data) \endcode
 * 
 * This callback is triggered periodically according to the configuration set by
 * {@link analog_in_v3_set_voltage_callback_configuration}.
 * 
 * The parameter is the same as {@link analog_in_v3_get_voltage}.
 */
#define ANALOG_IN_V3_CALLBACK_VOLTAGE 4


/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_THRESHOLD_OPTION_OFF 'x'

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_THRESHOLD_OPTION_OUTSIDE 'o'

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_THRESHOLD_OPTION_INSIDE 'i'

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_THRESHOLD_OPTION_SMALLER '<'

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_THRESHOLD_OPTION_GREATER '>'

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_32 0

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_64 1

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_128 2

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_256 3

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_512 4

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_1024 5

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_2048 6

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_4096 7

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_8192 8

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_OVERSAMPLING_16384 9

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_MODE_BOOTLOADER 0

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_MODE_FIRMWARE 1

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_STATUS_OK 0

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_STATUS_INVALID_MODE 1

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_STATUS_NO_CHANGE 2

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_BOOTLOADER_STATUS_CRC_MISMATCH 5

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_STATUS_LED_CONFIG_OFF 0

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_STATUS_LED_CONFIG_ON 1

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2

/**
 * \ingroup BrickletAnalogInV3
 */
#define ANALOG_IN_V3_STATUS_LED_CONFIG_SHOW_STATUS 3

/**
 * \ingroup BrickletAnalogInV3
 *
 * This constant is used to identify a Analog In Bricklet 3.0.
 *
 * The {@link analog_in_v3_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define ANALOG_IN_V3_DEVICE_IDENTIFIER 295

/**
 * \ingroup BrickletAnalogInV3
 *
 * This constant represents the display name of a Analog In Bricklet 3.0.
 */
#define ANALOG_IN_V3_DEVICE_DISPLAY_NAME "Analog In Bricklet 3.0"

/**
 * \ingroup BrickletAnalogInV3
 *
 * Creates the device object \c analog_in_v3 with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void analog_in_v3_create(AnalogInV3 *analog_in_v3, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Removes the device object \c analog_in_v3 from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void analog_in_v3_destroy(AnalogInV3 *analog_in_v3);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the analog_in_v3_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int analog_in_v3_get_response_expected(AnalogInV3 *analog_in_v3, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickletAnalogInV3
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
int analog_in_v3_set_response_expected(AnalogInV3 *analog_in_v3, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int analog_in_v3_set_response_expected_all(AnalogInV3 *analog_in_v3, bool response_expected);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void analog_in_v3_register_callback(AnalogInV3 *analog_in_v3, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int analog_in_v3_get_api_version(AnalogInV3 *analog_in_v3, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the measured voltage. The resolution is approximately 10mV to 1mV
 * depending on the oversampling configuration ({@link analog_in_v3_set_oversampling}).
 * 
 * 
 * If you want to get the value periodically, it is recommended to use the
 * {@link ANALOG_IN_V3_CALLBACK_VOLTAGE} callback. You can set the callback configuration
 * with {@link analog_in_v3_set_voltage_callback_configuration}.
 */
int analog_in_v3_get_voltage(AnalogInV3 *analog_in_v3, uint16_t *ret_voltage);

/**
 * \ingroup BrickletAnalogInV3
 *
 * The period is the period with which the {@link ANALOG_IN_V3_CALLBACK_VOLTAGE} callback is triggered
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
 * The `option`-parameter together with min/max sets a threshold for the {@link ANALOG_IN_V3_CALLBACK_VOLTAGE} callback.
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
int analog_in_v3_set_voltage_callback_configuration(AnalogInV3 *analog_in_v3, uint32_t period, bool value_has_to_change, char option, uint16_t min, uint16_t max);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the callback configuration as set by {@link analog_in_v3_set_voltage_callback_configuration}.
 */
int analog_in_v3_get_voltage_callback_configuration(AnalogInV3 *analog_in_v3, uint32_t *ret_period, bool *ret_value_has_to_change, char *ret_option, uint16_t *ret_min, uint16_t *ret_max);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Sets the oversampling between 32x and 16384x. The Bricklet
 * takes one 12bit sample every 17.5µs. Thus an oversampling
 * of 32x is equivalent to an integration time of 0.56ms and
 * a oversampling of 16384x is equivalent to an integration
 * time of 286ms.
 * 
 * The oversampling uses the moving average principle. A
 * new value is always calculated once per millisecond.
 * 
 * With increased oversampling the noise decreases. With decreased
 * oversampling the reaction time increases (changes in voltage will be
 * measured faster).
 */
int analog_in_v3_set_oversampling(AnalogInV3 *analog_in_v3, uint8_t oversampling);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the oversampling value as set by {@link analog_in_v3_set_oversampling}.
 */
int analog_in_v3_get_oversampling(AnalogInV3 *analog_in_v3, uint8_t *ret_oversampling);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Sets a calibration for the measured voltage value.
 * The formula for the calibration is as follows::
 * 
 *  Calibrated Value = (Value + Offset) * Multiplier / Divisor
 * 
 * We recommend that you use the Brick Viewer to calibrate
 * the Bricklet. The calibration will be saved internally and only
 * has to be done once.
 */
int analog_in_v3_set_calibration(AnalogInV3 *analog_in_v3, int16_t offset, uint16_t multiplier, uint16_t divisor);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the calibration as set by {@link analog_in_v3_set_calibration}.
 */
int analog_in_v3_get_calibration(AnalogInV3 *analog_in_v3, int16_t *ret_offset, uint16_t *ret_multiplier, uint16_t *ret_divisor);

/**
 * \ingroup BrickletAnalogInV3
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
int analog_in_v3_get_spitfp_error_count(AnalogInV3 *analog_in_v3, uint32_t *ret_error_count_ack_checksum, uint32_t *ret_error_count_message_checksum, uint32_t *ret_error_count_frame, uint32_t *ret_error_count_overflow);

/**
 * \ingroup BrickletAnalogInV3
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
int analog_in_v3_set_bootloader_mode(AnalogInV3 *analog_in_v3, uint8_t mode, uint8_t *ret_status);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the current bootloader mode, see {@link analog_in_v3_set_bootloader_mode}.
 */
int analog_in_v3_get_bootloader_mode(AnalogInV3 *analog_in_v3, uint8_t *ret_mode);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Sets the firmware pointer for {@link analog_in_v3_write_firmware}. The pointer has
 * to be increased by chunks of size 64. The data is written to flash
 * every 4 chunks (which equals to one page of size 256).
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int analog_in_v3_set_write_firmware_pointer(AnalogInV3 *analog_in_v3, uint32_t pointer);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Writes 64 Bytes of firmware at the position as written by
 * {@link analog_in_v3_set_write_firmware_pointer} before. The firmware is written
 * to flash every 4 chunks.
 * 
 * You can only write firmware in bootloader mode.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int analog_in_v3_write_firmware(AnalogInV3 *analog_in_v3, uint8_t data[64], uint8_t *ret_status);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Sets the status LED configuration. By default the LED shows
 * communication traffic between Brick and Bricklet, it flickers once
 * for every 10 received data packets.
 * 
 * You can also turn the LED permanently on/off or show a heartbeat.
 * 
 * If the Bricklet is in bootloader mode, the LED is will show heartbeat by default.
 */
int analog_in_v3_set_status_led_config(AnalogInV3 *analog_in_v3, uint8_t config);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the configuration as set by {@link analog_in_v3_set_status_led_config}
 */
int analog_in_v3_get_status_led_config(AnalogInV3 *analog_in_v3, uint8_t *ret_config);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the temperature as measured inside the microcontroller. The
 * value returned is not the ambient temperature!
 * 
 * The temperature is only proportional to the real temperature and it has bad
 * accuracy. Practically it is only useful as an indicator for
 * temperature changes.
 */
int analog_in_v3_get_chip_temperature(AnalogInV3 *analog_in_v3, int16_t *ret_temperature);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Calling this function will reset the Bricklet. All configurations
 * will be lost.
 * 
 * After a reset you have to create new device objects,
 * calling functions on the existing ones will result in
 * undefined behavior!
 */
int analog_in_v3_reset(AnalogInV3 *analog_in_v3);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Writes a new UID into flash. If you want to set a new UID
 * you have to decode the Base58 encoded UID string into an
 * integer first.
 * 
 * We recommend that you use Brick Viewer to change the UID.
 */
int analog_in_v3_write_uid(AnalogInV3 *analog_in_v3, uint32_t uid);

/**
 * \ingroup BrickletAnalogInV3
 *
 * Returns the current UID as an integer. Encode as
 * Base58 to get the usual string version.
 */
int analog_in_v3_read_uid(AnalogInV3 *analog_in_v3, uint32_t *ret_uid);

/**
 * \ingroup BrickletAnalogInV3
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
int analog_in_v3_get_identity(AnalogInV3 *analog_in_v3, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
