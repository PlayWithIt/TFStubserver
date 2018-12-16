/*
 * DeviceOled.h
 *
 * Copyright (C) 2017 Holger Grosenick
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
    V2Device *v2device;

    uint8_t  windowColFrom, windowColTo;
    uint8_t  windowRowFrom, windowRowTo;
    uint8_t  currentCol, currentRow;
    uint16_t chunkPixelOffset;
    bool     isLcd;
    bool     automaticDraw;

    /**
     * print text: full data is the io packet with column, line and text.
     */
    void writeText(uint8_t fulldata[]);

    void clearDisplay(VisualizationClient &visualizationClient);

public:
    DeviceOled(unsigned _cols, unsigned _lines);
    ~DeviceOled();

    /**
     * For new LCD with 128x64 pixels.
     */
    DeviceOled(bool isLcd, unsigned _cols, unsigned _lines);

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEOLED_H_ */
