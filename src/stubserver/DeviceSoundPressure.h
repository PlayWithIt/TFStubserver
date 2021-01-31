/*
 * DeviceSoundPressure.h
 *
 * Copyright (C) 2019 Holger Grosenick
 */

#ifndef STUBSERVER_DEVICESOUNDPRESSURE_H_
#define STUBSERVER_DEVICESOUNDPRESSURE_H_

#include <utils/ValueProvider.h>

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * SoundPressureLevel device.
 */
class DeviceSoundPressure : public V2Device, public SensorState
{
    utils::CSVValueProvider *values;   // the pressure value provider

    RangeCallback  changedDecibelCallback;
    BasicCallback  spectrumCallback;

    std::vector<int> spectrum;
    unsigned spectrumPos;

    uint8_t fftSize;
    uint8_t weighting;

public:
    DeviceSoundPressure(utils::CSVValueProvider *_values);
    virtual ~DeviceSoundPressure();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICESOUNDPRESSURE_H_ */
