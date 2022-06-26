/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICKLET_PIEZO_SPEAKER_V2_H
#define BRICKLET_PIEZO_SPEAKER_V2_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickletPiezoSpeakerV2 Piezo Speaker Bricklet 2.0
 */

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Creates beep and alarm with configurable volume and frequency
 */
typedef Device PiezoSpeakerV2;

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_SET_BEEP 1

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_GET_BEEP 2

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_SET_ALARM 3

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_GET_ALARM 4

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_UPDATE_VOLUME 5

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_UPDATE_FREQUENCY 6

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_GET_SPITFP_ERROR_COUNT 234

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_SET_BOOTLOADER_MODE 235

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_GET_BOOTLOADER_MODE 236

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_SET_WRITE_FIRMWARE_POINTER 237

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_WRITE_FIRMWARE 238

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_SET_STATUS_LED_CONFIG 239

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_GET_STATUS_LED_CONFIG 240

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_GET_CHIP_TEMPERATURE 242

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_RESET 243

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_WRITE_UID 248

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_READ_UID 249

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Signature: \code void callback(void *user_data) \endcode
 * 
 * This callback is triggered if a beep set by {@link piezo_speaker_v2_set_beep} is finished
 */
#define PIEZO_SPEAKER_V2_CALLBACK_BEEP_FINISHED 7

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Signature: \code void callback(void *user_data) \endcode
 * 
 * This callback is triggered if a alarm set by {@link piezo_speaker_v2_set_alarm} is finished
 */
#define PIEZO_SPEAKER_V2_CALLBACK_ALARM_FINISHED 8


/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BEEP_DURATION_OFF 0

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BEEP_DURATION_INFINITE 4294967295

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_ALARM_DURATION_OFF 0

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_ALARM_DURATION_INFINITE 4294967295

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_MODE_BOOTLOADER 0

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_MODE_FIRMWARE 1

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_STATUS_OK 0

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_STATUS_INVALID_MODE 1

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_STATUS_NO_CHANGE 2

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_BOOTLOADER_STATUS_CRC_MISMATCH 5

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_STATUS_LED_CONFIG_OFF 0

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_STATUS_LED_CONFIG_ON 1

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2

/**
 * \ingroup BrickletPiezoSpeakerV2
 */
#define PIEZO_SPEAKER_V2_STATUS_LED_CONFIG_SHOW_STATUS 3

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * This constant is used to identify a Piezo Speaker Bricklet 2.0.
 *
 * The {@link piezo_speaker_v2_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define PIEZO_SPEAKER_V2_DEVICE_IDENTIFIER 2145

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * This constant represents the display name of a Piezo Speaker Bricklet 2.0.
 */
#define PIEZO_SPEAKER_V2_DEVICE_DISPLAY_NAME "Piezo Speaker Bricklet 2.0"

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Creates the device object \c piezo_speaker_v2 with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void piezo_speaker_v2_create(PiezoSpeakerV2 *piezo_speaker_v2, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Removes the device object \c piezo_speaker_v2 from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void piezo_speaker_v2_destroy(PiezoSpeakerV2 *piezo_speaker_v2);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the piezo_speaker_v2_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int piezo_speaker_v2_get_response_expected(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickletPiezoSpeakerV2
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
int piezo_speaker_v2_set_response_expected(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int piezo_speaker_v2_set_response_expected_all(PiezoSpeakerV2 *piezo_speaker_v2, bool response_expected);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void piezo_speaker_v2_register_callback(PiezoSpeakerV2 *piezo_speaker_v2, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int piezo_speaker_v2_get_api_version(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Beeps with the given frequency and volume for the duration.
 * 
 * A duration of 0 stops the current beep if any is ongoing.
 * A duration of 4294967295 results in an infinite beep.
 */
int piezo_speaker_v2_set_beep(PiezoSpeakerV2 *piezo_speaker_v2, uint16_t frequency, uint8_t volume, uint32_t duration);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the last beep settings as set by {@link piezo_speaker_v2_set_beep}. If a beep is currently
 * running it also returns the remaining duration of the beep.
 * 
 * If the frequency or volume is updated during a beep (with {@link piezo_speaker_v2_update_frequency}
 * or {@link piezo_speaker_v2_update_volume}) this function returns the updated value.
 */
int piezo_speaker_v2_get_beep(PiezoSpeakerV2 *piezo_speaker_v2, uint16_t *ret_frequency, uint8_t *ret_volume, uint32_t *ret_duration, uint32_t *ret_duration_remaining);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Creates an alarm (a tone that goes back and force between two specified frequencies).
 * 
 * The following parameters can be set:
 * 
 * * Start Frequency: Start frequency of the alarm.
 * * End Frequency: End frequency of the alarm.
 * * Step Size: Size of one step of the sweep between the start/end frequencies.
 * * Step Delay: Delay between two steps (duration of time that one tone is used in a sweep).
 * * Duration: Duration of the alarm.
 * 
 * A duration of 0 stops the current alarm if any is ongoing.
 * A duration of 4294967295 results in an infinite alarm.
 * 
 * Below you can find two sets of example settings that you can try out. You can use
 * these as a starting point to find an alarm signal that suits your application.
 * 
 * Example 1: 10 seconds of loud annoying fast alarm
 * 
 * * Start Frequency = 800
 * * End Frequency = 2000
 * * Step Size = 10
 * * Step Delay = 1
 * * Volume = 10
 * * Duration = 10000
 * 
 * Example 2: 10 seconds of soft siren sound with slow build-up
 * 
 * * Start Frequency = 250
 * * End Frequency = 750
 * * Step Size = 1
 * * Step Delay = 5
 * * Volume = 0
 * * Duration = 10000
 * 
 * The following conditions must be met:
 * 
 * * Start Frequency: has to be smaller than end frequency
 * * End Frequency: has to be bigger than start frequency
 * * Step Size: has to be small enough to fit into the frequency range
 * * Step Delay: has to be small enough to fit into the duration
 */
int piezo_speaker_v2_set_alarm(PiezoSpeakerV2 *piezo_speaker_v2, uint16_t start_frequency, uint16_t end_frequency, uint16_t step_size, uint16_t step_delay, uint8_t volume, uint32_t duration);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the last alarm settings as set by {@link piezo_speaker_v2_set_alarm}. If an alarm is currently
 * running it also returns the remaining duration of the alarm as well as the
 * current frequency of the alarm.
 * 
 * If the volume is updated during an alarm (with {@link piezo_speaker_v2_update_volume})
 * this function returns the updated value.
 */
int piezo_speaker_v2_get_alarm(PiezoSpeakerV2 *piezo_speaker_v2, uint16_t *ret_start_frequency, uint16_t *ret_end_frequency, uint16_t *ret_step_size, uint16_t *ret_step_delay, uint8_t *ret_volume, uint32_t *ret_duration, uint32_t *ret_duration_remaining, uint16_t *ret_current_frequency);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Updates the volume of an ongoing beep or alarm.
 */
int piezo_speaker_v2_update_volume(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t volume);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Updates the frequency of an ongoing beep.
 */
int piezo_speaker_v2_update_frequency(PiezoSpeakerV2 *piezo_speaker_v2, uint16_t frequency);

/**
 * \ingroup BrickletPiezoSpeakerV2
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
int piezo_speaker_v2_get_spitfp_error_count(PiezoSpeakerV2 *piezo_speaker_v2, uint32_t *ret_error_count_ack_checksum, uint32_t *ret_error_count_message_checksum, uint32_t *ret_error_count_frame, uint32_t *ret_error_count_overflow);

/**
 * \ingroup BrickletPiezoSpeakerV2
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
int piezo_speaker_v2_set_bootloader_mode(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t mode, uint8_t *ret_status);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the current bootloader mode, see {@link piezo_speaker_v2_set_bootloader_mode}.
 */
int piezo_speaker_v2_get_bootloader_mode(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t *ret_mode);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Sets the firmware pointer for {@link piezo_speaker_v2_write_firmware}. The pointer has
 * to be increased by chunks of size 64. The data is written to flash
 * every 4 chunks (which equals to one page of size 256).
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int piezo_speaker_v2_set_write_firmware_pointer(PiezoSpeakerV2 *piezo_speaker_v2, uint32_t pointer);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Writes 64 Bytes of firmware at the position as written by
 * {@link piezo_speaker_v2_set_write_firmware_pointer} before. The firmware is written
 * to flash every 4 chunks.
 * 
 * You can only write firmware in bootloader mode.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int piezo_speaker_v2_write_firmware(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t data[64], uint8_t *ret_status);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Sets the status LED configuration. By default the LED shows
 * communication traffic between Brick and Bricklet, it flickers once
 * for every 10 received data packets.
 * 
 * You can also turn the LED permanently on/off or show a heartbeat.
 * 
 * If the Bricklet is in bootloader mode, the LED is will show heartbeat by default.
 */
int piezo_speaker_v2_set_status_led_config(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t config);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the configuration as set by {@link piezo_speaker_v2_set_status_led_config}
 */
int piezo_speaker_v2_get_status_led_config(PiezoSpeakerV2 *piezo_speaker_v2, uint8_t *ret_config);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the temperature as measured inside the microcontroller. The
 * value returned is not the ambient temperature!
 * 
 * The temperature is only proportional to the real temperature and it has bad
 * accuracy. Practically it is only useful as an indicator for
 * temperature changes.
 */
int piezo_speaker_v2_get_chip_temperature(PiezoSpeakerV2 *piezo_speaker_v2, int16_t *ret_temperature);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Calling this function will reset the Bricklet. All configurations
 * will be lost.
 * 
 * After a reset you have to create new device objects,
 * calling functions on the existing ones will result in
 * undefined behavior!
 */
int piezo_speaker_v2_reset(PiezoSpeakerV2 *piezo_speaker_v2);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Writes a new UID into flash. If you want to set a new UID
 * you have to decode the Base58 encoded UID string into an
 * integer first.
 * 
 * We recommend that you use Brick Viewer to change the UID.
 */
int piezo_speaker_v2_write_uid(PiezoSpeakerV2 *piezo_speaker_v2, uint32_t uid);

/**
 * \ingroup BrickletPiezoSpeakerV2
 *
 * Returns the current UID as an integer. Encode as
 * Base58 to get the usual string version.
 */
int piezo_speaker_v2_read_uid(PiezoSpeakerV2 *piezo_speaker_v2, uint32_t *ret_uid);

/**
 * \ingroup BrickletPiezoSpeakerV2
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
int piezo_speaker_v2_get_identity(PiezoSpeakerV2 *piezo_speaker_v2, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
