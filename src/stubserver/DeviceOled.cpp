/*
 * DeviceOled.cpp
 *
 * Copyright (C) 2017 Holger Grosenick
 */

#include <bricklet_lcd_128x64.h>
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
#if OLED_128X64_FUNCTION_WRITE_LINE != LCD_128X64_FUNCTION_WRITE_LINE
#error "OLED_128X64_FUNCTION_WRITE_LINE != LCD_128X48_FUNCTION_WRITE_LINE"
#endif
#if OLED_128X64_FUNCTION_CLEAR_DISPLAY != LCD_128X64_FUNCTION_CLEAR_DISPLAY
#error "OLED_128X64_FUNCTION_CLEAR_DISPLAY != LCD_128X64_FUNCTION_CLEAR_DISPLAY"
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
    : DisplayState(_cols, _lines)
    , v2device(NULL)
    , windowColFrom(0), windowColTo(0)
    , windowRowFrom(0), windowRowTo(0)
    , currentCol(0), currentRow(0)
    , chunkPixelOffset(0)
    , isLcd(false)
    , automaticDraw(false)
{
}

/**
 * For new LCD with 128x64 pixels.
 */
DeviceOled::DeviceOled(bool _isLcd, unsigned _cols, unsigned _lines)
    : DisplayState(_cols, _lines)
    , v2device(NULL)
    , windowColFrom(0), windowColTo(127)
    , windowRowFrom(0), windowRowTo(7)
    , currentCol(0), currentRow(0)
    , chunkPixelOffset(0)
    , isLcd(_isLcd)
    , automaticDraw(_isLcd)
{
    if (_isLcd) {
        v2device = new V2Device(NULL, this);
        contrast = 14;
        backlight = 100;
    }
}

DeviceOled::~DeviceOled() {
    delete v2device;
}


/**
 * print text: full data is the io packet with column, line and text.
 */
void DeviceOled::writeText(uint8_t fulldata[])
{
    uint8_t line = fulldata[0];
    uint8_t ind  = fulldata[1];
    unsigned colOffset  = ind * 5;
    unsigned byteOffset = line * cols + colOffset;

    for (unsigned i = 2; i < 28; ++i)
    {
        uint8_t c = fulldata[i];
        if (c == 0)
            return;

        PCharMask mask = oledFont.get(c);
        for (int x = 0; x < 5; ++x)
        {
            if (colOffset >= cols)  // truncate line
                return;

            uint8_t byte = 0;
            for (int y = 0; y < 8; ++y)
                if (mask->isOn(x, y))
                    byte += (1 << y);

            pixels[byteOffset] = byte;
            // printf("Byte %d = %d\n", x, byte);
            ++byteOffset;
            ++colOffset;
        }
    }
}


/**
 * Clear whole screen.
 */
void DeviceOled::clearDisplay(VisualizationClient &visualizationClient)
{
    // clear the screen area of the current window

    unsigned index = windowRowFrom * cols + windowColFrom;
    unsigned size  = (windowColTo - windowColFrom + 1) * (windowRowTo - windowRowFrom + 1);
    for (unsigned i = 0; i < size; ++i) {
        pixels[index + i] = 0;
    }

    // printf("OLED_FUNCTION_CLEAR_DISPLAY\n");
    currentCol = windowColFrom;
    currentRow = windowRowFrom;

    notify(visualizationClient, VALUE_CHANGE);
}


/**
 * Consume command for OLED devices
 */
bool DeviceOled::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);
    unsigned index, i, size;

    unsigned x, y;
    unsigned x_end, y_end;
    unsigned rowPixels, addRow;

    //--------------------------------------------------------------
    // Commands which are the same for OLED + LCD (same command ID)
    //--------------------------------------------------------------
    switch (p.header.function_id) {
    case OLED_128X64_FUNCTION_WRITE_LINE:
        // printf("OLED_FUNCTION_WRITE_LINE %d \n", currentRow);
        writeText(p.fullData.payload);
        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case OLED_128X64_FUNCTION_CLEAR_DISPLAY:
        clearDisplay(visualizationClient);
        return true;
    }

    if (isLcd) {
        switch (p.header.function_id) {
        case LCD_128X64_FUNCTION_WRITE_PIXELS_LOW_LEVEL:
            // uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t pixels_length, uint16_t pixels_chunk_offset, bool pixels_chunk_data[448]);

            x     = p.lcdData.x_start;
            y     = p.lcdData.y_start;
            x_end = p.lcdData.x_end;
            y_end = p.lcdData.y_end;

            rowPixels = x_end - x + 1;
            addRow    = (p.lcdData.pixels_chunk_offset / rowPixels);

            x += (p.lcdData.pixels_chunk_offset % rowPixels);
            y += addRow;

//            printf("LCD_FUNCTION_WRITE %u %u %u %u; size %u; offset %4u; first pos: %2d %2d; row data: %d %d\n",
//                   p.lcdData.x_start, p.lcdData.y_start, x_end, y_end, p.lcdData.pixels_length, p.lcdData.pixels_chunk_offset,
//                   x, y, rowPixels, addRow);

            for (unsigned byte = 0; byte < 56; ++byte) {
                uint8_t bits = p.lcdData.bits[byte];
                for (i = 0; i < 8; i++) {
                    bool bit = bits & SINGLE_PIXEL[i];
                    setPixel(y, x, bit);
                    if (++x > x_end) {
                        x = p.lcdData.x_start;
                        if (++y > y_end) {
                            i = 8;
                            byte = 56;
                        }
                    }
                }
            }

            if (automaticDraw)
                notify(visualizationClient, VALUE_CHANGE);
            return true;

        case LCD_128X64_FUNCTION_READ_PIXELS_LOW_LEVEL:
            // uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t *ret_pixels_length, uint16_t *ret_pixels_chunk_offset, bool ret_pixels_chunk_data[480]

            x     = p.lcdData.x_start;
            y     = p.lcdData.y_start;
            x_end = p.lcdData.x_end;
            y_end = p.lcdData.y_end;

            rowPixels = x_end - x + 1;
            addRow    = (chunkPixelOffset / rowPixels);
            size      = rowPixels * (y_end - y + 1);  // total number of pixels

            x += (chunkPixelOffset % rowPixels);
            y += addRow;

//            printf("LCD_FUNCTION_READ %u %u %u %u; size %u; offset %4u; first pos: %2d %2d; row data: %d %d\n",
//                   p.lcdData.x_start, p.lcdData.y_start, x_end, y_end, p.lcdData.pixels_length, p.lcdData.pixels_chunk_offset,
//                   x, y, rowPixels, addRow);

            bzero(p.fullData.payload, sizeof(p.fullData.payload));
            p.header.length += 64;
            p.ushorts.value1 = size;

            // read up to 60 bytes
            for (size = 0, index = 4; size < 480; ++size)
            {
                bool bit = isPixelOn(y, x);
                if (bit)
                    p.fullData.payload[index] |= SINGLE_PIXEL[size % 8];
                if (++x > x_end) {
                    x = p.lcdData.x_start;
                    if (++y > y_end) {
                        p.ushorts.value2 = chunkPixelOffset;
                        chunkPixelOffset = 0;
//                        printf("LCD_FUNCTION_READ return_A %u %u\n", p.ushorts.value1, p.ushorts.value2);
                        return true;
                    }
                }
                if ((size + 1) % 8 == 0)
                    ++index;
            }

            p.ushorts.value2 = chunkPixelOffset;
            chunkPixelOffset += 480;

//            printf("LCD_FUNCTION_READ return_B %u %u\n", p.ushorts.value1, p.ushorts.value2);

            return true;

        case LCD_128X64_FUNCTION_GET_TOUCH_GESTURE:
            // uint8_t *ret_gesture, uint32_t *ret_duration, uint16_t *ret_pressure_max, uint16_t *ret_x_start, uint16_t *ret_y_start, uint16_t *ret_x_end, uint16_t *ret_y_end, uint32_t *ret_age);
            p.header.length += 19;
            memset(p.fullData.payload, 0, sizeof(p.fullData.payload));
            return true;

        case LCD_128X64_FUNCTION_GET_TOUCH_POSITION:
            // uint16_t *ret_pressure, uint16_t *ret_x, uint16_t *ret_y, uint32_t *ret_age
            p.header.length += 10;
            memset(p.fullData.payload, 0, sizeof(p.fullData.payload));
            return true;

        case LCD_128X64_FUNCTION_DRAW_BUFFERED_FRAME:
            notify(visualizationClient, VALUE_CHANGE);
            return true;

        case LCD_128X64_FUNCTION_SET_DISPLAY_CONFIGURATION:
            // uint8_t contrast, uint8_t backlight, bool invert, bool automatic_draw
            contrast  = p.fullData.payload[0];
            backlight = p.fullData.payload[1];
            inverted  = p.fullData.payload[2] ? true : false;
            automaticDraw = p.fullData.payload[3] ? true : false;
            notify(visualizationClient, VALUE_CHANGE);
            return true;

        case LCD_128X64_FUNCTION_GET_DISPLAY_CONFIGURATION:
            p.header.length += 4;
            p.fullData.payload[0] = contrast;
            p.fullData.payload[1] = backlight;
            p.fullData.payload[2] = inverted;
            p.fullData.payload[3] = automaticDraw;
            return true;
        }
    }
    else {

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
    }

    if (v2device)
        return v2device->consumeCommand(relativeTimeMs, p, visualizationClient);

    return false;
}

/**
 * OLED has no callbacks, but LCD
 */
void DeviceOled::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &vc)
{
}

} /* namespace stubserver */
