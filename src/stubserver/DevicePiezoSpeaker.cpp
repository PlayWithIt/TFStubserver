/*
 * DevicePiezoSpeaker.cpp
 *
 * Copyright (C) 2013-2022 Holger Grosenick
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
  , SensorState(0, 44000)
  , callbackTime(0)
  , alarmUpdateTime(0)
  , callbackFunctionId(false)
  , sendCallback(false)
  , plusFrequency(true)
  , frequency(0)
  , duration(0)
  , volume(5)
  , startFrequency(0)
  , endFrequency(0)
  , stepSize(0)
  , stepDelay(0)
  , currentFrequency(0)
{
    // default is on
    if (isV2) {
        printf("PiezoSpeaker created\n");
        setStatusLedOn(true);
    }
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

        // show frequency in UI is connected:
        sensorValue = frequency;
        notify(c);
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

        // show frequency in UI is connected:
        sensorValue = frequency;
        notify(visualizationClient);
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
//        start_frequency – Typ: uint16_t, Einheit: 1 Hz, Wertebereich: [50 bis 14999]
//        end_frequency – Typ: uint16_t, Einheit: 1 Hz, Wertebereich: [51 bis 15000]
//        step_size – Typ: uint16_t, Einheit: 1 Hz, Wertebereich: [0 bis 14950]
//        step_delay – Typ: uint16_t, Einheit: 1 ms, Wertebereich: [0 bis 216 - 1]
//        volume – Typ: uint8_t, Wertebereich: [0 bis 10]
//        duration – Typ: uint32_t, Einheit: 1 ms, Wertebereich: [0 bis 232 - 1] mit Konstanten
        if (p.alarmRequest.start_frequency <= p.alarmRequest.end_frequency) {
            startFrequency = p.alarmRequest.start_frequency;
            endFrequency   = p.alarmRequest.end_frequency;
        }
        else {
            startFrequency = p.alarmRequest.end_frequency;
            endFrequency   = p.alarmRequest.start_frequency;
        }
        stepSize         = p.alarmRequest.step_size;
        stepDelay        = p.alarmRequest.step_delay;
        duration         = p.alarmRequest.duration;
        wavBuffer        = player.makeWav(duration, startFrequency, p.alarmRequest.volume);
        currentFrequency = startFrequency;
        plusFrequency    = true;

        callbackFunctionId = PIEZO_SPEAKER_V2_CALLBACK_ALARM_FINISHED;
        sendCallback = true;
        callbackTime = relativeTimeMs + duration;
        alarmUpdateTime = relativeTimeMs + stepDelay;

        // show frequency in UI is connected:
        sensorValue = currentFrequency;
        notify(visualizationClient);
        return true;

    case PIEZO_SPEAKER_V2_FUNCTION_GET_ALARM:
//        ret_start_frequency – Typ: uint16_t, Einheit: 1 Hz, Wertebereich: [50 bis 14999]
//        ret_end_frequency – Typ: uint16_t, Einheit: 1 Hz, Wertebereich: [51 bis 15000]
//        ret_step_size – Typ: uint16_t, Einheit: 1 Hz, Wertebereich: [50 bis 14950]
//        ret_step_delay – Typ: uint16_t, Einheit: 1 ms, Wertebereich: [0 bis 216 - 1]
//        ret_volume – Typ: uint8_t, Wertebereich: [0 bis 10]
//        ret_duration – Typ: uint32_t, Einheit: 1 ms, Wertebereich: [0 bis 232 - 1] mit Konstanten
//        ret_duration_remaining – Typ: uint32_t, Einheit: 1 ms, Wertebereich: [0 bis 232 - 1] mit Konstanten
//        ret_current_frequency – Typ: uint16_t, Einheit: 1 Hz, Wertebereich: [50 bis 15000]
        p.header.length += sizeof(p.alarmRequest);
        p.alarmRequest.start_frequency = startFrequency;
        p.alarmRequest.end_frequency = endFrequency;
        p.alarmRequest.step_size = stepSize;
        p.alarmRequest.step_delay = stepDelay;
        p.alarmRequest.volume = volume;
        p.alarmRequest.duration = duration;
        p.alarmRequest.current_frequency = currentFrequency;

        // alarm was activated at least once and not yet elapsed
        if (callbackTime > 0 && relativeTimeMs < callbackTime)
            p.alarmRequest.duration_remaining = callbackTime - relativeTimeMs;
        else
            p.alarmRequest.duration_remaining = 0;
        return true;

    default:
        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);

    }
    return false;
}

/**
 * Check for the switchDone callbacks.
 */
void DevicePiezoSpeaker::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &c)
{
    if (!sendCallback)
        return;

    if (callbackFunctionId == PIEZO_SPEAKER_V2_CALLBACK_ALARM_FINISHED && alarmUpdateTime <= relativeTimeMs) {
        if (plusFrequency) {
            // plus: increase frequency
            if (currentFrequency < endFrequency) {
                currentFrequency += stepSize;
            } else {
                // max reached: start to decrease
                plusFrequency = false;
                currentFrequency -= stepSize;
            }
        }
        else {
            // not plus: decrease frequency
            if (currentFrequency > startFrequency) {
                currentFrequency -= stepSize;
            } else {
                // max reached: start to decrease
                plusFrequency = true;
                currentFrequency += stepSize;
            }
        }
        alarmUpdateTime  += stepSize;

        // show frequency in UI is connected:
        sensorValue = currentFrequency;
        notify(c);
    }

    if (callbackTime <= relativeTimeMs)
    {
        // trigger sound done callback
        IOPacket packet(uid, callbackFunctionId);
        brickStack->dispatchCallback(packet);
        sendCallback = false;

        // show frequency in UI is connected:
        sensorValue = 0;
        notify(c);
    }
}

} /* namespace stubserver */
