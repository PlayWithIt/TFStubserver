/*
 * PacketTypes.cpp
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


#include <memory.h>

#include "PacketTypes.h"

namespace stubserver {

/**
 * Init all with zeroes.
 */
IOPacket::IOPacket()
{
    bzero(this, sizeof(IOPacket));
}

/**
 * Init an IOPacket with the uid, the function id and the additional length
 * that is used as return data in addition to the header.
 */
IOPacket::IOPacket(uint32_t uid, uint8_t function_id, uint8_t addLength)
{
    bzero(this, sizeof(IOPacket));
    this->header.uid = uid;
    this->header.length = sizeof(PacketHeader) + addLength;
    this->header.function_id = function_id;
}

/**
 * Set the error code in the packet header.
 * @param rc error code to set.
 */
void IOPacket::setErrorCode(ErrorCode rc)
{
    header.error_code_and_future_use |= (rc << 6);
}

} /* namespace stubserver */
