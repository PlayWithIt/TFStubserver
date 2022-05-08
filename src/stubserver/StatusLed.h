/*
 * StatusLed.h
 *
 * Copyright (C) 2022 Holger Grosenick
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

#ifndef STUBSERVER_STATUSLED_H_
#define STUBSERVER_STATUSLED_H_

#include <stdint.h>


namespace stubserver {

/**
 * How the status led is configured: not supported by all devices, but most of them.
 */
enum class StatusLedConfig : uint8_t {
    LED_OFF = 0,         // always off
    LED_ON  = 1,         // always on
    LED_HEARTBEAT = 2,   // show heart beat
    LED_ACTIVITY = 3,    // show activity or channel status
    LED_HIDDEN = 10      // not shown in the UI / not present in a device
};

/**
 * Bricklet status LED or channel status LED
 */
class StatusLed {
    StatusLedConfig config;
    bool            ledOn;

public:
    /**
     * Default init: set LED config to HIDDEN, LED off
     */
    StatusLed();

    /**
     * Set specific config
     */
    explicit StatusLed(StatusLedConfig cfg);

    /**
     * Maps the input int into an LedState value:
     * 0 .. 3 is copied, everything else prints a warning and is mapped to 0.
     */
    static StatusLedConfig getLedConfigFromParam(uint8_t cfgIn);

    /**
     * Query attributes.
     */
    StatusLedConfig getConfig() const {
        return config;
    }

    uint8_t getLedConfigAsInt() const {
        return static_cast<uint8_t>(config);
    }

    bool isLedOn() const {
        return ledOn;
    }

    /**
     * Change the LED config:
     * OFF and ON don't need the activity, but if the cfg is ACTIVITY, the
     * led itself is set based on that value.
     */
    void setLedConfig(StatusLedConfig cfg, bool activity);

    /**
     * Change the LED config based on an input value from bricklet command:
     * first map the value to valid config values and then set the led.
     */
    void setLedConfig(uint8_t cfg);

    /**
     * Change LED on/off based on bricklet or channel activity:
     * if config is ON or OFF, this method does nothing, only in state ACTIVITY
     * this updates the led state.
     */
    void setActivity(bool on);
};

} /* namespace stubserver */

#endif /* STUBSERVER_STATUSLED_H_ */
