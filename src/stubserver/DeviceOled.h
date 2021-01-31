/*
 * DeviceOled.h
 *
 * Copyright (C) 2017-2021 Holger Grosenick
 */

#ifndef STUBSERVER_DEVICEOLED_H_
#define STUBSERVER_DEVICEOLED_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * Simulated OLED display with 128*64 or 64*64 pixels.
 */
class DeviceOled : public DeviceFunctions, public DisplayState
{
protected:
    uint8_t  windowColFrom, windowColTo;
    uint8_t  windowRowFrom, windowRowTo;
    uint8_t  currentCol, currentRow;
    uint16_t chunkPixelOffset;

    /**
     * print text: full data is the io packet with column, line and text.
     */
    void writeText(uint8_t fulldata[]);

    void clearDisplay(VisualizationClient &visualizationClient);

public:
    DeviceOled(unsigned _cols, unsigned _lines);

    DECLARE_OWN_DEVICE_CALLBACKS
};

/**
 * Simulated LCD with touch, 128*64 pixels.
 */
class DeviceLcdWithTouch : public DeviceOled
{
    V2Device *v2device;

    // callback values
    uint64_t lastEventTime;
    uint32_t touchCbperiod;
    uint32_t gestureCbperiod;
    bool     touchHasToChange;
    bool     gestureHasToChange;
    bool     automaticDraw;

public:
    DeviceLcdWithTouch(unsigned _cols, unsigned _lines);
    ~DeviceLcdWithTouch();

    DECLARE_OWN_DEVICE_CALLBACKS
};

/**
 * Simulated ePaper, 296x128 pixels.
 */
class DeviceEPaper : public DeviceOled
{
    V2Device *v2device;

public:
    DeviceEPaper();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEOLED_H_ */
