/* ***********************************************************
 * This file was automatically generated on 2022-05-11.      *
 *                                                           *
 * C/C++ Bindings Version 2.1.33                             *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generators git repository on tinkerforge.com       *
 *************************************************************/


#define IPCON_EXPOSE_INTERNALS

#include "bricklet_multi_touch_v2.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef void (*TouchState_CallbackFunction)(bool state[13], void *user_data);

#if defined _MSC_VER || defined __BORLANDC__
	#pragma pack(push)
	#pragma pack(1)
	#define ATTRIBUTE_PACKED
#elif defined __GNUC__
	#ifdef _WIN32
		// workaround struct packing bug in GCC 4.7 on Windows
		// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52991
		#define ATTRIBUTE_PACKED __attribute__((gcc_struct, packed))
	#else
		#define ATTRIBUTE_PACKED __attribute__((packed))
	#endif
#else
	#error unknown compiler, do not know how to enable struct packing
#endif

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetTouchState_Request;

typedef struct {
	PacketHeader header;
	uint8_t state[2];
} ATTRIBUTE_PACKED GetTouchState_Response;

typedef struct {
	PacketHeader header;
	uint32_t period;
	uint8_t value_has_to_change;
} ATTRIBUTE_PACKED SetTouchStateCallbackConfiguration_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetTouchStateCallbackConfiguration_Request;

typedef struct {
	PacketHeader header;
	uint32_t period;
	uint8_t value_has_to_change;
} ATTRIBUTE_PACKED GetTouchStateCallbackConfiguration_Response;

typedef struct {
	PacketHeader header;
	uint8_t state[2];
} ATTRIBUTE_PACKED TouchState_Callback;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED Recalibrate_Request;

typedef struct {
	PacketHeader header;
	uint8_t enabled_electrodes[2];
} ATTRIBUTE_PACKED SetElectrodeConfig_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetElectrodeConfig_Request;

typedef struct {
	PacketHeader header;
	uint8_t enabled_electrodes[2];
} ATTRIBUTE_PACKED GetElectrodeConfig_Response;

typedef struct {
	PacketHeader header;
	uint8_t sensitivity;
} ATTRIBUTE_PACKED SetElectrodeSensitivity_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetElectrodeSensitivity_Request;

typedef struct {
	PacketHeader header;
	uint8_t sensitivity;
} ATTRIBUTE_PACKED GetElectrodeSensitivity_Response;

typedef struct {
	PacketHeader header;
	uint8_t config;
} ATTRIBUTE_PACKED SetTouchLEDConfig_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetTouchLEDConfig_Request;

typedef struct {
	PacketHeader header;
	uint8_t config;
} ATTRIBUTE_PACKED GetTouchLEDConfig_Response;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetSPITFPErrorCount_Request;

typedef struct {
	PacketHeader header;
	uint32_t error_count_ack_checksum;
	uint32_t error_count_message_checksum;
	uint32_t error_count_frame;
	uint32_t error_count_overflow;
} ATTRIBUTE_PACKED GetSPITFPErrorCount_Response;

typedef struct {
	PacketHeader header;
	uint8_t mode;
} ATTRIBUTE_PACKED SetBootloaderMode_Request;

typedef struct {
	PacketHeader header;
	uint8_t status;
} ATTRIBUTE_PACKED SetBootloaderMode_Response;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetBootloaderMode_Request;

typedef struct {
	PacketHeader header;
	uint8_t mode;
} ATTRIBUTE_PACKED GetBootloaderMode_Response;

typedef struct {
	PacketHeader header;
	uint32_t pointer;
} ATTRIBUTE_PACKED SetWriteFirmwarePointer_Request;

typedef struct {
	PacketHeader header;
	uint8_t data[64];
} ATTRIBUTE_PACKED WriteFirmware_Request;

typedef struct {
	PacketHeader header;
	uint8_t status;
} ATTRIBUTE_PACKED WriteFirmware_Response;

typedef struct {
	PacketHeader header;
	uint8_t config;
} ATTRIBUTE_PACKED SetStatusLEDConfig_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetStatusLEDConfig_Request;

typedef struct {
	PacketHeader header;
	uint8_t config;
} ATTRIBUTE_PACKED GetStatusLEDConfig_Response;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetChipTemperature_Request;

typedef struct {
	PacketHeader header;
	int16_t temperature;
} ATTRIBUTE_PACKED GetChipTemperature_Response;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED Reset_Request;

typedef struct {
	PacketHeader header;
	uint32_t uid;
} ATTRIBUTE_PACKED WriteUID_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED ReadUID_Request;

typedef struct {
	PacketHeader header;
	uint32_t uid;
} ATTRIBUTE_PACKED ReadUID_Response;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetIdentity_Request;

typedef struct {
	PacketHeader header;
	char uid[8];
	char connected_uid[8];
	char position;
	uint8_t hardware_version[3];
	uint8_t firmware_version[3];
	uint16_t device_identifier;
} ATTRIBUTE_PACKED GetIdentity_Response;

#if defined _MSC_VER || defined __BORLANDC__
	#pragma pack(pop)
#endif
#undef ATTRIBUTE_PACKED

static void multi_touch_v2_callback_wrapper_touch_state(DevicePrivate *device_p, Packet *packet) {
	TouchState_CallbackFunction callback_function;
	void *user_data;
	TouchState_Callback *callback;
	int i;
	bool unpacked_state[13];

	if (packet->header.length != sizeof(TouchState_Callback)) {
		return; // silently ignoring callback with wrong length
	}

	callback_function = (TouchState_CallbackFunction)device_p->registered_callbacks[DEVICE_NUM_FUNCTION_IDS + MULTI_TOUCH_V2_CALLBACK_TOUCH_STATE];
	user_data = device_p->registered_callback_user_data[DEVICE_NUM_FUNCTION_IDS + MULTI_TOUCH_V2_CALLBACK_TOUCH_STATE];
	callback = (TouchState_Callback *)packet;
	(void)callback; // avoid unused variable warning

	if (callback_function == NULL) {
		return;
	}
	for (i = 0; i < 13; i++) unpacked_state[i] = (callback->state[i / 8] & (1 << (i % 8))) != 0;

	callback_function(unpacked_state, user_data);
}

void multi_touch_v2_create(MultiTouchV2 *multi_touch_v2, const char *uid, IPConnection *ipcon) {
	IPConnectionPrivate *ipcon_p = ipcon->p;
	DevicePrivate *device_p;

	device_create(multi_touch_v2, uid, ipcon_p, 2, 0, 0, MULTI_TOUCH_V2_DEVICE_IDENTIFIER);

	device_p = multi_touch_v2->p;

	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_TOUCH_STATE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_SET_TOUCH_STATE_CALLBACK_CONFIGURATION] = DEVICE_RESPONSE_EXPECTED_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_TOUCH_STATE_CALLBACK_CONFIGURATION] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_RECALIBRATE] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_SET_ELECTRODE_CONFIG] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_ELECTRODE_CONFIG] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_SET_ELECTRODE_SENSITIVITY] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_ELECTRODE_SENSITIVITY] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_SET_TOUCH_LED_CONFIG] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_TOUCH_LED_CONFIG] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_SPITFP_ERROR_COUNT] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_SET_BOOTLOADER_MODE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_BOOTLOADER_MODE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_SET_WRITE_FIRMWARE_POINTER] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_WRITE_FIRMWARE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_SET_STATUS_LED_CONFIG] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_STATUS_LED_CONFIG] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_CHIP_TEMPERATURE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_RESET] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_WRITE_UID] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_READ_UID] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MULTI_TOUCH_V2_FUNCTION_GET_IDENTITY] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;

	device_p->callback_wrappers[MULTI_TOUCH_V2_CALLBACK_TOUCH_STATE] = multi_touch_v2_callback_wrapper_touch_state;

	ipcon_add_device(ipcon_p, device_p);
}

void multi_touch_v2_destroy(MultiTouchV2 *multi_touch_v2) {
	device_release(multi_touch_v2->p);
}

int multi_touch_v2_get_response_expected(MultiTouchV2 *multi_touch_v2, uint8_t function_id, bool *ret_response_expected) {
	return device_get_response_expected(multi_touch_v2->p, function_id, ret_response_expected);
}

int multi_touch_v2_set_response_expected(MultiTouchV2 *multi_touch_v2, uint8_t function_id, bool response_expected) {
	return device_set_response_expected(multi_touch_v2->p, function_id, response_expected);
}

int multi_touch_v2_set_response_expected_all(MultiTouchV2 *multi_touch_v2, bool response_expected) {
	return device_set_response_expected_all(multi_touch_v2->p, response_expected);
}

void multi_touch_v2_register_callback(MultiTouchV2 *multi_touch_v2, int16_t callback_id, void (*function)(void), void *user_data) {
	device_register_callback(multi_touch_v2->p, callback_id, function, user_data);
}

int multi_touch_v2_get_api_version(MultiTouchV2 *multi_touch_v2, uint8_t ret_api_version[3]) {
	return device_get_api_version(multi_touch_v2->p, ret_api_version);
}

int multi_touch_v2_get_touch_state(MultiTouchV2 *multi_touch_v2, bool ret_state[13]) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetTouchState_Request request;
	GetTouchState_Response response;
	int ret;
	int i;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_TOUCH_STATE, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	for (i = 0; i < 13; i++) ret_state[i] = (response.state[i / 8] & (1 << (i % 8))) != 0;

	return ret;
}

int multi_touch_v2_set_touch_state_callback_configuration(MultiTouchV2 *multi_touch_v2, uint32_t period, bool value_has_to_change) {
	DevicePrivate *device_p = multi_touch_v2->p;
	SetTouchStateCallbackConfiguration_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_SET_TOUCH_STATE_CALLBACK_CONFIGURATION, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.period = leconvert_uint32_to(period);
	request.value_has_to_change = value_has_to_change ? 1 : 0;

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_get_touch_state_callback_configuration(MultiTouchV2 *multi_touch_v2, uint32_t *ret_period, bool *ret_value_has_to_change) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetTouchStateCallbackConfiguration_Request request;
	GetTouchStateCallbackConfiguration_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_TOUCH_STATE_CALLBACK_CONFIGURATION, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_period = leconvert_uint32_from(response.period);
	*ret_value_has_to_change = response.value_has_to_change != 0;

	return ret;
}

int multi_touch_v2_recalibrate(MultiTouchV2 *multi_touch_v2) {
	DevicePrivate *device_p = multi_touch_v2->p;
	Recalibrate_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_RECALIBRATE, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_set_electrode_config(MultiTouchV2 *multi_touch_v2, bool enabled_electrodes[13]) {
	DevicePrivate *device_p = multi_touch_v2->p;
	SetElectrodeConfig_Request request;
	int ret;
	int i;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_SET_ELECTRODE_CONFIG, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	memset(request.enabled_electrodes, 0, 2); for (i = 0; i < 13; i++) request.enabled_electrodes[i / 8] |= (enabled_electrodes[i] ? 1 : 0) << (i % 8);

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_get_electrode_config(MultiTouchV2 *multi_touch_v2, bool ret_enabled_electrodes[13]) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetElectrodeConfig_Request request;
	GetElectrodeConfig_Response response;
	int ret;
	int i;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_ELECTRODE_CONFIG, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	for (i = 0; i < 13; i++) ret_enabled_electrodes[i] = (response.enabled_electrodes[i / 8] & (1 << (i % 8))) != 0;

	return ret;
}

int multi_touch_v2_set_electrode_sensitivity(MultiTouchV2 *multi_touch_v2, uint8_t sensitivity) {
	DevicePrivate *device_p = multi_touch_v2->p;
	SetElectrodeSensitivity_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_SET_ELECTRODE_SENSITIVITY, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.sensitivity = sensitivity;

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_get_electrode_sensitivity(MultiTouchV2 *multi_touch_v2, uint8_t *ret_sensitivity) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetElectrodeSensitivity_Request request;
	GetElectrodeSensitivity_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_ELECTRODE_SENSITIVITY, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_sensitivity = response.sensitivity;

	return ret;
}

int multi_touch_v2_set_touch_led_config(MultiTouchV2 *multi_touch_v2, uint8_t config) {
	DevicePrivate *device_p = multi_touch_v2->p;
	SetTouchLEDConfig_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_SET_TOUCH_LED_CONFIG, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.config = config;

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_get_touch_led_config(MultiTouchV2 *multi_touch_v2, uint8_t *ret_config) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetTouchLEDConfig_Request request;
	GetTouchLEDConfig_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_TOUCH_LED_CONFIG, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_config = response.config;

	return ret;
}

int multi_touch_v2_get_spitfp_error_count(MultiTouchV2 *multi_touch_v2, uint32_t *ret_error_count_ack_checksum, uint32_t *ret_error_count_message_checksum, uint32_t *ret_error_count_frame, uint32_t *ret_error_count_overflow) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetSPITFPErrorCount_Request request;
	GetSPITFPErrorCount_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_SPITFP_ERROR_COUNT, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_error_count_ack_checksum = leconvert_uint32_from(response.error_count_ack_checksum);
	*ret_error_count_message_checksum = leconvert_uint32_from(response.error_count_message_checksum);
	*ret_error_count_frame = leconvert_uint32_from(response.error_count_frame);
	*ret_error_count_overflow = leconvert_uint32_from(response.error_count_overflow);

	return ret;
}

int multi_touch_v2_set_bootloader_mode(MultiTouchV2 *multi_touch_v2, uint8_t mode, uint8_t *ret_status) {
	DevicePrivate *device_p = multi_touch_v2->p;
	SetBootloaderMode_Request request;
	SetBootloaderMode_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_SET_BOOTLOADER_MODE, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.mode = mode;

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_status = response.status;

	return ret;
}

int multi_touch_v2_get_bootloader_mode(MultiTouchV2 *multi_touch_v2, uint8_t *ret_mode) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetBootloaderMode_Request request;
	GetBootloaderMode_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_BOOTLOADER_MODE, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_mode = response.mode;

	return ret;
}

int multi_touch_v2_set_write_firmware_pointer(MultiTouchV2 *multi_touch_v2, uint32_t pointer) {
	DevicePrivate *device_p = multi_touch_v2->p;
	SetWriteFirmwarePointer_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_SET_WRITE_FIRMWARE_POINTER, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.pointer = leconvert_uint32_to(pointer);

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_write_firmware(MultiTouchV2 *multi_touch_v2, uint8_t data[64], uint8_t *ret_status) {
	DevicePrivate *device_p = multi_touch_v2->p;
	WriteFirmware_Request request;
	WriteFirmware_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_WRITE_FIRMWARE, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	memcpy(request.data, data, 64 * sizeof(uint8_t));

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_status = response.status;

	return ret;
}

int multi_touch_v2_set_status_led_config(MultiTouchV2 *multi_touch_v2, uint8_t config) {
	DevicePrivate *device_p = multi_touch_v2->p;
	SetStatusLEDConfig_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_SET_STATUS_LED_CONFIG, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.config = config;

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_get_status_led_config(MultiTouchV2 *multi_touch_v2, uint8_t *ret_config) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetStatusLEDConfig_Request request;
	GetStatusLEDConfig_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_STATUS_LED_CONFIG, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_config = response.config;

	return ret;
}

int multi_touch_v2_get_chip_temperature(MultiTouchV2 *multi_touch_v2, int16_t *ret_temperature) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetChipTemperature_Request request;
	GetChipTemperature_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_CHIP_TEMPERATURE, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_temperature = leconvert_int16_from(response.temperature);

	return ret;
}

int multi_touch_v2_reset(MultiTouchV2 *multi_touch_v2) {
	DevicePrivate *device_p = multi_touch_v2->p;
	Reset_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_RESET, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_write_uid(MultiTouchV2 *multi_touch_v2, uint32_t uid) {
	DevicePrivate *device_p = multi_touch_v2->p;
	WriteUID_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_WRITE_UID, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.uid = leconvert_uint32_to(uid);

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int multi_touch_v2_read_uid(MultiTouchV2 *multi_touch_v2, uint32_t *ret_uid) {
	DevicePrivate *device_p = multi_touch_v2->p;
	ReadUID_Request request;
	ReadUID_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_READ_UID, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_uid = leconvert_uint32_from(response.uid);

	return ret;
}

int multi_touch_v2_get_identity(MultiTouchV2 *multi_touch_v2, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier) {
	DevicePrivate *device_p = multi_touch_v2->p;
	GetIdentity_Request request;
	GetIdentity_Response response;
	int ret;

	ret = packet_header_create(&request.header, sizeof(request), MULTI_TOUCH_V2_FUNCTION_GET_IDENTITY, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	memcpy(ret_uid, response.uid, 8);
	memcpy(ret_connected_uid, response.connected_uid, 8);
	*ret_position = response.position;
	memcpy(ret_hardware_version, response.hardware_version, 3 * sizeof(uint8_t));
	memcpy(ret_firmware_version, response.firmware_version, 3 * sizeof(uint8_t));
	*ret_device_identifier = leconvert_uint16_from(response.device_identifier);

	return ret;
}

#ifdef __cplusplus
}
#endif
