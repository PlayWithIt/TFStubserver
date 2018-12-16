/*
 * VisualizationClient.cpp
 *
 * Copyright (C) 2015 Holger Grosenick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <string.h>
#include <strings.h>

#include "VisualizationClient.h"

namespace stubserver {

// each byte has just one single pixel set
const uint8_t DisplayState::SINGLE_PIXEL[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };


VisibleDeviceState::~VisibleDeviceState() { }

VisualizationClient::~VisualizationClient() { }

/**
 * Do nothing.
 */
void VisualizationClient::notify(const VisibleDeviceState &hint) {

}

/**
 * Default impl: return false.
 */
bool VisualizationClient::useAsInputSource(unsigned sn) const {
    return false;
}

/**
 * Default impl.
 */
int64_t VisualizationClient::getInputState(unsigned sn) const {
    return 0;
}


SensorState::SensorState()
  : VisibleDeviceState(0)
  , min(0)
  , max(4095)
  , sensorValue(0)
  , counter(0)
{
}

SensorState::SensorState(int _min, int _max)
  : VisibleDeviceState(0)
  , min(_min)
  , max(_max)
  , sensorValue(0)
  , counter(0)
{
}


DualButtonState::DualButtonState()
  : VisibleDeviceState(0)
  , buttonStates(0)
  , ledOn_l(false)
  , ledOn_r(false)
  , red(0)
  , green(0)
  , blue(0)
{
}

/**
 * Unknown time so far.
 */
RealTimeClockState::RealTimeClockState()
    : VisibleDeviceState(0)
    , brickletTime(0)
{ }

/**
 * Just basic init.
 */
RelayState::RelayState(unsigned n)
    : VisibleDeviceState(0), numSwitches(n)
{
    if (n > 16)
        throw std::invalid_argument("numSwitches must be <= 16, but is larger");
    bzero(switchOn, sizeof(switchOn));
}

bool RelayState::isOn(unsigned switchNo) const
{
    if (switchNo > numSwitches)
        throw std::out_of_range("RelayState::isOn: 'switchNo' is too high");
    return switchOn[switchNo];
}

/**
 * Returns a label for the switch: this can be just the switch number
 * of 'SW1' or the remote switch code.
 */
std::string RelayState::getLabel(unsigned switchNo) const
{
    if (switchNo > numSwitches)
        throw std::out_of_range("RelayState::isOn: 'switchNo' is too high");

    char buf[16];
    sprintf(buf, "%u", switchNo);
    return std::string(buf);
}

LcdState::LcdState(unsigned _cols, unsigned _lines)
   : VisibleDeviceState(0)
   , cols(_cols)
   , lines(_lines)
   , changedLine(0)
   , buttonState(0)
   , cursorX(0), cursorY(0)
   , backlightOn(false)
   , blinking(false)
   , cursorVisible(false)
{
}


/**
 * Returns the text from the given line.
 */
const std::string& LcdState::getLine(unsigned line) const
{
    if (line >= lines)
        throw std::out_of_range("Line no invalid");
    return text[line];
}

/**
 * Default init.
 */
DisplayState::DisplayState(unsigned _cols, unsigned _lines)
    : VisibleDeviceState(0)
    , cols(_cols)
    , lines(_lines)
    , contrast(143)
    , inverted(false)
    , newEvent(false)
    , gesture(NOTHING)
    , duration(0)
    , x_start(0)
    , y_start(0)
    , x_end(0)
    , y_end(0)
    , time(0)
{
    clear();
}

/**
 * Clear all pixel data of the screen.
 */
void DisplayState::clear() {
    bzero(pixels, sizeof(pixels));
}

/**
 * Is a single pixel set in the screen?
 */
bool DisplayState::isPixelOn(unsigned line, unsigned col) const
{
    if (line > lines || col > cols)
        return false;

    unsigned byteNo = (line / 8) * cols + col;
    unsigned bitNo  = line & 7;

    return pixels[byteNo] & SINGLE_PIXEL[bitNo] ? true:false;
}

/**
 * Set/clear a single pixel?
 */
void DisplayState::setPixel(unsigned line, unsigned col, bool on)
{
    if (line > lines || col > cols)
        return;

    unsigned byteNo = (line / 8) * cols + col;
    unsigned bitNo  = line & 7;

    if (on)
        pixels[byteNo] |= SINGLE_PIXEL[bitNo];
    else
        pixels[byteNo] &= (0xFF - SINGLE_PIXEL[bitNo]);
}


/**
 * Start a press or a swipe (touch the display and hold).
 */
void DisplayState::startTouch(uint16_t x, uint16_t y) const
{
    x_start  = x;
    y_start  = y;
    gesture  = PRESS;
    newEvent = true;
}

/**
 * End touching the display: based on the end position the gesture is calculated!
 */
void DisplayState::endTouch(uint16_t x, uint16_t y) const
{
    x_end = x;
    y_end = y;

    if (static_cast<int>(x_end - x_start) < -1)
        gesture = SWIPE_LEFT;
    else if (static_cast<int>(x_end - x_start) > 1)
        gesture = SWIPE_RIGHT;
    else if (static_cast<int>(y_end - y_start) < -1)
        gesture = SWIPE_UP;
    else if (static_cast<int>(y_end - y_start) > 1)
        gesture = SWIPE_DOWN;
}

}
