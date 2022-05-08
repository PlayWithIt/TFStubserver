/*
 * StatusLed.cpp
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

#include "StatusLed.h"

#include <utils/Log.h>

namespace stubserver {


/**
 * Default init: set LED config to HIDDEN, LED off
 */
StatusLed::StatusLed()
  : config(StatusLedConfig::LED_HIDDEN)
  , ledOn(false)
{ }

/**
 * Set specific config
 */
StatusLed::StatusLed(StatusLedConfig cfg)
  : config(cfg)
  , ledOn(cfg == StatusLedConfig::LED_ON ? true : false)
{ }


/**
 * Change the LED config:
 * OFF and ON don't need the activity, but if the cfg is ACTIVITY, the
 * led itself is set based on that value.
 */
void StatusLed::setLedConfig(StatusLedConfig cfg, bool activity)
{
    config = cfg;
    if (cfg == StatusLedConfig::LED_ACTIVITY)
        ledOn = activity;
    else if (cfg == StatusLedConfig::LED_ON)
        ledOn = true;
    else // HEARTBEAT, OFF, HIDDEN: use false
        ledOn = false;
}

/**
 * Change the LED config based on an input value from bricklet command:
 * first map the value to valid config values and then set the led.
 */
void StatusLed::setLedConfig(uint8_t cfg)
{
    if (cfg >= 0 && cfg <= 3)
        setLedConfig(static_cast<StatusLedConfig>(cfg), 0);
    else
        utils::Log() << "StatusLed::setLedConfig: invalid config value " << static_cast<int>(cfg);
}

/**
 * Change LED on/off based on bricklet or channel activity:
 * if config is ON or OFF, this method does nothing, only in state ACTIVITY
 * this updates the led state.
 */
void StatusLed::setActivity(bool on)
{
    if (config == StatusLedConfig::LED_ACTIVITY || config == StatusLedConfig::LED_HEARTBEAT)
        ledOn = on;
}


/**
 * Maps the input int into an LedState value:
 * 0 .. 3 is copied, everything else prints a warning and is mapped to 0.
 */
StatusLedConfig StatusLed::getLedConfigFromParam(uint8_t cfgIn)
{
    if (cfgIn >= 0 && cfgIn <= 3)
        return static_cast<StatusLedConfig>(cfgIn);
    utils::Log() << "StatusLed::getLedConfigFromParam: invalid status LED config value "
                 << static_cast<int>(cfgIn);
    return StatusLedConfig::LED_OFF;
}

} /* namespace stubserver */
