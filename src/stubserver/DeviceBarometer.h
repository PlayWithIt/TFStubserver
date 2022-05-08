/*
 * DeviceBarometer.h
 *
 *  Created on: 14.12.2013
 *      Author: holger
 */

#ifndef DEVICEBAROMETER_H_
#define DEVICEBAROMETER_H_

#include <utils/ValueProvider.h>

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * A simulated barometer sensor: the barometer has more callbacks and the two values
 * have the same base (pressure + altitude), this means they belong together and
 * depend on each other.
 *
 * This device serves get requests and changed callbacks.
 */
class DeviceBarometer : public DeviceFunctions, public SensorState
{
    utils::ValueProvider *values;   // the pressure value provider

    uint8_t          averaging[4];
    GetSetRaw       *getSet;
    GetSet<int32_t> *getSetRefPressure;
    BasicCallback    changedPressureCb;
    BasicCallback    changedHeightCb;
    RangeCallback    rangeCallback;

    // calculate altitude from height
    int getAltitude(int pressure) const;

public:
    DeviceBarometer(utils::ValueProvider *_values);
    ~DeviceBarometer();

    DECLARE_OWN_DEVICE_CALLBACKS
};

/**
 * Like a DeviceBarometer but in hardware release V2 ...
 */
class DeviceBarometerV2 : public V2Device, public SensorState
{
    utils::ValueProvider *vpAir;    // the air pressure value provider
    utils::ValueProvider *vpTemp;   // the temperature value provider

    uint8_t          sensorConfig[2];
    GetSetRaw       *getSet;
    GetSet<int32_t> *getSetRefPressure;
    RangeCallback    cbAirPressure;
    RangeCallback    cbTemp;

    // calculate altitude from height
    int getAltitude(int pressure) const;

public:
    DeviceBarometerV2(utils::ValueProvider *_values, utils::ValueProvider *_vpTemp);
    ~DeviceBarometerV2();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* DEVICEBAROMETER_H_ */
