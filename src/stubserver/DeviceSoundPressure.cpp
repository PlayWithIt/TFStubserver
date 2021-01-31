/*
 * DeviceSoundPressure.cpp
 *
 * Copyright (C) 2020 Holger Grosenick
 */

#include <bricklet_sound_pressure_level.h>

#include <BrickStack.h>

#include "DeviceSoundPressure.h"

namespace stubserver {


DeviceSoundPressure::DeviceSoundPressure(utils::CSVValueProvider *_values)
  : V2Device(NULL, this, true)
  , SensorState(0, 1000)  // -0 .. 100dbA
  , values(_values)
  , spectrumCallback(SOUND_PRESSURE_LEVEL_FUNCTION_GET_SPECTRUM_CALLBACK_CONFIGURATION,
                     SOUND_PRESSURE_LEVEL_FUNCTION_SET_SPECTRUM_CALLBACK_CONFIGURATION,
                     SOUND_PRESSURE_LEVEL_CALLBACK_SPECTRUM, 0)
  , spectrumPos(0)
  , fftSize(SOUND_PRESSURE_LEVEL_FFT_SIZE_256)
  , weighting(SOUND_PRESSURE_LEVEL_WEIGHTING_A)
{
    changedDecibelCallback.callbackCode = SOUND_PRESSURE_LEVEL_CALLBACK_DECIBEL;
    changedDecibelCallback.getThresholdFunctionCode = SOUND_PRESSURE_LEVEL_FUNCTION_GET_DECIBEL_CALLBACK_CONFIGURATION;
    changedDecibelCallback.setThresholdFunctionCode = SOUND_PRESSURE_LEVEL_FUNCTION_SET_DECIBEL_CALLBACK_CONFIGURATION;

    other = new GetSet<uint16_t>(SOUND_PRESSURE_LEVEL_FUNCTION_GET_CHIP_TEMPERATURE, 0, 2450);   // 24.5 degrees
}

DeviceSoundPressure::~DeviceSoundPressure()
{
    delete values;
}



bool DeviceSoundPressure::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    if (values == NULL)
        throw utils::Exception("ValueProvider not set!");

    uint8_t func = p.header.function_id;
    p.header.length = sizeof(p.header);
    unsigned ssize;

    switch (func) {
    case SOUND_PRESSURE_LEVEL_FUNCTION_GET_DECIBEL:
        p.header.length += 2;
        p.uint16Value = values->getValue(relativeTimeMs);
        return true;

    case SOUND_PRESSURE_LEVEL_FUNCTION_GET_SPECTRUM_LOW_LEVEL:
        // Rückgabe:
        // spectrum_length       – Typ: uint16, Wertebereich: [64 bis 512]
        // spectrum_chunk_offset – Typ: uint16, Wertebereich: [0 bis 216 - 1]
        // spectrum_chunk_data   – Typ: uint16[30], Einheit: ? dB, Wertebereich: [0 bis 216 - 1]

        ssize = spectrum.size();
        if (spectrumPos >= ssize) {
            // start new set of data
            values->getValues(relativeTimeMs, spectrum);
            spectrumPos = 0;
            ssize = spectrum.size();
        }

        p.header.length += 2*32;
        p.ushorts.value1 = ssize - 1;    // first element of array is the DBA value
        p.ushorts.value2 = spectrumPos;

        for (unsigned i = 0; i < 30; ++i) {
            unsigned index = i+spectrumPos+1;
            uint16_t v = index >= ssize ? 0 : spectrum[index];
            p.ushorts.values[i] = v;
        }
        spectrumPos += 30;
        return true;

    case SOUND_PRESSURE_LEVEL_FUNCTION_GET_CONFIGURATION:
        p.header.length += 2;
        p.fullData.payload[0] = this->fftSize;
        p.fullData.payload[1] = this->weighting;
        return true;

    case SOUND_PRESSURE_LEVEL_FUNCTION_SET_CONFIGURATION:
        this->fftSize   = p.fullData.payload[0];
        this->weighting = p.fullData.payload[1];
        return true;
    }

    // get/set range callback options
    if (changedDecibelCallback.consumeGetSetConfig(p)) {
        return true;
    }
    if (func == spectrumCallback.setPeriodFunc) {
        spectrumCallback.period = p.uint32Value;

        // reduce to 20ms period as there is not more test data and spectrum transfer takes time
        if (spectrumCallback.period > 0 && spectrumCallback.period < 20)
            spectrumCallback.period = 20;

        spectrumCallback.active = (spectrumCallback.period > 0);
        spectrumCallback.relativeStartTime = relativeTimeMs;

        //printf("spectrum callback: %d\n", spectrumCallback.active);
        return true;
    }
    if (func == spectrumCallback.getPeriodFunc) {
        p.uint32Value = spectrumCallback.period;
        return true;
    }

    // other get set functions
    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}


void DeviceSoundPressure::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int currentValue;

    if (visualizationClient.useAsInputSource()) {
        currentValue = visualizationClient.getInputState();
        if (currentValue != sensorValue)
            sensorValue = currentValue;
    }
    else {
        currentValue = values->getValue(relativeTimeMs);
        if (currentValue != sensorValue)
        {
            sensorValue = currentValue;
            notify(visualizationClient, VALUE_CHANGE);
        }
    }

    //----- check for spectrum low level callback
    if (spectrumCallback.active && relativeTimeMs > spectrumCallback.relativeStartTime)
    {
        IOPacket packet(uid, SOUND_PRESSURE_LEVEL_CALLBACK_SPECTRUM_LOW_LEVEL, 64);
        std::vector<int> data;

        values->getValues(relativeTimeMs, data);
        unsigned last = data.size();
        unsigned spectrumPos = 0;
        unsigned opos        = 0;

        packet.ushorts.value1 = last - 1;        // first element of array is the DBA value
        packet.ushorts.value2 = spectrumPos;     // spectrum_pos
        spectrumCallback.relativeStartTime += spectrumCallback.period;

        for (unsigned i = 1; i < last; ++i) {
            packet.ushorts.values[opos] = data[i];
            if (++opos == 30) {
                brickStack->dispatchCallback(packet);
               // printf("dispatch spectrum: %u\n", spectrumPos);
                opos = 0;
                spectrumPos += 30;
                packet.ushorts.value2 = spectrumPos;     // spectrum_pos
                memset(packet.ushorts.values, 0, sizeof(packet.ushorts.values));
            }
        }

        // dispath last packet
        if (opos != 30) {
            brickStack->dispatchCallback(packet);
            // printf("dispatch spectrum: %u\n", spectrumPos);
        }
    }
/*
    if (changedPressureCb.mayExecute(relativeTimeMs) && currentValue != changedPressureCb.param1)
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, changedPressureCb, currentValue);
        changedPressureCb.param1 = currentValue;
    }

    // changed callback: if pressure has changed, height has changed too.
    // param1 of height callback holds the latest pressure value...
    if (changedHeightCb.mayExecute(relativeTimeMs) && currentValue != changedHeightCb.param1)
    {
        // calculate height
        int currentHeight = getAltitude(currentValue);
        triggerCallbackInt(relativeTimeMs, uid, brickStack, changedHeightCb, currentHeight);
        changedHeightCb.param1 = currentValue;
    }

    if (rangeCallback.shouldTriggerRangeCallback(relativeTimeMs, currentValue))
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, rangeCallback, currentValue);
    }
*/
}


} /* namespace stubserver */
