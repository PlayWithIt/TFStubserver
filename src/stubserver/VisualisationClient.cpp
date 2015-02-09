/*
 * VisualisationClient.cpp
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
#include <strings.h>

#include "VisualisationClient.h"

namespace stubserver {


VisibleDeviceState::~VisibleDeviceState() { }

VisualisationClient::~VisualisationClient() { }

/**
 * Do nothing.
 */
void VisualisationClient::notify(const VisibleDeviceState &hint) {

}

/**
 * Default impl: return false.
 */
bool VisualisationClient::useAsInputSource() const {
    return false;
}

/**
 * Default impl.
 */
int64_t VisualisationClient::getInputState() const {
    return 0;
}


SensorState::SensorState()
  : VisibleDeviceState(0)
  , sensorValue(0)
  , counter(0)
  , min(0)
  , max(4095)
{
}

SensorState::SensorState(int _min, int _max)
  : VisibleDeviceState(0)
  , sensorValue(0)
  , counter(0)
  , min(_min)
  , max(_max)
{
}


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


}
