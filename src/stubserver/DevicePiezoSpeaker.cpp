/*
 * DevicePiezoSpeaker.cpp
 *
 * Copyright (C) 2013-2021 Holger Grosenick
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

#include <bricklet_piezo_speaker.h>
#include <bricklet_piezo_speaker_v2.h>

#include <utils/Log.h>

#include "DevicePiezoSpeaker.h"
#include "BrickStack.h"

namespace stubserver {

DevicePiezoSpeaker::DevicePiezoSpeaker(bool isV2)
  : V2Device(NULL, this, isV2)
  , VisibleDeviceState(0)
  , callbackTime(0)
  , callbackFunctionId(false)
  , sendCallback(false)
  , frequency(0)
  , duration(0)
  , volume(5)
{
}

DevicePiezoSpeaker::~DevicePiezoSpeaker()
{
}


/**
 * Check for known function codes.
 */
bool DevicePiezoSpeaker::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &c)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    if (isV2)
        return consumeCommandV2(relativeTimeMs, p, c);

    // check function to perform
    switch(p.header.function_id)
    {
    case PIEZO_SPEAKER_FUNCTION_BEEP:
        if (frequency != p.beepRequest.frequency || duration != p.beepRequest.duration)
        {
            // allocate new buffer
            duration  = p.beepRequest.duration;
            frequency = p.beepRequest.frequency;
            wavBuffer = player.makeWav(duration, frequency);
            // utils::Log() << "New sound buffer " << duration << " " << frequency;
        }
        player.playback(wavBuffer, true);

        callbackFunctionId = PIEZO_SPEAKER_CALLBACK_BEEP_FINISHED;
        sendCallback = true;
        callbackTime = relativeTimeMs + duration;
        return true;

    case PIEZO_SPEAKER_FUNCTION_MORSE_CODE:
    {
        callbackFunctionId = PIEZO_SPEAKER_CALLBACK_MORSE_CODE_FINISHED;
        duration     = 0;
        sendCallback = true;
        callbackTime = relativeTimeMs + duration;

        unsigned byteNo = 0;
        char *code = p.morseCode.morse;
        uint16_t freq = p.morseCode.frequency;

        SoundPlayback::SoundList list;

        while (byteNo < sizeof(p.morseCode.morse) && *code)
        {
            if (*code == '.')
            {
                // short sound
                list.push_back(std::make_pair(130, freq));
                list.push_back(std::make_pair(50, 0));
            }
            else if (*code == '-') {
                // long sound
                list.push_back(std::make_pair(200, freq));
                list.push_back(std::make_pair(50, 0));
            }
            else if (*code == ' ') {
                // long pause
                list.push_back(std::make_pair(180, 0));
            }
            else {
                printf("Ignored a morse code '%c'\n", *code);
            }
            ++byteNo;
            ++code;
        }

        wavBuffer = player.makeWav(list);
        player.playback(wavBuffer, true);
        frequency = 0;
    }
    return true;

    case PIEZO_SPEAKER_FUNCTION_CALIBRATE:
        ++p.header.length; // return a bool
        p.boolValue = 1;
        return true;
    }
    return false;
}

/**
 * Check for known function codes.
 */
bool DevicePiezoSpeaker::consumeCommandV2(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // check function to perform
    switch(p.header.function_id)
    {
    case PIEZO_SPEAKER_V2_FUNCTION_SET_BEEP:
        // Eine duration von 0 stoppt den aktuellen Piepton. Eine duration von 4294967295 führt zu einem unendlich langen Piepton.
        // uint16_t frequency, uint8_t volume, uint32_t duration

        if (frequency != p.beepV2Request.frequency || duration != p.beepV2Request.duration || volume != p.beepV2Request.volume)
        {
            // allocate new buffer
            duration  = p.beepV2Request.duration;
            frequency = p.beepV2Request.frequency;
            volume    = p.beepV2Request.volume;
            wavBuffer = player.makeWav(duration, frequency, volume);
        }
        player.playback(wavBuffer, true);

        callbackFunctionId = PIEZO_SPEAKER_V2_CALLBACK_BEEP_FINISHED;
        sendCallback = true;
        callbackTime = relativeTimeMs + duration;
        return true;

    case PIEZO_SPEAKER_V2_FUNCTION_GET_BEEP:
        p.header.length += sizeof(p.beepV2Request);
        p.beepV2Request.duration = duration;
        p.beepV2Request.volume = volume;
        p.beepV2Request.frequency = frequency;
        return true;

    case PIEZO_SPEAKER_V2_FUNCTION_UPDATE_FREQUENCY:
    case PIEZO_SPEAKER_V2_FUNCTION_UPDATE_VOLUME:
        // update not really supported ...
        return true;

    case PIEZO_SPEAKER_V2_FUNCTION_SET_ALARM:
    case PIEZO_SPEAKER_V2_FUNCTION_GET_ALARM:
        break;

    default:
        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);

    }
    return false;
}

/**
 * Check for the switchDone callbacks.
 */
void DevicePiezoSpeaker::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &)
{
    if (sendCallback && callbackTime <= relativeTimeMs)
    {
        // trigger switching done callback
        IOPacket packet(uid, callbackFunctionId);
        brickStack->dispatchCallback(packet);
        sendCallback = false;
    }
}

} /* namespace stubserver */
