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

#include "EpeverData.hpp"

EpeverData::EpeverData(float panelVoltage, float panelCurrent, float batteryVoltage, float batteryChargeCurrent) :
        valid(true),
        panelVoltage(panelVoltage),
        panelCurrent(panelCurrent),
        batteryVoltage(batteryVoltage),
        batteryChargeCurrent(batteryChargeCurrent) {
}

EpeverData::EpeverData() :
        valid(false),
        panelVoltage(0),
        panelCurrent(0),
        batteryVoltage(0),
        batteryChargeCurrent(0) {
}

EpeverData::EpeverData(const EpeverData &other) = default;

bool EpeverData::isValid() const {
    return valid;
}

float EpeverData::getPanelVoltage() const {
    return panelVoltage;
}

float EpeverData::getPanelCurrent() const {
    return panelCurrent;
}

float EpeverData::getBatteryVoltage() const {
    return batteryVoltage;
}

float EpeverData::getBatteryChargeCurrent() const {
    return batteryChargeCurrent;
}
