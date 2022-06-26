/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/

#ifndef BRICK_SERVO_H
#define BRICK_SERVO_H

#include "ip_connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup BrickServo Servo Brick
 */

/**
 * \ingroup BrickServo
 *
 * Drives up to 7 RC Servos with up to 3A
 */
typedef Device Servo;

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_ENABLE 1

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_DISABLE 2

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_IS_ENABLED 3

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_POSITION 4

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_POSITION 5

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_CURRENT_POSITION 6

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_VELOCITY 7

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_VELOCITY 8

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_CURRENT_VELOCITY 9

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_ACCELERATION 10

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_ACCELERATION 11

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_OUTPUT_VOLTAGE 12

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_OUTPUT_VOLTAGE 13

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_PULSE_WIDTH 14

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_PULSE_WIDTH 15

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_DEGREE 16

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_DEGREE 17

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_PERIOD 18

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_PERIOD 19

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_SERVO_CURRENT 20

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_OVERALL_CURRENT 21

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_STACK_INPUT_VOLTAGE 22

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_EXTERNAL_INPUT_VOLTAGE 23

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_MINIMUM_VOLTAGE 24

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_MINIMUM_VOLTAGE 25

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_ENABLE_POSITION_REACHED_CALLBACK 29

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_DISABLE_POSITION_REACHED_CALLBACK 30

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_IS_POSITION_REACHED_CALLBACK_ENABLED 31

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_ENABLE_VELOCITY_REACHED_CALLBACK 32

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_DISABLE_VELOCITY_REACHED_CALLBACK 33

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_IS_VELOCITY_REACHED_CALLBACK_ENABLED 34

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_SPITFP_BAUDRATE_CONFIG 231

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_SPITFP_BAUDRATE_CONFIG 232

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_SEND_TIMEOUT_COUNT 233

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_SET_SPITFP_BAUDRATE 234

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_SPITFP_BAUDRATE 235

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_SPITFP_ERROR_COUNT 237

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_ENABLE_STATUS_LED 238

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_DISABLE_STATUS_LED 239

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_IS_STATUS_LED_ENABLED 240

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_PROTOCOL1_BRICKLET_NAME 241

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_CHIP_TEMPERATURE 242

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_RESET 243

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_WRITE_BRICKLET_PLUGIN 246

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_READ_BRICKLET_PLUGIN 247

/**
 * \ingroup BrickServo
 */
#define SERVO_FUNCTION_GET_IDENTITY 255

/**
 * \ingroup BrickServo
 *
 * Signature: \code void callback(uint16_t voltage, void *user_data) \endcode
 * 
 * This callback is triggered when the input voltage drops below the value set by
 * {@link servo_set_minimum_voltage}. The parameter is the current voltage.
 */
#define SERVO_CALLBACK_UNDER_VOLTAGE 26

/**
 * \ingroup BrickServo
 *
 * Signature: \code void callback(uint8_t servo_num, int16_t position, void *user_data) \endcode
 * 
 * This callback is triggered when a position set by {@link servo_set_position}
 * is reached. If the new position matches the current position then the
 * callback is not triggered, because the servo didn't move.
 * The parameters are the servo and the position that is reached.
 * 
 * You can enable this callback with {@link servo_enable_position_reached_callback}.
 * 
 * \note
 *  Since we can't get any feedback from the servo, this only works if the
 *  velocity (see {@link servo_set_velocity}) is set smaller or equal to the
 *  maximum velocity of the servo. Otherwise the servo will lag behind the
 *  control value and the callback will be triggered too early.
 */
#define SERVO_CALLBACK_POSITION_REACHED 27

/**
 * \ingroup BrickServo
 *
 * Signature: \code void callback(uint8_t servo_num, int16_t velocity, void *user_data) \endcode
 * 
 * This callback is triggered when a velocity set by {@link servo_set_velocity}
 * is reached. The parameters are the servo and the velocity that is reached.
 * 
 * You can enable this callback with {@link servo_enable_velocity_reached_callback}.
 * 
 * \note
 *  Since we can't get any feedback from the servo, this only works if the
 *  acceleration (see {@link servo_set_acceleration}) is set smaller or equal to the
 *  maximum acceleration of the servo. Otherwise the servo will lag behind the
 *  control value and the callback will be triggered too early.
 */
#define SERVO_CALLBACK_VELOCITY_REACHED 28


/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_NONE 0

/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_USB 1

/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_SPI_STACK 2

/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_CHIBI 3

/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_RS485 4

/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_WIFI 5

/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_ETHERNET 6

/**
 * \ingroup BrickServo
 */
#define SERVO_COMMUNICATION_METHOD_WIFI_V2 7

/**
 * \ingroup BrickServo
 *
 * This constant is used to identify a Servo Brick.
 *
 * The {@link servo_get_identity} function and the
 * {@link IPCON_CALLBACK_ENUMERATE} callback of the IP Connection have a
 * \c device_identifier parameter to specify the Brick's or Bricklet's type.
 */
#define SERVO_DEVICE_IDENTIFIER 14

/**
 * \ingroup BrickServo
 *
 * This constant represents the display name of a Servo Brick.
 */
#define SERVO_DEVICE_DISPLAY_NAME "Servo Brick"

/**
 * \ingroup BrickServo
 *
 * Creates the device object \c servo with the unique device ID \c uid and adds
 * it to the IPConnection \c ipcon.
 */
void servo_create(Servo *servo, const char *uid, IPConnection *ipcon);

/**
 * \ingroup BrickServo
 *
 * Removes the device object \c servo from its IPConnection and destroys it.
 * The device object cannot be used anymore afterwards.
 */
void servo_destroy(Servo *servo);

/**
 * \ingroup BrickServo
 *
 * Returns the response expected flag for the function specified by the
 * \c function_id parameter. It is *true* if the function is expected to
 * send a response, *false* otherwise.
 *
 * For getter functions this is enabled by default and cannot be disabled,
 * because those functions will always send a response. For callback
 * configuration functions it is enabled by default too, but can be disabled
 * via the servo_set_response_expected function. For setter functions it is
 * disabled by default and can be enabled.
 *
 * Enabling the response expected flag for a setter function allows to
 * detect timeouts and other error conditions calls of this setter as well.
 * The device will then send a response for this purpose. If this flag is
 * disabled for a setter function then no response is sent and errors are
 * silently ignored, because they cannot be detected.
 */
int servo_get_response_expected(Servo *servo, uint8_t function_id, bool *ret_response_expected);

/**
 * \ingroup BrickServo
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
int servo_set_response_expected(Servo *servo, uint8_t function_id, bool response_expected);

/**
 * \ingroup BrickServo
 *
 * Changes the response expected flag for all setter and callback configuration
 * functions of this device at once.
 */
int servo_set_response_expected_all(Servo *servo, bool response_expected);

/**
 * \ingroup BrickServo
 *
 * Registers the given \c function with the given \c callback_id. The
 * \c user_data will be passed as the last parameter to the \c function.
 */
void servo_register_callback(Servo *servo, int16_t callback_id, void (*function)(void), void *user_data);

/**
 * \ingroup BrickServo
 *
 * Returns the API version (major, minor, release) of the bindings for this
 * device.
 */
int servo_get_api_version(Servo *servo, uint8_t ret_api_version[3]);

/**
 * \ingroup BrickServo
 *
 * Enables a servo (0 to 6). If a servo is enabled, the configured position,
 * velocity, acceleration, etc. are applied immediately.
 */
int servo_enable(Servo *servo, uint8_t servo_num);

/**
 * \ingroup BrickServo
 *
 * Disables a servo (0 to 6). Disabled servos are not driven at all, i.e. a
 * disabled servo will not hold its position if a load is applied.
 */
int servo_disable(Servo *servo, uint8_t servo_num);

/**
 * \ingroup BrickServo
 *
 * Returns *true* if the specified servo is enabled, *false* otherwise.
 */
int servo_is_enabled(Servo *servo, uint8_t servo_num, bool *ret_enabled);

/**
 * \ingroup BrickServo
 *
 * Sets the position for the specified servo.
 * 
 * The default range of the position is -9000 to 9000, but it can be specified
 * according to your servo with {@link servo_set_degree}.
 * 
 * If you want to control a linear servo or RC brushless motor controller or
 * similar with the Servo Brick, you can also define lengths or speeds with
 * {@link servo_set_degree}.
 */
int servo_set_position(Servo *servo, uint8_t servo_num, int16_t position);

/**
 * \ingroup BrickServo
 *
 * Returns the position of the specified servo as set by {@link servo_set_position}.
 */
int servo_get_position(Servo *servo, uint8_t servo_num, int16_t *ret_position);

/**
 * \ingroup BrickServo
 *
 * Returns the *current* position of the specified servo. This may not be the
 * value of {@link servo_set_position} if the servo is currently approaching a
 * position goal.
 */
int servo_get_current_position(Servo *servo, uint8_t servo_num, int16_t *ret_position);

/**
 * \ingroup BrickServo
 *
 * Sets the maximum velocity of the specified servo. The velocity
 * is accelerated according to the value set by {@link servo_set_acceleration}.
 * 
 * The minimum velocity is 0 (no movement) and the maximum velocity is 65535.
 * With a value of 65535 the position will be set immediately (no velocity).
 */
int servo_set_velocity(Servo *servo, uint8_t servo_num, uint16_t velocity);

/**
 * \ingroup BrickServo
 *
 * Returns the velocity of the specified servo as set by {@link servo_set_velocity}.
 */
int servo_get_velocity(Servo *servo, uint8_t servo_num, uint16_t *ret_velocity);

/**
 * \ingroup BrickServo
 *
 * Returns the *current* velocity of the specified servo. This may not be the
 * value of {@link servo_set_velocity} if the servo is currently approaching a
 * velocity goal.
 */
int servo_get_current_velocity(Servo *servo, uint8_t servo_num, uint16_t *ret_velocity);

/**
 * \ingroup BrickServo
 *
 * Sets the acceleration of the specified servo.
 * 
 * The minimum acceleration is 1 and the maximum acceleration is 65535.
 * With a value of 65535 the velocity will be set immediately (no acceleration).
 */
int servo_set_acceleration(Servo *servo, uint8_t servo_num, uint16_t acceleration);

/**
 * \ingroup BrickServo
 *
 * Returns the acceleration for the specified servo as set by
 * {@link servo_set_acceleration}.
 */
int servo_get_acceleration(Servo *servo, uint8_t servo_num, uint16_t *ret_acceleration);

/**
 * \ingroup BrickServo
 *
 * Sets the output voltages with which the servos are driven.
 * 
 * \note
 *  We recommend that you set this value to the maximum voltage that is
 *  specified for your servo, most servos achieve their maximum force only
 *  with high voltages.
 */
int servo_set_output_voltage(Servo *servo, uint16_t voltage);

/**
 * \ingroup BrickServo
 *
 * Returns the output voltage as specified by {@link servo_set_output_voltage}.
 */
int servo_get_output_voltage(Servo *servo, uint16_t *ret_voltage);

/**
 * \ingroup BrickServo
 *
 * Sets the minimum and maximum pulse width of the specified servo.
 * 
 * Usually, servos are controlled with a
 * `PWM <https://en.wikipedia.org/wiki/Pulse-width_modulation>`__, whereby the
 * length of the pulse controls the position of the servo. Every servo has
 * different minimum and maximum pulse widths, these can be specified with
 * this function.
 * 
 * If you have a datasheet for your servo that specifies the minimum and
 * maximum pulse width, you should set the values accordingly. If your servo
 * comes without any datasheet you have to find the values via trial and error.
 * 
 * The minimum must be smaller than the maximum.
 */
int servo_set_pulse_width(Servo *servo, uint8_t servo_num, uint16_t min, uint16_t max);

/**
 * \ingroup BrickServo
 *
 * Returns the minimum and maximum pulse width for the specified servo as set by
 * {@link servo_set_pulse_width}.
 */
int servo_get_pulse_width(Servo *servo, uint8_t servo_num, uint16_t *ret_min, uint16_t *ret_max);

/**
 * \ingroup BrickServo
 *
 * Sets the minimum and maximum degree for the specified servo (by default
 * given as °/100).
 * 
 * This only specifies the abstract values between which the minimum and maximum
 * pulse width is scaled. For example: If you specify a pulse width of 1000µs
 * to 2000µs and a degree range of -90° to 90°, a call of {@link servo_set_position}
 * with 0 will result in a pulse width of 1500µs
 * (-90° = 1000µs, 90° = 2000µs, etc.).
 * 
 * Possible usage:
 * 
 * * The datasheet of your servo specifies a range of 200° with the middle position
 *   at 110°. In this case you can set the minimum to -9000 and the maximum to 11000.
 * * You measure a range of 220° on your servo and you don't have or need a middle
 *   position. In this case you can set the minimum to 0 and the maximum to 22000.
 * * You have a linear servo with a drive length of 20cm, In this case you could
 *   set the minimum to 0 and the maximum to 20000. Now you can set the Position
 *   with {@link servo_set_position} with a resolution of cm/100. Also the velocity will
 *   have a resolution of cm/100s and the acceleration will have a resolution of
 *   cm/100s².
 * * You don't care about units and just want the highest possible resolution. In
 *   this case you should set the minimum to -32767 and the maximum to 32767.
 * * You have a brushless motor with a maximum speed of 10000 rpm and want to
 *   control it with a RC brushless motor controller. In this case you can set the
 *   minimum to 0 and the maximum to 10000. {@link servo_set_position} now controls the rpm.
 * 
 * The minimum must be smaller than the maximum.
 */
int servo_set_degree(Servo *servo, uint8_t servo_num, int16_t min, int16_t max);

/**
 * \ingroup BrickServo
 *
 * Returns the minimum and maximum degree for the specified servo as set by
 * {@link servo_set_degree}.
 */
int servo_get_degree(Servo *servo, uint8_t servo_num, int16_t *ret_min, int16_t *ret_max);

/**
 * \ingroup BrickServo
 *
 * Sets the period of the specified servo.
 * 
 * Usually, servos are controlled with a
 * `PWM <https://en.wikipedia.org/wiki/Pulse-width_modulation>`__. Different
 * servos expect PWMs with different periods. Most servos run well with a
 * period of about 20ms.
 * 
 * If your servo comes with a datasheet that specifies a period, you should
 * set it accordingly. If you don't have a datasheet and you have no idea
 * what the correct period is, the default value will most likely
 * work fine.
 */
int servo_set_period(Servo *servo, uint8_t servo_num, uint16_t period);

/**
 * \ingroup BrickServo
 *
 * Returns the period for the specified servo as set by {@link servo_set_period}.
 */
int servo_get_period(Servo *servo, uint8_t servo_num, uint16_t *ret_period);

/**
 * \ingroup BrickServo
 *
 * Returns the current consumption of the specified servo.
 */
int servo_get_servo_current(Servo *servo, uint8_t servo_num, uint16_t *ret_current);

/**
 * \ingroup BrickServo
 *
 * Returns the current consumption of all servos together.
 */
int servo_get_overall_current(Servo *servo, uint16_t *ret_current);

/**
 * \ingroup BrickServo
 *
 * Returns the stack input voltage. The stack input voltage is the
 * voltage that is supplied via the stack, i.e. it is given by a
 * Step-Down or Step-Up Power Supply.
 */
int servo_get_stack_input_voltage(Servo *servo, uint16_t *ret_voltage);

/**
 * \ingroup BrickServo
 *
 * Returns the external input voltage. The external input voltage is
 * given via the black power input connector on the Servo Brick.
 * 
 * If there is an external input voltage and a stack input voltage, the motors
 * will be driven by the external input voltage. If there is only a stack
 * voltage present, the motors will be driven by this voltage.
 * 
 * \warning
 *  This means, if you have a high stack voltage and a low external voltage,
 *  the motors will be driven with the low external voltage. If you then remove
 *  the external connection, it will immediately be driven by the high
 *  stack voltage
 */
int servo_get_external_input_voltage(Servo *servo, uint16_t *ret_voltage);

/**
 * \ingroup BrickServo
 *
 * Sets the minimum voltage, below which the {@link SERVO_CALLBACK_UNDER_VOLTAGE} callback
 * is triggered. The minimum possible value that works with the Servo Brick is 5V.
 * You can use this function to detect the discharge of a battery that is used
 * to drive the stepper motor. If you have a fixed power supply, you likely do
 * not need this functionality.
 */
int servo_set_minimum_voltage(Servo *servo, uint16_t voltage);

/**
 * \ingroup BrickServo
 *
 * Returns the minimum voltage as set by {@link servo_set_minimum_voltage}
 */
int servo_get_minimum_voltage(Servo *servo, uint16_t *ret_voltage);

/**
 * \ingroup BrickServo
 *
 * Enables the {@link SERVO_CALLBACK_POSITION_REACHED} callback.
 * 
 * Default is disabled.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Firmware)
 */
int servo_enable_position_reached_callback(Servo *servo);

/**
 * \ingroup BrickServo
 *
 * Disables the {@link SERVO_CALLBACK_POSITION_REACHED} callback.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Firmware)
 */
int servo_disable_position_reached_callback(Servo *servo);

/**
 * \ingroup BrickServo
 *
 * Returns *true* if {@link SERVO_CALLBACK_POSITION_REACHED} callback is enabled, *false* otherwise.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Firmware)
 */
int servo_is_position_reached_callback_enabled(Servo *servo, bool *ret_enabled);

/**
 * \ingroup BrickServo
 *
 * Enables the {@link SERVO_CALLBACK_VELOCITY_REACHED} callback.
 * 
 * Default is disabled.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Firmware)
 */
int servo_enable_velocity_reached_callback(Servo *servo);

/**
 * \ingroup BrickServo
 *
 * Disables the {@link SERVO_CALLBACK_VELOCITY_REACHED} callback.
 * 
 * Default is disabled.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Firmware)
 */
int servo_disable_velocity_reached_callback(Servo *servo);

/**
 * \ingroup BrickServo
 *
 * Returns *true* if {@link SERVO_CALLBACK_VELOCITY_REACHED} callback is enabled, *false* otherwise.
 * 
 * .. versionadded:: 2.0.1$nbsp;(Firmware)
 */
int servo_is_velocity_reached_callback_enabled(Servo *servo, bool *ret_enabled);

/**
 * \ingroup BrickServo
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
 * {@link servo_set_spitfp_baudrate}. If the dynamic baudrate is disabled, the baudrate
 * as set by {@link servo_set_spitfp_baudrate} will be used statically.
 * 
 * .. versionadded:: 2.3.4$nbsp;(Firmware)
 */
int servo_set_spitfp_baudrate_config(Servo *servo, bool enable_dynamic_baudrate, uint32_t minimum_dynamic_baudrate);

/**
 * \ingroup BrickServo
 *
 * Returns the baudrate config, see {@link servo_set_spitfp_baudrate_config}.
 * 
 * .. versionadded:: 2.3.4$nbsp;(Firmware)
 */
int servo_get_spitfp_baudrate_config(Servo *servo, bool *ret_enable_dynamic_baudrate, uint32_t *ret_minimum_dynamic_baudrate);

/**
 * \ingroup BrickServo
 *
 * Returns the timeout count for the different communication methods.
 * 
 * The methods 0-2 are available for all Bricks, 3-7 only for Master Bricks.
 * 
 * This function is mostly used for debugging during development, in normal operation
 * the counters should nearly always stay at 0.
 * 
 * .. versionadded:: 2.3.2$nbsp;(Firmware)
 */
int servo_get_send_timeout_count(Servo *servo, uint8_t communication_method, uint32_t *ret_timeout_count);

/**
 * \ingroup BrickServo
 *
 * Sets the baudrate for a specific Bricklet port.
 * 
 * If you want to increase the throughput of Bricklets you can increase
 * the baudrate. If you get a high error count because of high
 * interference (see {@link servo_get_spitfp_error_count}) you can decrease the
 * baudrate.
 * 
 * If the dynamic baudrate feature is enabled, the baudrate set by this
 * function corresponds to the maximum baudrate (see {@link servo_set_spitfp_baudrate_config}).
 * 
 * Regulatory testing is done with the default baudrate. If CE compatibility
 * or similar is necessary in your applications we recommend to not change
 * the baudrate.
 * 
 * .. versionadded:: 2.3.2$nbsp;(Firmware)
 */
int servo_set_spitfp_baudrate(Servo *servo, char bricklet_port, uint32_t baudrate);

/**
 * \ingroup BrickServo
 *
 * Returns the baudrate for a given Bricklet port, see {@link servo_set_spitfp_baudrate}.
 * 
 * .. versionadded:: 2.3.2$nbsp;(Firmware)
 */
int servo_get_spitfp_baudrate(Servo *servo, char bricklet_port, uint32_t *ret_baudrate);

/**
 * \ingroup BrickServo
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
 * .. versionadded:: 2.3.2$nbsp;(Firmware)
 */
int servo_get_spitfp_error_count(Servo *servo, char bricklet_port, uint32_t *ret_error_count_ack_checksum, uint32_t *ret_error_count_message_checksum, uint32_t *ret_error_count_frame, uint32_t *ret_error_count_overflow);

/**
 * \ingroup BrickServo
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
int servo_enable_status_led(Servo *servo);

/**
 * \ingroup BrickServo
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
int servo_disable_status_led(Servo *servo);

/**
 * \ingroup BrickServo
 *
 * Returns *true* if the status LED is enabled, *false* otherwise.
 * 
 * .. versionadded:: 2.3.1$nbsp;(Firmware)
 */
int servo_is_status_led_enabled(Servo *servo, bool *ret_enabled);

/**
 * \ingroup BrickServo
 *
 * Returns the firmware and protocol version and the name of the Bricklet for a
 * given port.
 * 
 * This functions sole purpose is to allow automatic flashing of v1.x.y Bricklet
 * plugins.
 */
int servo_get_protocol1_bricklet_name(Servo *servo, char port, uint8_t *ret_protocol_version, uint8_t ret_firmware_version[3], char ret_name[40]);

/**
 * \ingroup BrickServo
 *
 * Returns the temperature as measured inside the microcontroller. The
 * value returned is not the ambient temperature!
 * 
 * The temperature is only proportional to the real temperature and it has an
 * accuracy of ±15%. Practically it is only useful as an indicator for
 * temperature changes.
 */
int servo_get_chip_temperature(Servo *servo, int16_t *ret_temperature);

/**
 * \ingroup BrickServo
 *
 * Calling this function will reset the Brick. Calling this function
 * on a Brick inside of a stack will reset the whole stack.
 * 
 * After a reset you have to create new device objects,
 * calling functions on the existing ones will result in
 * undefined behavior!
 */
int servo_reset(Servo *servo);

/**
 * \ingroup BrickServo
 *
 * Writes 32 bytes of firmware to the bricklet attached at the given port.
 * The bytes are written to the position offset * 32.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int servo_write_bricklet_plugin(Servo *servo, char port, uint8_t offset, uint8_t chunk[32]);

/**
 * \ingroup BrickServo
 *
 * Reads 32 bytes of firmware from the bricklet attached at the given port.
 * The bytes are read starting at the position offset * 32.
 * 
 * This function is used by Brick Viewer during flashing. It should not be
 * necessary to call it in a normal user program.
 */
int servo_read_bricklet_plugin(Servo *servo, char port, uint8_t offset, uint8_t ret_chunk[32]);

/**
 * \ingroup BrickServo
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
int servo_get_identity(Servo *servo, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier);

#ifdef __cplusplus
}
#endif

#endif
