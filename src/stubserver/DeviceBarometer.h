/*
 * DeviceBarometer.h
 *
 *  Created on: 14.12.2013
 *      Author: holger
 */

#ifndef DEVICEBAROMETER_H_
#define DEVICEBAROMETER_H_

#include <utils/ValueProvider.h>
#include <DeviceFunctions.h>

namespace stubserver {

/**
 * A simulated barometer sensor: the barometer has more callbacks and the two values
 * have the same base (pressure + altitude), this means they belong together and
 * depend on each other.
 *
 * This device serves get requests and changed callbacks.
 */
class DeviceBarometer : public DeviceFunctions
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
    DeviceBarometer();
    ~DeviceBarometer();

    /**
     * Changes the value provider: must be non-null, the ownership is taken
     * into this object and released when the object is destroyed.
     */
    void setValueProvider(utils::ValueProvider *values);

    bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged);
    void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged);
};

} /* namespace stubserver */

#endif /* DEVICEBAROMETER_H_ */
