/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICK_DC_H
#define BRICK_DC_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickDC DC Brick
 */

/**
 * \ingroup BrickDC
 *
 * Drives one brushed DC motor with up to 28V and 5A (peak)
 */
typedef Device DC;

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_VELOCITY 1

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_VELOCITY 2

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_CURRENT_VELOCITY 3

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_ACCELERATION 4

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_ACCELERATION 5

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_PWM_FREQUENCY 6

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_PWM_FREQUENCY 7

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_FULL_BRAKE 8

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_STACK_INPUT_VOLTAGE 9

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_EXTERNAL_INPUT_VOLTAGE 10

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_CURRENT_CONSUMPTION 11

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_ENABLE 12

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_DISABLE 13

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_IS_ENABLED 14

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_MINIMUM_VOLTAGE 15

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_MINIMUM_VOLTAGE 16

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_DRIVE_MODE 17

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_DRIVE_MODE 18

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_CURRENT_VELOCITY_PERIOD 19

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_CURRENT_VELOCITY_PERIOD 20

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_SPITFP_BAUDRATE_CONFIG 231

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_SPITFP_BAUDRATE_CONFIG 232

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_SEND_TIMEOUT_COUNT 233

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_SET_SPITFP_BAUDRATE 234

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_SPITFP_BAUDRATE 235

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_SPITFP_ERROR_COUNT 237

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_ENABLE_STATUS_LED 238

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_DISABLE_STATUS_LED 239

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_IS_STATUS_LED_ENABLED 240

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_PROTOCOL1_BRICKLET_NAME 241

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_CHIP_TEMPERATURE 242

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_RESET 243

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_WRITE_BRICKLET_PLUGIN 246

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_READ_BRICKLET_PLUGIN 247

/**
 * \ingroup BrickDC
 */
#define DC_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickDC
 *
 * Signature: \code void callback(uint16_t voltage, void *user_data) \endcode
 * 
 * This callback is triggered when the input voltage drops below the value set by
 * {@link dc_set_minimum_voltage}. The parameter is the current voltage.
 */
#define DC_CALLBACK_UNDER_VOLTAGE 21

/**
 * \ingroup BrickDC
 *
 * Signature: \code void callback(void *user_data) \endcode
 * 
 * This callback is triggered if either the current consumption
 * is too high (above 5A) or the temperature of the driver chip is too high
 * (above 175°C). These two possibilities are essentially the same, since the
 * temperature will reach this threshold immediately if the motor consumes too
 * much current. In case of a voltage below 3.3V (external or stack) this
 * callback is triggered as well.
 * 
 * If this callback is triggered, the driver chip gets disabled at the same time.
 * That means, {@link dc_enable} has to be called to drive the motor again.
 * 
 * \note
 *  This callback only works in Drive/Brake mode (see {@link dc_set_drive_mode}). In
 *  Drive/Coast mode it is unfortunately impossible to reliably read the
 *  overcurrent/overtemperature signal from the driver chip.
 */
#define DC_CALLBACK_EMERGENCY_SHUTDOWN 22

/**
 * \ingroup BrickDC
 *
 * Signature: \code void callback(int16_t velocity, void *user_data) \endcode
 * 
 * This callback is triggered whenever a set velocity is reached. For example:
 * If a velocity of 0 is present, acceleration is set to 5000 and velocity
 * to 10000, the {@link DC_CALLBACK_VELOCITY_REACHED} callback will be triggered after about
 * 2 seconds, when the set velocity is actually reached.
 * 
 * \note
 *  Since we can't get any feedback from the DC motor, this only works if the
 *  acceleration (see {@link dc_set_acceleration}) is set smaller or equal to the
 *  maximum acceleration of the motor. Otherwise the motor will lag behind the
 *  control value and the callback will be triggered too early.
 */
#define DC_CALLBACK_VELOCITY_REACHED 23

/**
 * \ingroup BrickDC
 *
 * Signature: \code void callback(int16_t velocity, void *user_data) \endcode
 * 
 * This callback is triggered with the period that is set by
 * {@link dc_set_current_velocity_period}. The parameter is the *current*
 * velocity used by the motor.
 * 
 * The {@link DC_CALLBACK_CURRENT_VELOCITY} callback is only triggered after the set period
 * if there is a change in the velocity.
 */
#define DC_CALLBACK_CURRENT_VELOCITY 24


/**
 * \ingroup BrickDC
 */
#define DC_DRIVE_MODE_DRIVE_BRAKE 0

/**
 * \ingroup BrickDC
 */
#define DC_DRIVE_MODE_DRIVE_COAST 1

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_NONE 0

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_USB 1

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_SPI_STACK 2

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_CHIBI 3

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_RS485 4

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_WIFI 5

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_ETHERNET 6

/**
 * \ingroup BrickDC
 */
#define DC_COMMUNICATION_METHOD_WIFI_V2 7

/**
 * \ingroup BrickDC
 *
 * This constant is used to identify a DC Brick.
 *
 * The {@link dc_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define DC_DEVICE_IDENTIFIER 11

/**
 * \ingroup BrickDC
 *
 * This constant represents the display name of a DC Brick.
 */
#define DC_DEVICE_DISPLAY_NAME "DC Brick"

/**
 * \ingroup BrickDC
 *
 * Creates the device object \c dc with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void dc_create(DC *dc, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickDC
 *
 * Removes the device object \c dc from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void dc_destroy(DC *dc);

/**
 * \ingroup BrickDC
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the dc_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int dc_get_response_expected(DC *dc, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickDC
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
int dc_set_response_expected(DC *dc, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickDC
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int dc_set_response_expected_all(DC *dc, bool response_expected);

/**
 * \ingroup BrickDC
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void dc_register_callback(DC *dc, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickDC
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int dc_get_api_version(DC *dc, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickDC
 *
 * Sets the velocity of the motor. Whereas -32767 is full speed backward,
 * 0 is stop and 32767 is full speed forward. Depending on the
 * acceleration (see {@link dc_set_acceleration}), the motor is not immediately
 * brought to the velocity but smoothly accelerated.
 * 
 * The velocity describes the duty cycle of the PWM with which the motor is
 * controlled, e.g. a velocity of 3277 sets a PWM with a 10% duty cycle.
 * You can not only control the duty cycle of the PWM but also the frequency,
 * see {@link dc_set_pwm_frequency}.
 */
int dc_set_velocity(DC *dc, int16_t velocity);

/**
 * \ingroup BrickDC
 *
 * Returns the velocity as set by {@link dc_set_velocity}.
 */
int dc_get_velocity(DC *dc, int16_t *ret_velocity);

/**
 * \ingroup BrickDC
 *
 * Returns the *current* velocity of the motor. This value is different
 * from {@link dc_get_velocity} whenever the motor is currently accelerating
 * to a goal set by {@link dc_set_velocity}.
 */
int dc_get_current_velocity(DC *dc, int16_t *ret_velocity);

/**
 * \ingroup BrickDC
 *
 * Sets the acceleration of the motor. It is given in *velocity/s*. An
 * acceleration of 10000 means, that every second the velocity is increased
 * by 10000 (or about 30% duty cycle).
 * 
 * For example: If the current velocity is 0 and you want to accelerate to a
 * velocity of 16000 (about 50% duty cycle) in 10 seconds, you should set
 * an acceleration of 1600.
 * 
 * If acceleration is set to 0, there is no speed ramping, i.e. a new velocity
 * is immediately given to the motor.
 */
int dc_set_acceleration(DC *dc, uint16_t acceleration);

/**
 * \ingroup BrickDC
 *
 * Returns the acceleration as set by {@link dc_set_acceleration}.
 */
int dc_get_acceleration(DC *dc, uint16_t *ret_acceleration);

/**
 * \ingroup BrickDC
 *
 * Sets the frequency of the PWM with which the motor is driven.
 * Often a high frequency
 * is less noisy and the motor runs smoother. However, with a low frequency
 * there are less switches and therefore fewer switching losses. Also with
 * most motors lower frequencies enable higher torque.
 * 
 * If you have no idea what all this means, just ignore this function and use
 * the default frequency, it will very likely work fine.
 */
int dc_set_pwm_frequency(DC *dc, uint16_t frequency);

/**
 * \ingroup BrickDC
 *
 * Returns the PWM frequency as set by {@link dc_set_pwm_frequency}.
 */
int dc_get_pwm_frequency(DC *dc, uint16_t *ret_frequency);

/**
 * \ingroup BrickDC
 *
 * Executes an active full brake.
 * 
 * \warning
 *  This function is for emergency purposes,
 *  where an immediate brake is necessary. Depending on the current velocity and
 *  the strength of the motor, a full brake can be quite violent.
 * 
 * Call {@link dc_set_velocity} with 0 if you just want to stop the motor.
 */
int dc_full_brake(DC *dc);

/**
 * \ingroup BrickDC
 *
 * Returns the stack input voltage. The stack input voltage is the
 * voltage that is supplied via the stack, i.e. it is given by a
 * Step-Down or Step-Up Power Supply.
 */
int dc_get_stack_input_voltage(DC *dc, uint16_t *ret_voltage);

/**
 * \ingroup BrickDC
 *
 * Returns the external input voltage. The external input voltage is
 * given via the black power input connector on the DC Brick.
 * 
 * If there is an external input voltage and a stack input voltage, the motor
 * will be driven by the external input voltage. If there is only a stack
 * voltage present, the motor will be driven by this voltage.
 * 
 * \warning
 *  This means, if you have a high stack voltage and a low external voltage,
 *  the motor will be driven with the low external voltage. If you then remove
 *  the external connection, it will immediately be driven by the high
 *  stack voltage.
 */
int dc_get_external_input_voltage(DC *dc, uint16_t *ret_voltage);

/**
 * \ingroup BrickDC
 *
 * Returns the current consumption of the motor.
 */
int dc_get_current_consumption(DC *dc, uint16_t *ret_voltage);

/**
 * \ingroup BrickDC
 *
 * Enables the driver chip. The driver parameters can be configured (velocity,
 * acceleration, etc) before it is enabled.
 */
int dc_enable(DC *dc);

/**
 * \ingroup BrickDC
 *
 * Disables the driver chip. The configurations are kept (velocity,
 * acceleration, etc) but the motor is not driven until it is enabled again.
 * 
 * \warning
 *  Disabling the driver chip while the motor is still turning can damage the
 *  driver chip. The motor should be stopped calling {@link dc_set_velocity} with 0
 *  before disabling the motor power. The {@link dc_set_velocity} function will **not**
 *  wait until the motor is actually stopped. You have to explicitly wait for the
 *  appropriate time after calling the {@link dc_set_velocity} function before calling
 *  the {@link dc_disable} function.
 */
int dc_disable(DC *dc);

/**
 * \ingroup BrickDC
 *
 * Returns *true* if the driver chip is enabled, *false* otherwise.
 */
int dc_is_enabled(DC *dc, bool *ret_enabled);

/**
 * \ingroup BrickDC
 *
 * Sets the minimum voltage, below which the {@link DC_CALLBACK_UNDER_VOLTAGE} callback
 * is triggered. The minimum possible value that works with the DC Brick is 6V.
 * You can use this function to detect the discharge of a battery that is used
 * to drive the motor. If you have a fixed power supply, you likely do not need
 * this functionality.
 */
int dc_set_minimum_voltage(DC *dc, uint16_t voltage);

/**
 * \ingroup BrickDC
 *
 * Returns the minimum voltage as set by {@link dc_set_minimum_voltage}
 */
int dc_get_minimum_voltage(DC *dc, uint16_t *ret_voltage);

/**
 * \ingroup BrickDC
 *
 * Sets the drive mode. Possible modes are:
 * 
 * * 0 = Drive/Brake
 * * 1 = Drive/Coast
 * 
 * These modes are different kinds of motor controls.
 * 
 * In Drive/Brake mode, the motor is always either driving or braking. There
 * is no freewheeling. Advantages are: A more linear correlation between
 * PWM and velocity, more exact accelerations and the possibility to drive
 * with slower velocities.
 * 
 * In Drive/Coast mode, the motor is always either driving or freewheeling.
 * Advantages are: Less current consumption and less demands on the motor and
 * driver chip.
 */
int dc_set_drive_mode(DC *dc, uint8_t mode);

/**
 * \ingroup BrickDC
 *
 * Returns the drive mode, as set by {@link dc_set_drive_mode}.
 */
int dc_get_drive_mode(DC *dc, uint8_t *ret_mode);

/**
 * \ingroup BrickDC
 *
 * Sets a period with which the {@link DC_CALLBACK_CURRENT_VELOCITY} callback is triggered.
 * A period of 0 turns the callback off.
 */
int dc_set_current_velocity_period(DC *dc, uint16_t period);

/**
 * \ingroup BrickDC
 *
 * Returns the period as set by {@link dc_set_current_velocity_period}.
 */
int dc_get_current_velocity_period(DC *dc, uint16_t *ret_period);

/**
 * \ingroup BrickDC
 *
 * The SPITF protocol can be used with a dynamic baudrate. If the dynamic baudrate is
 * enabled, the Brick will try to adapt the baudrate for the communication
 * between Bricks and Bricklets according to the amount of data that is transferred.
 * 
 * The baudrate will be increased exponentially if lots of data is sent/received and
 * decreased linearly if little data is sent/received.
 * 
 * This lowers the baudrate in applications where little data is transferred (e.g.
 * a weather station) and increases the robustness. If there is lots of data to transfer
 * (e.g. Thermal Imaging Bricklet) it automatically increases the baudrate as needed.
 * 
 * In cases where some data has to transferred as fast as possible every few seconds
 * (e.g. RS485 Bricklet with a high baudrate but small payload) you may want to turn
 * the dynamic baudrate off to get the highest possible performance.
 * 
 * The maximum value of the baudrate can be set per port with the function
 * {@link dc_set_spitfp_baudrate}. If the dynamic baudrate is disabled, the baudrate
 * as set by {@link dc_set_spitfp_baudrate} will be used statically.
 * 
 * .. versionadded:: 2.3.5$nbsp;(Firmware)
 */
int dc_set_spitfp_baudrate_config(DC *dc, bool enable_dynamic_baudrate, uint32_t minimum_dynamic_baudrate);

/**
 * \ingroup BrickDC
 *
 * Returns the baudrate config, see {@link dc_set_spitfp_baudrate_config}.
 * 
 * .. versionadded:: 2.3.5$nbsp;(Firmware)
 */
int dc_get_spitfp_baudrate_config(DC *dc, bool *ret_enable_dynamic_baudrate, uint32_t *ret_minimum_dynamic_baudrate);

/**
 * \ingroup BrickDC
 *
 * Returns the timeout count for the different communication methods.
 * 
 * The methods 0-2 are available for all Bricks, 3-7 only for Master Bricks.
 * 
 * This function is mostly used for debugging during development, in normal operation
 * the counters should nearly always stay at 0.
 * 
 * .. versionadded:: 2.3.3$nbsp;(Firmware)
 */
int dc_get_send_timeout_count(DC *dc, uint8_t communication_method, uint32_t *ret_timeout_count);

/**
 * \ingroup BrickDC
 *
 * Sets the baudrate for a specific Bricklet port.
 * 
 * If you want to increase the throughput of Bricklets you can increase
 * the baudrate. If you get a high error count because of high
 * interference (see {@link dc_get_spitfp_error_count}) you can decrease the
 * baudrate.
 * 
 * If the dynamic baudrate feature is enabled, the baudrate set by this
 * function corresponds to the maximum baudrate (see {@link dc_set_spitfp_baudrate_config}).
 * 
 * Regulatory testing is done with the default baudrate. If CE compatibility
 * or similar is necessary in your applications we recommend to not change
 * the baudrate.
 * 
 * .. versionadded:: 2.3.3$nbsp;(Firmware)
 */
int dc_set_spitfp_baudrate(DC *dc, char bricklet_port, uint32_t baudrate);

/**
 * \ingroup BrickDC
 *
 * Returns the baudrate for a given Bricklet port, see {@link dc_set_spitfp_baudrate}.
 * 
 * .. versionadded:: 2.3.3$nbsp;(Firmware)
 */
int dc_get_spitfp_baudrate(DC *dc, char bricklet_port, uint32_t *ret_baudrate);

/**
 * \ingroup BrickDC
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
 * The errors counts are for errors that occur on the Brick side. All
 * Bricklets have a similar function that returns the errors on the Bricklet side.
 * 
 * .. versionadded:: 2.3.3$nbsp;(Firmware)
 */
int dc_get_spitfp_error_count(DC *dc, char bricklet_port, uint32_t *ret_error_count_ack_checksum, uint32_t *ret_error_count_message_checksum, uint32_t *ret_error_count_frame, uint32_t *ret_error_count_overflow);

/**
 * \ingroup BrickDC
 *
 * Enables the status LED.
 * 
 * The status LED is the blue LED next to the USB connector. If enabled is is
 * on and it flickers if data is transfered. If disabled it is always off.
 * 
 * The default state is enabled.
 * 
 * .. versionadded:: 2.3.1$nbsp;(Firmware)
 */
int dc_enable_status_led(DC *dc);

/**
 * \ingroup BrickDC
 *
 * Disables the status LED.
 * 
 * The status LED is the blue LED next to the USB connector. If enabled is is
 * on and it flickers if data is transfered. If disabled it is always off.
 * 
 * The default state is enabled.
 * 
 * .. versionadded:: 2.3.1$nbsp;(Firmware)
 */
int dc_disable_status_led(DC *dc);

/**
 * \ingroup BrickDC
 *
 * Returns *true* if the status LED is enabled, *false* otherwise.
 * 
 * .. versionadded:: 2.3.1$nbsp;(Firmware)
 */
int dc_is_status_led_enabled(DC *dc, bool *ret_enabled);

/**
 * \ingroup BrickDC
 *
 * Returns the firmware and protocol version and the name of the Bricklet for a
 * given port.
 * 
 * This functions sole purpose is to allow automatic flashing of v1.x.y Bricklet
 * plugins.
 */
int dc_get_protocol1_bricklet_name(DC *dc, char port, uint8_t *ret_protocol_version, uint8_t ret_firmware_version[3], char ret_name[40]);

/**
 * \ingroup BrickDC
 *
 * Returns the temperature as measured inside the microcontroller. The
 * value returned is not the ambient temperature!
 * 
 * The temperature is only proportional to the real temperature and it has an
 * accuracy of ±15%. Practically it is only useful as an indicator for
 * temperature changes.
 */
int dc_get_chip_temperature(DC *dc, int16_t *ret_temperature);

/**
 * \ingroup BrickDC
 *
 * Calling this function will reset the Brick. Calling this function
 * on a Brick inside of a stack will reset the whole stack.
 * 
 * After a reset you have to create new device objects,
 * calling functions on the existing ones will result in
 * undefined behavior!
 */
int dc_reset(DC *dc);

/**
 * \ingroup BrickDC
 *
 * Writes 32 bytes of firmware to the bricklet attached at the given port.
 * The bytes are written to the position offset * 32.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int dc_write_bricklet_plugin(DC *dc, char port, uint8_t offset, uint8_t chunk[32]);

/**
 * \ingroup BrickDC
 *
 * Reads 32 bytes of firmware from the bricklet attached at the given port.
 * The bytes are read starting at the position offset * 32.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int dc_read_bricklet_plugin(DC *dc, char port, uint8_t offset, uint8_t ret_chunk[32]);

/**
 * \ingroup BrickDC
 *
 * Returns the UID, the UID where the Brick is connected to,
 * the position, the hardware and firmware version as well as the
 * device identifier.
 * 
 * The position is the position in the stack from '0' (bottom) to '8' (top).
 * 
 * The device identifier numbers can be found :ref:`here <device_identifier>`.
 * |device_identifier_constant|
 */
int dc_get_identity(DC *dc, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
