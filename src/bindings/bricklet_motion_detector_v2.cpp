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

#include "bricklet_motion_detector_v2.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef void (*MotionDetected_CallbackFunction)(void *user_data);

typedef void (*DetectionCycleEnded_CallbackFunction)(void *user_data);

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
} ATTRIBUTE_PACKED GetMotionDetected_Request;

typedef struct {
	PacketHeader header;
	uint8_t motion;
} ATTRIBUTE_PACKED GetMotionDetected_Response;

typedef struct {
	PacketHeader header;
	uint8_t sensitivity;
} ATTRIBUTE_PACKED SetSensitivity_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetSensitivity_Request;

typedef struct {
	PacketHeader header;
	uint8_t sensitivity;
} ATTRIBUTE_PACKED GetSensitivity_Response;

typedef struct {
	PacketHeader header;
	uint8_t top_left;
	uint8_t top_right;
	uint8_t bottom;
} ATTRIBUTE_PACKED SetIndicator_Request;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED GetIndicator_Request;

typedef struct {
	PacketHeader header;
	uint8_t top_left;
	uint8_t top_right;
	uint8_t bottom;
} ATTRIBUTE_PACKED GetIndicator_Response;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED MotionDetected_Callback;

typedef struct {
	PacketHeader header;
} ATTRIBUTE_PACKED DetectionCycleEnded_Callback;

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

static void motion_detector_v2_callback_wrapper_motion_detected(DevicePrivate *device_p, Packet *packet) {
	MotionDetected_CallbackFunction callback_function;
	void *user_data;
	MotionDetected_Callback *callback;

	if (packet->header.length != sizeof(MotionDetected_Callback)) {
		return; // silently ignoring callback with wrong length
	}

	callback_function = (MotionDetected_CallbackFunction)device_p->registered_callbacks[DEVICE_NUM_FUNCTION_IDS + MOTION_DETECTOR_V2_CALLBACK_MOTION_DETECTED];
	user_data = device_p->registered_callback_user_data[DEVICE_NUM_FUNCTION_IDS + MOTION_DETECTOR_V2_CALLBACK_MOTION_DETECTED];
	callback = (MotionDetected_Callback *)packet;
	(void)callback; // avoid unused variable warning

	if (callback_function == NULL) {
		return;
	}

	callback_function(user_data);
}

static void motion_detector_v2_callback_wrapper_detection_cycle_ended(DevicePrivate *device_p, Packet *packet) {
	DetectionCycleEnded_CallbackFunction callback_function;
	void *user_data;
	DetectionCycleEnded_Callback *callback;

	if (packet->header.length != sizeof(DetectionCycleEnded_Callback)) {
		return; // silently ignoring callback with wrong length
	}

	callback_function = (DetectionCycleEnded_CallbackFunction)device_p->registered_callbacks[DEVICE_NUM_FUNCTION_IDS + MOTION_DETECTOR_V2_CALLBACK_DETECTION_CYCLE_ENDED];
	user_data = device_p->registered_callback_user_data[DEVICE_NUM_FUNCTION_IDS + MOTION_DETECTOR_V2_CALLBACK_DETECTION_CYCLE_ENDED];
	callback = (DetectionCycleEnded_Callback *)packet;
	(void)callback; // avoid unused variable warning

	if (callback_function == NULL) {
		return;
	}

	callback_function(user_data);
}

void motion_detector_v2_create(MotionDetectorV2 *motion_detector_v2, const char *uid, IPConnection *ipcon) {
	IPConnectionPrivate *ipcon_p = ipcon->p;
	DevicePrivate *device_p;

	device_create(motion_detector_v2, uid, ipcon_p, 2, 0, 0, MOTION_DETECTOR_V2_DEVICE_IDENTIFIER);

	device_p = motion_detector_v2->p;

	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_MOTION_DETECTED] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_SET_SENSITIVITY] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_SENSITIVITY] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_SET_INDICATOR] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_INDICATOR] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_SPITFP_ERROR_COUNT] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_SET_BOOTLOADER_MODE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_BOOTLOADER_MODE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_SET_WRITE_FIRMWARE_POINTER] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_WRITE_FIRMWARE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_SET_STATUS_LED_CONFIG] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_STATUS_LED_CONFIG] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_CHIP_TEMPERATURE] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_RESET] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_WRITE_UID] = DEVICE_RESPONSE_EXPECTED_FALSE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_READ_UID] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;
	device_p->response_expected[MOTION_DETECTOR_V2_FUNCTION_GET_IDENTITY] = DEVICE_RESPONSE_EXPECTED_ALWAYS_TRUE;

	device_p->callback_wrappers[MOTION_DETECTOR_V2_CALLBACK_MOTION_DETECTED] = motion_detector_v2_callback_wrapper_motion_detected;
	device_p->callback_wrappers[MOTION_DETECTOR_V2_CALLBACK_DETECTION_CYCLE_ENDED] = motion_detector_v2_callback_wrapper_detection_cycle_ended;

	ipcon_add_device(ipcon_p, device_p);
}

void motion_detector_v2_destroy(MotionDetectorV2 *motion_detector_v2) {
	device_release(motion_detector_v2->p);
}

int motion_detector_v2_get_response_expected(MotionDetectorV2 *motion_detector_v2, uint8_t function_id, bool *ret_response_expected) {
	return device_get_response_expected(motion_detector_v2->p, function_id, ret_response_expected);
}

int motion_detector_v2_set_response_expected(MotionDetectorV2 *motion_detector_v2, uint8_t function_id, bool response_expected) {
	return device_set_response_expected(motion_detector_v2->p, function_id, response_expected);
}

int motion_detector_v2_set_response_expected_all(MotionDetectorV2 *motion_detector_v2, bool response_expected) {
	return device_set_response_expected_all(motion_detector_v2->p, response_expected);
}

void motion_detector_v2_register_callback(MotionDetectorV2 *motion_detector_v2, int16_t callback_id, void (*function)(void), void *user_data) {
	device_register_callback(motion_detector_v2->p, callback_id, function, user_data);
}

int motion_detector_v2_get_api_version(MotionDetectorV2 *motion_detector_v2, uint8_t ret_api_version[3]) {
	return device_get_api_version(motion_detector_v2->p, ret_api_version);
}

int motion_detector_v2_get_motion_detected(MotionDetectorV2 *motion_detector_v2, uint8_t *ret_motion) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetMotionDetected_Request request;
	GetMotionDetected_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_MOTION_DETECTED, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_motion = response.motion;

	return ret;
}

int motion_detector_v2_set_sensitivity(MotionDetectorV2 *motion_detector_v2, uint8_t sensitivity) {
	DevicePrivate *device_p = motion_detector_v2->p;
	SetSensitivity_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_SET_SENSITIVITY, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.sensitivity = sensitivity;

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int motion_detector_v2_get_sensitivity(MotionDetectorV2 *motion_detector_v2, uint8_t *ret_sensitivity) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetSensitivity_Request request;
	GetSensitivity_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_SENSITIVITY, device_p->ipcon_p, device_p);

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

int motion_detector_v2_set_indicator(MotionDetectorV2 *motion_detector_v2, uint8_t top_left, uint8_t top_right, uint8_t bottom) {
	DevicePrivate *device_p = motion_detector_v2->p;
	SetIndicator_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_SET_INDICATOR, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.top_left = top_left;
	request.top_right = top_right;
	request.bottom = bottom;

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int motion_detector_v2_get_indicator(MotionDetectorV2 *motion_detector_v2, uint8_t *ret_top_left, uint8_t *ret_top_right, uint8_t *ret_bottom) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetIndicator_Request request;
	GetIndicator_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_INDICATOR, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, (Packet *)&response, sizeof(response));

	if (ret < 0) {
		return ret;
	}

	*ret_top_left = response.top_left;
	*ret_top_right = response.top_right;
	*ret_bottom = response.bottom;

	return ret;
}

int motion_detector_v2_get_spitfp_error_count(MotionDetectorV2 *motion_detector_v2, uint32_t *ret_error_count_ack_checksum, uint32_t *ret_error_count_message_checksum, uint32_t *ret_error_count_frame, uint32_t *ret_error_count_overflow) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetSPITFPErrorCount_Request request;
	GetSPITFPErrorCount_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_SPITFP_ERROR_COUNT, device_p->ipcon_p, device_p);

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

int motion_detector_v2_set_bootloader_mode(MotionDetectorV2 *motion_detector_v2, uint8_t mode, uint8_t *ret_status) {
	DevicePrivate *device_p = motion_detector_v2->p;
	SetBootloaderMode_Request request;
	SetBootloaderMode_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_SET_BOOTLOADER_MODE, device_p->ipcon_p, device_p);

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

int motion_detector_v2_get_bootloader_mode(MotionDetectorV2 *motion_detector_v2, uint8_t *ret_mode) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetBootloaderMode_Request request;
	GetBootloaderMode_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_BOOTLOADER_MODE, device_p->ipcon_p, device_p);

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

int motion_detector_v2_set_write_firmware_pointer(MotionDetectorV2 *motion_detector_v2, uint32_t pointer) {
	DevicePrivate *device_p = motion_detector_v2->p;
	SetWriteFirmwarePointer_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_SET_WRITE_FIRMWARE_POINTER, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.pointer = leconvert_uint32_to(pointer);

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int motion_detector_v2_write_firmware(MotionDetectorV2 *motion_detector_v2, uint8_t data[64], uint8_t *ret_status) {
	DevicePrivate *device_p = motion_detector_v2->p;
	WriteFirmware_Request request;
	WriteFirmware_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_WRITE_FIRMWARE, device_p->ipcon_p, device_p);

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

int motion_detector_v2_set_status_led_config(MotionDetectorV2 *motion_detector_v2, uint8_t config) {
	DevicePrivate *device_p = motion_detector_v2->p;
	SetStatusLEDConfig_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_SET_STATUS_LED_CONFIG, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.config = config;

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int motion_detector_v2_get_status_led_config(MotionDetectorV2 *motion_detector_v2, uint8_t *ret_config) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetStatusLEDConfig_Request request;
	GetStatusLEDConfig_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_STATUS_LED_CONFIG, device_p->ipcon_p, device_p);

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

int motion_detector_v2_get_chip_temperature(MotionDetectorV2 *motion_detector_v2, int16_t *ret_temperature) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetChipTemperature_Request request;
	GetChipTemperature_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_CHIP_TEMPERATURE, device_p->ipcon_p, device_p);

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

int motion_detector_v2_reset(MotionDetectorV2 *motion_detector_v2) {
	DevicePrivate *device_p = motion_detector_v2->p;
	Reset_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_RESET, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int motion_detector_v2_write_uid(MotionDetectorV2 *motion_detector_v2, uint32_t uid) {
	DevicePrivate *device_p = motion_detector_v2->p;
	WriteUID_Request request;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_WRITE_UID, device_p->ipcon_p, device_p);

	if (ret < 0) {
		return ret;
	}

	request.uid = leconvert_uint32_to(uid);

	ret = device_send_request(device_p, (Packet *)&request, NULL, 0);

	return ret;
}

int motion_detector_v2_read_uid(MotionDetectorV2 *motion_detector_v2, uint32_t *ret_uid) {
	DevicePrivate *device_p = motion_detector_v2->p;
	ReadUID_Request request;
	ReadUID_Response response;
	int ret;

	ret = device_check_validity(device_p);

	if (ret < 0) {
		return ret;
	}

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_READ_UID, device_p->ipcon_p, device_p);

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

int motion_detector_v2_get_identity(MotionDetectorV2 *motion_detector_v2, char ret_uid[8], char ret_connected_uid[8], char *ret_position, uint8_t ret_hardware_version[3], uint8_t ret_firmware_version[3], uint16_t *ret_device_identifier) {
	DevicePrivate *device_p = motion_detector_v2->p;
	GetIdentity_Request request;
	GetIdentity_Response response;
	int ret;

	ret = packet_header_create(&request.header, sizeof(request), MOTION_DETECTOR_V2_FUNCTION_GET_IDENTITY, device_p->ipcon_p, device_p);

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
