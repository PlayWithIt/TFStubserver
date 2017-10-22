/*
 * DeviceOled.cpp
 *
 * Copyright (C) 2017 Holger Grosenick
 */

#include <bricklet_oled_128x64.h>
#include <bricklet_oled_64x48.h>

#include "CharMask.h"
#include "DeviceOled.h"

// just verify that the function codes are the same for both OLED types
#if OLED_128X64_FUNCTION_WRITE != OLED_64X48_FUNCTION_WRITE
#error "OLED_128X64_FUNCTION_WRITE != OLED_64X48_FUNCTION_WRITE"
#endif
#if OLED_128X64_FUNCTION_WRITE_LINE != OLED_64X48_FUNCTION_WRITE_LINE
#error "OLED_128X64_FUNCTION_WRITE_LINE != OLED_64X48_FUNCTION_WRITE_LINE"
#endif
#if OLED_128X64_FUNCTION_NEW_WINDOW != OLED_64X48_FUNCTION_NEW_WINDOW
#error "OLED_128X64_FUNCTION_NEW_WINDOW != OLED_64X48_FUNCTION_NEW_WINDOW"
#endif
#if OLED_128X64_FUNCTION_SET_DISPLAY_CONFIGURATION != OLED_64X48_FUNCTION_SET_DISPLAY_CONFIGURATION
#error "OLED_128X64_FUNCTION_SET_DISPLAY_CONFIGURATION != OLED_64X48_FUNCTION_SET_DISPLAY_CONFIGURATION"
#endif
#if OLED_128X64_FUNCTION_GET_DISPLAY_CONFIGURATION != OLED_64X48_FUNCTION_GET_DISPLAY_CONFIGURATION
#error "OLED_128X64_FUNCTION_GET_DISPLAY_CONFIGURATION != OLED_64X48_FUNCTION_GET_DISPLAY_CONFIGURATION"
#endif


namespace stubserver {

// OLED char dot matrix
static ScreenFont oledFont(true);

/**
 * Init attrs and char mask if necessary.
 */
DeviceOled::DeviceOled(unsigned _cols, unsigned _lines)
   : OledState(_cols, _lines)
   , windowColFrom(0), windowColTo(0)
   , windowRowFrom(0), windowRowTo(0)
   , currentCol(0), currentRow(0)
{
}

/**
 * print text: fulldata is the io packet with col, line and text.
 */
void DeviceOled::writeText(uint8_t fulldata[])
{
    uint8_t line = fulldata[0];
    uint8_t ind  = fulldata[1];
    unsigned offset = line * cols + ind * 5;

    for (unsigned i = 2; i < 28; ++i)
    {
        uint8_t c = fulldata[i];
        if (c == 0)
            return;

        PCharMask mask = oledFont.get(c);
        for (int x = 0; x < 5; ++x)
        {
            uint8_t byte = 0;
            for (int y = 0; y < 8; ++y)
                if (mask->isOn(x, y))
                    byte += (1 << y);

            // skip leading blank cols
            if (x > 0 || byte != 0) {
                pixels[offset] = byte;
                // printf("Byte %d = %d\n", x, byte);
                ++offset;
            }
        }

        // one blank col between chars
        if (pixels[offset - 1] != 0) {
            pixels[offset] = 0;
            ++offset;
        }
    }
}

/**
 *
 */
bool DeviceOled::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);
    unsigned index, i, size;

    switch (p.header.function_id) {
    case OLED_128X64_FUNCTION_WRITE:

        // printf("OLED_FUNCTION_WRITE %d %d\n", currentRow, currentCol);

        index = currentRow * cols + currentCol;
        for (i = 0; i < 64; ++i) {
            pixels[index + i] = p.fullData.payload[i];

            if (++currentCol > windowColTo) {
                currentCol = windowColFrom;
                if (++currentRow > windowRowTo) {
                    currentRow = windowRowFrom;
                    notify(visualizationClient, VALUE_CHANGE);
                    return true;
                }
            }
        }
        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case OLED_128X64_FUNCTION_WRITE_LINE:
        // printf("OLED_FUNCTION_WRITE_LINE %d \n", currentRow);
        writeText(p.fullData.payload);
        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case OLED_128X64_FUNCTION_NEW_WINDOW:
        if (p.fullData.payload[0] < cols)
            windowColFrom = p.fullData.payload[0];
        if (p.fullData.payload[1] < cols)
            windowColTo   = p.fullData.payload[1];

        size = lines / 8; // max bytes
        if (p.fullData.payload[2] < size)
            windowRowFrom = p.fullData.payload[2];
        if (p.fullData.payload[3] < size)
            windowRowTo   = p.fullData.payload[3];

        // printf("OLED_FUNCTION_NEW_WINDOW %d-%d %d-%d \n", windowColFrom, windowColTo, windowRowFrom, windowRowTo);

        currentCol = windowColFrom;
        currentRow = windowRowFrom;
        return true;

    case OLED_128X64_FUNCTION_CLEAR_DISPLAY:
        // clear the screen area of the current window

        index = windowRowFrom * cols + windowColFrom;
        size  = (windowColTo - windowColFrom + 1) * (windowRowTo - windowRowFrom + 1);
        for (i = 0; i < size; ++i) {
            pixels[index + i] = 0;
        }

        // printf("OLED_FUNCTION_CLEAR_DISPLAY\n");
        currentCol = windowColFrom;
        currentRow = windowRowFrom;

        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case OLED_128X64_FUNCTION_SET_DISPLAY_CONFIGURATION:
        contrast = p.fullData.payload[0];
        inverted = p.fullData.payload[1] ? true : false;
        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case OLED_128X64_FUNCTION_GET_DISPLAY_CONFIGURATION:
        p.header.length += 2;
        p.fullData.payload[0] = contrast;
        p.fullData.payload[1] = inverted;
        return true;
    }

    return false;
}

/**
 * OLED has no callbacks.
 */
void DeviceOled::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &vc)
{
}

} /* namespace stubserver */
