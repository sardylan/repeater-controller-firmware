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

Config::Config()
{
    mainVoltageOff = readFromEEPROM(EEPROM_ADDRESS_CONFIG_VOLTAGE_OFF);
    mainVoltageOn = readFromEEPROM(EEPROM_ADDRESS_CONFIG_VOLTAGE_ON);
}

Config::~Config() = default;

float Config::getMainVoltageOff() const
{
    return mainVoltageOff;
}

void Config::setMainVoltageOff(const float newValue)
{
    mainVoltageOff = newValue;
    writeToEEPROM(EEPROM_ADDRESS_CONFIG_VOLTAGE_OFF, mainVoltageOff);
}

float Config::getMainVoltageOn() const
{
    return mainVoltageOn;
}

void Config::setMainVoltageOn(float newValue)
{
    mainVoltageOn = newValue;
    writeToEEPROM(EEPROM_ADDRESS_CONFIG_VOLTAGE_ON, mainVoltageOn);
}

float Config::readFromEEPROM(const int address)
{
    float value;
    auto* valuePointer = reinterpret_cast<uint8_t*>(&value);
    valuePointer[0] = EEPROM.read(address + 0);
    valuePointer[1] = EEPROM.read(address + 1);
    valuePointer[2] = EEPROM.read(address + 2);
    valuePointer[3] = EEPROM.read(address + 3);
    return value;
}

void Config::writeToEEPROM(const int address, const float& value)
{
    const auto* valuePointer = reinterpret_cast<const uint8_t*>(&value);
    EEPROM.write(address + 0, valuePointer[0]);
    EEPROM.write(address + 1, valuePointer[1]);
    EEPROM.write(address + 2, valuePointer[2]);
    EEPROM.write(address + 3, valuePointer[3]);
}
