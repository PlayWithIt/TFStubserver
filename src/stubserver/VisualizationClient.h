/*
 * VisualizationClient.h
 *
 * Copyright (C) 2015-2022 Holger Grosenick
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
#include <ctime>
#include <vector>

#include "StatusLed.h"

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


// Sensor numbers for the AirQualityBricklet
#define AIR_QUALITY_IAQ         0
#define AIR_QUALITY_TEMP        1
#define AIR_QUALITY_HUMIDITY    2
#define AIR_QUALITY_PRESSURE    3


/**
 * A short living object which will be destroyed immediately after the
 * method {@link VisualizationClient::notify()} returns.
 */
class VisibleDeviceState
{
    unsigned changeCode;
    unsigned internalSensorNo;

    /**
     * Status led config is device specific value: some devices use 0,1 or 0,1,2,3.
     */
    StatusLed statusLed;

public:
    explicit VisibleDeviceState(unsigned c)
       : changeCode(c), internalSensorNo(0), statusLed(StatusLedConfig::LED_HEARTBEAT) { }

    explicit VisibleDeviceState(unsigned c, unsigned sn)
       : changeCode(c), internalSensorNo(sn), statusLed(StatusLedConfig::LED_HEARTBEAT) { }

    virtual ~VisibleDeviceState();

    /**
     * If an event is triggered with this code, the object gets invalidated.
     */
    static const unsigned DISCONNECT   = 0;
    static const unsigned CONNECTED    = 1;
    static const unsigned VALUE_CHANGE = 2;

    /**
     * Function code for LED status change.
     */
    static const unsigned LED_CHANGE = 3;


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
     * Changes the value of the status LED: some devices just have on/off,
     * some other on (1) / off (0), activity status (2) or channel status (3)
     */
    void setStatusLedConfig(uint8_t cfg) {
        statusLed.setLedConfig(cfg);
    }
    void setStatusLedConfig(StatusLedConfig cfg) {
        statusLed.setLedConfig(cfg, 0);
    }

    /**
     * Returns true if there is any light on the LED in some cases.
     */
    bool isStatusLedOn() const {
        return statusLed.getConfig() != StatusLedConfig::LED_OFF;
    }

    StatusLedConfig getStatusLedConfig() const {
        return statusLed.getConfig();
    }

    /**
     * Change the internal sensor number for devices that have more than one.
     * The default is 0 for the first (main) sensor.
     */
    void setInternalSensorNo(unsigned sn) {
        internalSensorNo = sn;
    }

    /**
     * Notify a change of a special sensor.
     */
    void notify(unsigned sn, VisualizationClient &client, unsigned code = VALUE_CHANGE) {
        internalSensorNo = sn;
        changeCode = code;
        client.notify(*this);
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
    static const unsigned CURSOR_CHANGE = 10;
    static const unsigned LIGHT_CHANGE  = 11;
    static const unsigned TEXT_CHANGE   = 12;
    static const unsigned CLEAR_SCREEN  = 13;
    static const unsigned CUSTOM_CHAR   = 14;   // change custom char definition

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
 * The state of a display: OLED screen or LCD 128x64.
 */
class DisplayState : public VisibleDeviceState
{
public:
    // values 1,2,4,8,16,32,64,128
    static const uint8_t SINGLE_PIXEL[8];

    enum Gesture : char {
        NOTHING     = ' ',   // init
        PRESS       = 'P',   // simple press, no swipe gesture
        SWIPE_LEFT  = '<',
        SWIPE_RIGHT = '>',
        SWIPE_UP    = '^',
        SWIPE_DOWN  = 'v',
    };


    /**
     * Default init with screen size.
     */
    DisplayState(unsigned _cols, unsigned _lines);

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

    uint8_t getContrast() const {
        return contrast;
    }

    bool isInverted() const {
        return inverted;
    }

    Gesture getGesture() const {
        return gesture;
    }

    unsigned getTouchX() const {
        return x_start;
    }

    unsigned getTouchY() const {
        return y_start;
    }


    /**
     * Is a single pixel set in the screen?
     */
    bool isPixelOn(unsigned line, unsigned col) const;

    /**
     * Start a press or a swipe (touch the display and hold).
     */
    void startTouch(uint16_t x, uint16_t y) const;

    /**
     * End touching the display: based on the end position the gesture is calculated!
     */
    void endTouch(uint16_t x, uint16_t y) const;

protected:
    const unsigned cols, lines;

    uint8_t pixels[128 * 8];  // 1 byte holds the value for 8 pixels in vertical order
    uint8_t contrast;
    uint8_t backlight;
    bool    inverted;

    // for touch displays - they have a "const" object but are allowed to update
    // these event attributes.
    mutable bool     pressedState; // currently touch area is pressed ?
    mutable Gesture  gesture;
    mutable uint32_t duration;     // in ms
    mutable uint16_t x_start;
    mutable uint16_t y_start;
    mutable uint16_t x_end;
    mutable uint16_t y_end;
    mutable uint64_t time;         // time when pressed

    /**
     * Set/clear a single pixel?
     */
    void setPixel(unsigned line, unsigned col, bool on);
    void clear();
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

    /**
     * Returns the current "sensor" value.
     */
    int getSensorValue() const {
        return sensorValue;
    }

    /**
     * Returns the "sensor" value from channel 1.
     */
    int getSensorValue1() const {
        return sensorValue1;
    }

    /**
     * Returns the "sensor" value from channel 2.
     */
    int getSensorValue2() const {
        return sensorValue2;
    }

    /**
     * Returns the "sensor" value from channel 3.
     */
    int getSensorValue3() const {
        return sensorValue3;
    }

    /**
     * Returns the "sensor" value from channel 4.
     */
    int getSensorValue4() const {
        return sensorValue4;
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

    int getZeroPoint() const {
        return zeroPoint;
    }

protected:
    int      min, max;
    int      sensorValue;
    int      sensorValue1;
    int      sensorValue2;
    int      sensorValue3;
    int      sensorValue4;
    int      zeroPoint;                 // defines an offset (0-value)
    unsigned counter;
    uint8_t  led1, led2, led3, led4;    // multiple purpose
};


/**
 * Dual button state or RGB Led Button state: the LED button is like the "left" button
 * of the dual button state. The RGB button also has a status led but an additional color value.
 */
class DualButtonState : public VisibleDeviceState
{
public:
    /**
     * Default init.
     */
    DualButtonState();

    /**
     * Returns the current button states: bit 0 is left, bit 1 right button.
     */
    int getButtonStates() const {
        return buttonStates;
    }

    // test if left or right LED is ON (status LED).
    bool isLedOn_l() const {
        return ledOn_l;
    }

    bool isLedOn_r() const {
        return ledOn_r;
    }

    /**
     * @brief getRed return current color value for red
     */
    unsigned getRed() const {
        return red;
    }

    /**
     * @brief getGreen return current color value for green
     */
    unsigned getGreen() const {
        return green;
    }

    /**
     * @brief getBlue return current color value for blue
     */
    unsigned getBlue() const {
        return blue;
    }

protected:
    int  buttonStates;
    bool ledOn_l;
    bool ledOn_r;
    uint8_t red, green, blue;   // RGB color of single button
};


/**
 * Holds the date of OutdoorWeather bricklet with multiple connected sensors.
 */
class OutdoorWeatherState : public VisibleDeviceState
{
public:
    OutdoorWeatherState();
    virtual ~OutdoorWeatherState();

    /**
     * The visible part of the OutdoorWeather state.
     */
    class BaseData {
        int         temperature;
        unsigned    humidity;
        unsigned    id;             // ID >= 1000 is for station objects

    protected:
        void setTemperature(int t) {
            temperature = t;
        }

        void setHumidity(unsigned h) {
            humidity = h;
        }

    public:
        BaseData(unsigned id);
        virtual ~BaseData();

        int getTemperatoure() const {
            return temperature;
        }

        unsigned getHumidity() const {
            return humidity;
        }

        unsigned getId() const {
            return id;
        }
    };

    /**
     * Get the state for the current internal sensor.
     */
    const BaseData& getCurrentState() const;

    /**
     * Get the state for a given internal sensor.
     */
    const BaseData& getState(unsigned sensorNo) const;

    /**
     * Returns the number of sensors in the list.
     */
    unsigned getNumSensors() const;

protected:
    std::vector<BaseData*> sensors;
};


/**
 * Holds date/time of a real-time clock bricklet.
 */
class RealTimeClockState : public VisibleDeviceState
{
public:
    RealTimeClockState();

protected:
    time_t brickletTime;
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
    bool isLedOn(unsigned switchNo) const;

    /**
     * Sets 1 bit per switch in the response:
     * bit 0 = switch 0 (value = 1)
     * bit 1 = switch 1 (value = 2)
     * bit 2 = switch 2 (value = 4)
     * ...
     */
    unsigned getSwitchStates() const {
        return switchStates;
    }

    /**
     * Returns a label for the switch: this can be just the switch number
     * of 'SW1' or the remote switch code.
     */
    virtual std::string getLabel(unsigned switchNo) const;

    /**
     * Return the led config for a given channel.
     */
    StatusLedConfig getLedConfig(unsigned channel) const;

protected:
    unsigned       numSwitches;

    /**
     * Changes the LED config for a channel and updates the LED state
     * based on the new config.
     */
    void setLedConfig(uint8_t channel, StatusLedConfig config);

    /**
     * Updates the bitValues and switchOn: just one bit
     */
    void setSwitch(unsigned switchNo, bool on);

    /**
     * Updates the bitValues and switchOn, all bits states.
     */
    void setSwitchStates(unsigned states);

private:
    unsigned       switchStates;     // one bit per switch
    bool           switchOn[16];     // one flag per switch, max 16 switches
    StatusLed      channelLed[16];   // some relays have one led per channel
};

}

#endif /* STUBSERVER_VISUALIZATIONCLIENT_H_ */
