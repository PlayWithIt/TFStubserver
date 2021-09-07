/*
 * DeviceOled.cpp
 *
 * Copyright (C) 2017 Holger Grosenick
 */

#include <bricklet_lcd_128x64.h>
#include <bricklet_oled_128x64.h>
#include <bricklet_oled_64x48.h>

#include <utils/Log.h>

#include "BrickStack.h"
#include "CharMask.h"
#include "DeviceOled.h"


namespace stubserver {

// OLED char dot matrix
static ScreenFont oledFont(true);

/**
 * Init attrs and char mask if necessary.
 */
DeviceOled::DeviceOled(unsigned _cols, unsigned _lines)
    : DisplayState(_cols, _lines)
    , windowColFrom(0), windowColTo(_cols - 1)
    , windowRowFrom(0), windowRowTo(_lines - 1)
    , currentCol(0), currentRow(0)
    , chunkPixelOffset(0)
{
}

/**
 * For new LCD with 128x64 pixels.
 */
DeviceLcdWithTouch::DeviceLcdWithTouch(unsigned _cols, unsigned _lines)
    : DeviceOled(_cols, _lines)
    , v2device(NULL)
    , lastEventTime(0)
    , touchCbperiod(0)
    , gestureCbperiod(0)
    , touchHasToChange(true)
    , gestureHasToChange(true)
    , automaticDraw(true)
{
    v2device = new V2Device(NULL, this, true);
    contrast = 14;
    backlight = 100;
}

DeviceLcdWithTouch::~DeviceLcdWithTouch() {
    delete v2device;
}


/**
 * print text: full data is the io packet with column, line and text.
 */
void DeviceOled::writeText(uint8_t fulldata[])
{
    uint8_t line = fulldata[0];
    uint8_t ind  = fulldata[1];
    unsigned colOffset  = ind * 6;  // char has 6 pixels + 1 pixel free
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
                    byte |= (1 << y);

            pixels[byteOffset] = byte;
            // printf("Byte %d = %d\n", x, byte);
            ++byteOffset;
            ++colOffset;

            if (byteOffset >= 128*8) {
                utils::Log() << "DeviceOled::writeText: reached end of buffer";
                return;
            }
        }
        ++byteOffset;  // 1 pixel free -> also one byte which is vertical
        ++colOffset;
    }
}


/**
 * Clear whole screen.
 */
void DeviceOled::clearDisplay(VisualizationClient &visualizationClient)
{
    // printf("OLED_FUNCTION_CLEAR_DISPLAY %u..%u  %u..%u\n", windowColFrom, windowColTo, windowRowFrom, windowRowTo);

    // clear the screen area of the current window: for old OLED the current window
    // might be relevant, but we ignore that here !
    clear();

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

    return false;
}


/**
 * OLED has no callbacks, but LCD
 */
void DeviceOled::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &vc)
{
}


/**
 * Consume command for OLED devices
 */
bool DeviceLcdWithTouch::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
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
    case LCD_128X64_FUNCTION_WRITE_LINE:
        // printf("OLED_FUNCTION_WRITE_LINE %d \n", currentRow);
        writeText(p.fullData.payload);
        notify(visualizationClient, VALUE_CHANGE);
        return true;

    case LCD_128X64_FUNCTION_CLEAR_DISPLAY:
        clearDisplay(visualizationClient);
        return true;

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

        // printf("LCD_FUNCTION_READ return_B %u %u\n", p.ushorts.value1, p.ushorts.value2);

        return true;

    case LCD_128X64_FUNCTION_GET_TOUCH_GESTURE:
        // TODO
        // uint8_t *ret_gesture, uint32_t *ret_duration, uint16_t *ret_pressure_max, uint16_t *ret_x_start, uint16_t *ret_y_start, uint16_t *ret_x_end, uint16_t *ret_y_end, uint32_t *ret_age);
        p.header.length += 19;
        memset(p.fullData.payload, 0, sizeof(p.fullData.payload));
        return true;

    case LCD_128X64_FUNCTION_GET_TOUCH_POSITION:
         // TODO
         // uint16_t *ret_pressure, uint16_t *ret_x, uint16_t *ret_y, uint32_t *ret_age
         p.header.length += 10;

         p.ushorts.value1 = 5;
         p.ushorts.value2 = this->x_start;
         p.ushorts.values[0] = this->x_end;
         p.ushorts.values[1] = 20;
         p.ushorts.values[2] = 0;
         return true;

    case LCD_128X64_FUNCTION_SET_TOUCH_POSITION_CALLBACK_CONFIGURATION:
        // uint32_t period, bool value_has_to_change
        touchCbperiod = p.callbackConfigInt.period;
        touchHasToChange = p.callbackConfigInt.value_has_to_change;
        utils::Log::log("LCD touch period set to", touchCbperiod);
        return true;

    case LCD_128X64_FUNCTION_GET_TOUCH_POSITION_CALLBACK_CONFIGURATION:
        // uint32_t period, bool value_has_to_change
        p.header.length += 5;
        p.callbackConfigInt.period = touchCbperiod;
        p.callbackConfigInt.value_has_to_change = touchHasToChange;
        return true;

    case LCD_128X64_FUNCTION_SET_TOUCH_GESTURE_CALLBACK_CONFIGURATION:
        // uint32_t period, bool value_has_to_change
        gestureCbperiod = p.callbackConfigInt.period;
        gestureHasToChange = p.callbackConfigInt.value_has_to_change;
        utils::Log::log("LCD gesture period set to", gestureCbperiod);
        return true;

    case LCD_128X64_FUNCTION_GET_TOUCH_GESTURE_CALLBACK_CONFIGURATION:
        // uint32_t period, bool value_has_to_change
        p.header.length += 5;
        p.callbackConfigInt.period = gestureCbperiod;
        p.callbackConfigInt.value_has_to_change = gestureHasToChange;
        return true;

    case LCD_128X64_FUNCTION_SET_GUI_BUTTON_PRESSED_CALLBACK_CONFIGURATION:
        // TODO
        return true;

    case LCD_128X64_FUNCTION_REMOVE_ALL_GUI:
        // TODO
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

    return v2device->consumeCommand(relativeTimeMs, p, visualizationClient);
}


/**
 * OLED has no callbacks, but LCD
 */
void DeviceLcdWithTouch::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &vc)
{
    if (pressedState && touchCbperiod > 0)
    {
        // printf("LCD event: %c  %u  %u\n", gesture, x_end, y_end);

        if (lastEventTime + touchCbperiod < relativeTimeMs) {

            // uint16_t pressure, uint16_t x, uint16_t y, uint32_t age
            IOPacket packet(uid, LCD_128X64_CALLBACK_TOUCH_POSITION, 10);

            packet.ushorts.value1 = 5;
            packet.ushorts.value2 = this->x_start;
            packet.ushorts.values[0] = this->y_start;
            packet.ushorts.values[1] = 50;
            packet.ushorts.values[2] = 0;

            brickStack->dispatchCallback(packet);
            lastEventTime = relativeTimeMs;
        }
    }
    else if (gesture > PRESS) {
        // trigger gesture callback

        // uint8_t *ret_gesture, uint32_t *ret_duration, uint16_t *ret_pressure_max, uint16_t *ret_x_start, uint16_t *ret_y_start, uint16_t *ret_x_end, uint16_t *ret_y_end, uint32_t *ret_age
        IOPacket packet(uid, LCD_128X64_CALLBACK_TOUCH_GESTURE, 10);

        packet.ushorts.value1 = 5;
        packet.ushorts.value2 = this->x_start;
        packet.ushorts.values[0] = this->y_start;
        packet.ushorts.values[1] = 50;
        packet.ushorts.values[2] = 0;

        // TODO
        // brickStack->dispatchCallback(packet);

        gesture = NOTHING;
    }
}

} /* namespace stubserver */
