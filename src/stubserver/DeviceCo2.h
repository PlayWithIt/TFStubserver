/*
 * DeviceCo2.h
 *
 * Copyright (C) 2021 Holger Grosenick
 */

#ifndef STUBSERVER_DEVICECO2_H_
#define STUBSERVER_DEVICECO2_H_

#include <utils/ValueProvider.h>

#include "DeviceFunctions.h"
#include "VisualizationClient.h"


namespace stubserver {

/**
 * Emulated CO2 sensor.
 * <p>
 * SensorState.sensorValue  = CO2 value
 * SensorState.sensorValue1 = temperature value
 * SensorState.sensorValue2 = humidity value
 */
class DeviceCo2: public V2Device, public SensorState
{
    // These are used either or: either CSV for all three values or 3 single ones
    utils::CSVValueProvider *multiValueProvider;
    utils::ValueProvider    *vpCo2;
    utils::ValueProvider    *vpTemp;
    utils::ValueProvider    *vpHum;

    RangeCallback cbAll;
    RangeCallback cbCo2;
    RangeCallback cbTemp;
    RangeCallback cbHum;

    int16_t tempOffset;

public:
    /**
     * If the first ValueProvider is a CSV value provider, then the others can be null,
     * otherwise all three must be set.
     */
    DeviceCo2(utils::ValueProvider *vp, utils::ValueProvider *vpTemp, utils::ValueProvider *vpHum);
    ~DeviceCo2();

    DECLARE_OWN_DEVICE_CALLBACKS
};


} /* namespace stubserver */

#endif /* STUBSERVER_DEVICECO2_H_ */
