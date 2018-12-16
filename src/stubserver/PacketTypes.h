/*
 * PacketTypes.h
 *
 * Copyright (C) 2013 Holger Grosenick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#ifndef PACKETTYPES_H_
#define PACKETTYPES_H_

#if defined _MSC_VER || defined __BORLANDC__
        #pragma pack(push)
        #pragma pack(1)
        #define ATTRIBUTE_PACKED
#elif defined __GNUC__
        #ifdef _WIN32
                // workaround struct packing bug in GCC 4.7 on Windows
                // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52991
                #define ATTRIBUTE_PACKED __attribute__((gcc_struct, packed))
        #else
                #define ATTRIBUTE_PACKED __attribute__((packed))
        #endif
#else
        #error unknown compiler, do not know how to enable struct packing
#endif

namespace stubserver {

/**
 * Same as in ip_connection.h
 */
typedef struct {
        uint32_t uid;
        uint8_t length;
        uint8_t function_id;
        uint8_t sequence_number_and_options;
        uint8_t error_code_and_future_use;
} ATTRIBUTE_PACKED PacketHeader;

/**
 *
 */
typedef struct IOPacket
{
    PacketHeader header;
    union {
        uint64_t uint64Value;
        int64_t  int64Value;
        uint32_t uint32Value;
        int32_t  int32Value;
        uint16_t uint16Value;
        int16_t  int16Value;
        uint8_t  uint8Value;
        int8_t   int8Value;
        bool     boolValue;
        char     group[4];

        struct {
            uint32_t value1;
            uint32_t value2;
        } ATTRIBUTE_PACKED uints;

        struct {
            int16_t value1;
            int16_t value2;
        } ATTRIBUTE_PACKED shorts;

        struct {
            uint16_t value1;
            uint16_t value2;
        } ATTRIBUTE_PACKED ushorts;

        struct {
            uint16_t value;
            uint32_t time;
            uint32_t time_remaining;
        } ATTRIBUTE_PACKED monoflopResponse;

        struct {
            uint32_t duration;
            uint16_t frequency;
        } ATTRIBUTE_PACKED beepRequest;

        struct {
            uint8_t  channel;
            int32_t  value1;
            int32_t  value2;
        } ATTRIBUTE_PACKED channelRequest;

        struct {
            bool     state;
            uint32_t time;
            uint32_t time_remaining;
        } ATTRIBUTE_PACKED monoflopResponseDualRelay;

        struct {
            uint16_t selection_mask;
            uint16_t value_mask;
            uint32_t time;
        } ATTRIBUTE_PACKED monoflopDefine;

        struct {
            uint8_t protocol_version;
            uint8_t firmware_version[3];
            char name[40];
        } ATTRIBUTE_PACKED protocol1Response;

        struct {
            char option;
            uint16_t min;
            uint16_t max;
        } ATTRIBUTE_PACKED threshold;

        struct {
            char option;
            int  min;
            int  max;
        } ATTRIBUTE_PACKED thresholdInt;

        struct {
            uint8_t channel;
            char option;
            int  min;
            int  max;
        } ATTRIBUTE_PACKED thresholdChannelInt;

        struct {
            char morse[60];
            uint16_t frequency;
        } ATTRIBUTE_PACKED morseCode;

        struct {
            uint16_t index;
            uint8_t length;
            uint8_t r[16];
            uint8_t g[16];
            uint8_t b[16];
        } ATTRIBUTE_PACKED ledColors;

        struct {
            int16_t  temperature;
            uint8_t  humidity;
            uint16_t last_change;
        } ATTRIBUTE_PACKED outdoorData;

        struct {
            uint8_t  x_start;
            uint8_t  y_start;
            uint8_t  x_end;
            uint8_t  y_end;
            uint16_t pixels_length;
            uint16_t pixels_chunk_offset;
            uint8_t  bits[56];
        } ATTRIBUTE_PACKED lcdData;

        struct {
            uint8_t  id;
            int16_t  temperature;
            uint8_t  humidity;
        } ATTRIBUTE_PACKED outdoorCallback;

        struct {
            char uid[8];
            char connected_uid[8];
            char position;
            uint8_t hardware_version[3];
            uint8_t firmware_version[3];
            uint16_t device_identifier;
        } ATTRIBUTE_PACKED identity;

        struct {
            uint8_t payload[64];
            uint8_t optional_data[8];
        } ATTRIBUTE_PACKED fullData;
    };

    /**
     * Known error code which might be returned by the devices
     */
    enum ErrorCode {
        OK = 0,
        INVALID_PARAMETER = 1,
        NOT_SUPPORTED = 2
    };

    /**
     * Init all with zeroes.
     */
    IOPacket();

    /**
     * Init an IOPacket with the uid, the function id and the additional length
     * that is used as return data in addition to the header.
     */
    IOPacket(uint32_t uid, uint8_t function_id, uint8_t addLength = 0);

    /**
     * Set the error code in the packet header.
     * @param rc error code to set.
     */
    void setErrorCode(ErrorCode rc);

    /**
     * See also IPConnection, but we don't link IPConnection here
     */
    bool packet_header_get_response_expected() {
        return ((header.sequence_number_and_options >> 3) & 0x01) != 0;
    }

} ATTRIBUTE_PACKED _IOPacket;

} /* namespace stubserver */


#if defined _MSC_VER || defined __BORLANDC__
        #pragma pack(pop)
#endif
#undef ATTRIBUTE_PACKED


#endif /* PACKETTYES_H_ */
