/*
 * Station MGMT
 *
 * Copyright (C) 2023:
 *  - Luca Cireddu IS0GVH (is0gvh@gmail.com)
 *  - Stefano Lande IS0EIR (landeste@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "config.hpp"

#include <EEPROM.h>

#include "eeprom.hpp"

Config::Config() {
    Config::mainVoltageOff =
            EEPROM.read(EEPROM_ADDRESS_CONFIG_VOLTAGE_OFF_LSB) + (EEPROM.read(EEPROM_ADDRESS_CONFIG_VOLTAGE_OFF_MSB) << 8);
    Config::mainVoltageOn =
            EEPROM.read(EEPROM_ADDRESS_CONFIG_VOLTAGE_ON_LSB) + (EEPROM.read(EEPROM_ADDRESS_CONFIG_VOLTAGE_ON_MSB) << 8);
}

Config::~Config() = default;

uint16_t Config::getMainVoltageOff() const {
    return mainVoltageOff;
}

void Config::setMainVoltageOff(uint16_t newValue) {
    Config::mainVoltageOff = newValue;

    EEPROM.write(EEPROM_ADDRESS_CONFIG_VOLTAGE_OFF_LSB, Config::mainVoltageOff & 0xFF);
    EEPROM.write(EEPROM_ADDRESS_CONFIG_VOLTAGE_OFF_MSB, (Config::mainVoltageOff >> 8) & 0xFF);
}

uint16_t Config::getMainVoltageOn() const {
    return mainVoltageOn;
}

void Config::setMainVoltageOn(uint16_t newValue) {
    Config::mainVoltageOn = newValue;

    EEPROM.write(EEPROM_ADDRESS_CONFIG_VOLTAGE_ON_LSB, Config::mainVoltageOn & 0xFF);
    EEPROM.write(EEPROM_ADDRESS_CONFIG_VOLTAGE_ON_MSB, (Config::mainVoltageOn >> 8) & 0xFF);
}
