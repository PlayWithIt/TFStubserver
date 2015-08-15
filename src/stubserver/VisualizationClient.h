/*
 * VisualizationClient.h
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

#ifndef STUBSERVER_VISUALIZATIONCLIENT_H_
#define STUBSERVER_VISUALIZATIONCLIENT_H_

#include <string>


namespace stubserver {

class VisibleDeviceState;

/**
 * A special in-memory client that can register to a device and receive events if some
 * device states change (e.g. LCD text or sensor values). There maybe one client per device
 * (client can be specific). It is even possible that some devices have a client, some not.
 * <P>
 * The client can be input and/or output: if {@link #useAsInputSource()} returns true
 * the value provider is ignored and the value is read via {@link #getInputState()}.
 */
class VisualizationClient
{
public:
    virtual ~VisualizationClient();

    /**
     * This method is called per state change, it should not consume too much CPU
     * but just read the device state or copy the change information.
     */
    virtual void notify(const VisibleDeviceState &hint);

    /**
     * A simulated sensor needs simulated values which can come from a ValueProvider
     * or from the visualization, choose the source by this function. The default
     * implementation returns false: use ValueProvider.
     *
     * @param sensorNo the internal sensor number for devices which have more than
     *        one internal sensor. E.g. temperature IR has ambient and object
     *        temperature: 2 internal sensors.
     */
    virtual bool useAsInputSource(unsigned sensorNo = 0) const;

    /**
     * Return a device specific state (0 in default impl).
     * @param sensorNo the internal sensor number for devices which have more than one
     */
    virtual int64_t getInputState(unsigned sensorNo = 0) const;
};


/**
 * A short living object which will be destroyed immediately after the
 * method {@link VisualizationClient::notify()} returns.
 */
class VisibleDeviceState
{
    unsigned changeCode;
    unsigned internalSensorNo;

public:
    explicit VisibleDeviceState(unsigned c)
       : changeCode(c), internalSensorNo(0) { }

    explicit VisibleDeviceState(unsigned c, unsigned sn)
       : changeCode(c), internalSensorNo(sn) { }

    virtual ~VisibleDeviceState();

    /**
     * If an event is triggered with this code, the object gets invalidated.
     * If a VisualizationClient holds a reference to a StateChangeHint, it
     * should clear this reference and may not use it any more.
     */
    static const unsigned DISCONNECT   = 0;
    static const unsigned CONNECTED    = 1;
    static const unsigned VALUE_CHANGE = 2;

    void setChangeCode(unsigned c) {
        changeCode = c;
    }

    bool isDisconnected() const {
        return DISCONNECT == changeCode;
    }

    unsigned getChangeCode() const {
        return changeCode;
    }

    unsigned getInternalSensorNo() const {
        return internalSensorNo;
    }

    /**
     * Change the internal sensor number for devices that have more than one.
     * The default is 0 for the first (main) sensor.
     */
    void setInternalSensorNo(unsigned sn) {
        internalSensorNo = sn;
    }

    void notify(VisualizationClient &client, unsigned code = VALUE_CHANGE) {
        changeCode = code;
        client.notify(*this);
    }
};

/**
 * The state of an LCD screen.
 */
class LcdState : public VisibleDeviceState
{
public:
    static const unsigned MAX_LINES     = 5;

    // possible change events
    static const unsigned CURSOR_CHANGE = 5;
    static const unsigned LIGHT_CHANGE  = 6;
    static const unsigned TEXT_CHANGE   = 7;
    static const unsigned CLEAR_SCREEN  = 8;
    static const unsigned CUSTOM_CHAR   = 9;   // change custom char definition

    /**
     * Default init with screen size.
     */
    LcdState(unsigned _cols, unsigned _lines);

    /**
     * Returns the number of display columns.
     */
    unsigned getCols() const {
        return cols;
    }

    /**
     * Returns the number of display lines.
     */
    unsigned getLines() const {
        return lines;
    }

    /**
     * Returns the text from the given line.
     */
    const std::string& getLine(unsigned line) const;

    /**
     * Return the char definition of the given customer char as an array of
     * 8 bytes. The index must be in the customer char range (0..7) otherwise
     * NULL will be returned.
     */
    virtual const uint8_t* getCustomerChar(unsigned index) const = 0;

    /**
     * Return the changed line: 0..MAX_LINES-1 means that just one line
     * was changed, -1 means that the whole screen was changed.
     */
    int getChangedLine() const {
        return changedLine;
    }

    unsigned getCursorX() const {
        return cursorX;
    }

    unsigned getCursorY() const {
        return cursorY;
    }

    bool isBacklightOn() const {
        return backlightOn;
    }

    bool isBlinking() const {
        return blinking;
    }

    bool isCursorVisible() const {
        return cursorVisible;
    }

protected:
    std::string text[MAX_LINES];

    const unsigned cols, lines;

    int      changedLine;
    unsigned buttonState;
    unsigned cursorX, cursorY;
    bool     backlightOn;
    bool     blinking;
    bool     cursorVisible;
};

/**
 * Virtual base class for LED strip state.
 */
class LedStripState : public VisibleDeviceState
{
public:
    static const unsigned MAX_LEDS = 320;

    LedStripState();

    /**
     * Returns the number of LEDs.
     */
    unsigned getNumLeds() const {
        return numLeds;
    }

    /**
     * Returns the number of LED changed in the last frame.
     */
    unsigned getFrameLength() const {
        return frameLength;
    }

    /**
     * Copies the actual frame data into the arguments and returns the frameLength which
     * equals to the number of colors copied into the output.
     */
    unsigned getFrameData(uint8_t r[MAX_LEDS], uint8_t g[MAX_LEDS], uint8_t b[MAX_LEDS]) const;

    /**
     * Return the char definition of the given customer char as an array of
     * 8 bytes. The index must be in the customer char range (0..7) otherwise
     * NULL will be returned.
     */
    int getLedColor(unsigned index) const;

protected:
    unsigned numLeds;
    unsigned frameLength;

    // pixel data
    uint8_t r[MAX_LEDS];
    uint8_t g[MAX_LEDS];
    uint8_t b[MAX_LEDS];
};

/**
 * A simple sensor with one value in the range 0 .. 4095 (typically), Potentiometers
 * can also be handled as sensors as they just return one value.
 */
class SensorState : public VisibleDeviceState
{
public:
    /**
     * Init with 0.
     */
    SensorState();
    SensorState(int _min, int _max);

    static const unsigned LED_CHANGE = 10;

    /**
     * Returns the current "sensor" value.
     */
    int getSensorValue() const {
        return sensorValue;
    }

    /**
     * Changes the value of the status LED.
     */
    void setStatusLedOn(bool on) {
        statusLedOn = on;
    }

    bool isStatusLedOn() const {
        return statusLedOn;
    }

    /**
     * Optionally used: flag counter or something like this.
     */
    unsigned getCounter() const {
        return counter;
    }

    int getMin() const {
        return min;
    }

    int getMax() const {
        return max;
    }

protected:
    int      sensorValue;
    unsigned counter;
    int      min, max;
    bool     statusLedOn;
};

/**
 * A simple relay state for a set of max 16 switches.
 */
class RelayState : public VisibleDeviceState
{
public:
    /**
     * Init with number of switches and set all switches off.
     */
    RelayState(unsigned num);

    unsigned getNumSwitches() const {
        return numSwitches;
    }

    bool isOn(unsigned switchNo) const;

    /**
     * Returns a label for the switch: this can be just the switch number
     * of 'SW1' or the remote switch code.
     */
    virtual std::string getLabel(unsigned switchNo) const;

protected:
    unsigned numSwitches;
    bool     switchOn[16];          // one flag per switch, max 16 switches
};

}

#endif /* STUBSERVER_VISUALIZATIONCLIENT_H_ */
