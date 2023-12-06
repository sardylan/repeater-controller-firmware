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

#include "EpeverStatus.hpp"

EpeverStatus::EpeverStatus(bool wrongVoltageIdentification,
                           Temperature temperature,
                           Battery battery,
                           Charging charging,
                           Arrays arrays,
                           Load load) :
        valid(true),
        wrongVoltageIdentification(wrongVoltageIdentification),
        temperature(temperature),
        battery(battery),
        charging(charging),
        arrays(arrays),
        load(load) {
}

EpeverStatus::EpeverStatus() :
        valid(false),
        wrongVoltageIdentification(false),
        temperature(Temperature::Normal),
        battery(Battery::Normal),
        charging(Charging::NoCharging),
        arrays(Arrays::NoInputPower),
        load(Load::Light) {
}

EpeverStatus::EpeverStatus(const EpeverStatus &other) = default;

bool EpeverStatus::isValid() const {
    return valid;
}

bool EpeverStatus::isWrongVoltageIdentification() const {
    return wrongVoltageIdentification;
}

Temperature EpeverStatus::getTemperature() const {
    return temperature;
}

Battery EpeverStatus::getBattery() const {
    return battery;
}

Charging EpeverStatus::getCharging() const {
    return charging;
}

Arrays EpeverStatus::getArrays() const {
    return arrays;
}

Load EpeverStatus::getLoad() const {
    return load;
}

const char *temperatureToString(const Temperature &temperature) {
    switch (temperature) {

        case Temperature::Normal:
            return "Normal";
        case Temperature::OverTemp:
            return "OverTemp";
        case Temperature::LowTemp:
            return "LowTemp";
    }

    return nullptr;
}

const char *batteryToString(const Battery &battery) {
    switch (battery) {

        case Battery::Normal:
            return "Normal";
        case Battery::OverVoltage:
            return "OverVoltage";
        case Battery::UnderVoltage:
            return "UnderVoltage";
        case Battery::OverDischarge:
            return "OverDischarge";
        case Battery::Fault:
            return "Fault";
    }

    return nullptr;
}

const char *chargingToString(const Charging &charging) {
    switch (charging) {

        case Charging::NoCharging:
            return "NoCharging";
        case Charging::Float:
            return "Float";
        case Charging::Boost:
            return "Boost";
        case Charging::Equalization:
            return "Equalization";
    }

    return nullptr;
}

const char *arraysToString(const Arrays &arrays) {
    switch (arrays) {

        case Arrays::Normal:
            return "Normal";
        case Arrays::NoInputPower:
            return "NoInputPower";
        case Arrays::HigherVoltage:
            return "HigherVoltage";
        case Arrays::VoltageError:
            return "VoltageError";
    }

    return nullptr;
}

const char *loadToString(const Load &load) {
    switch (load) {

        case Load::Light:
            return "Light";
        case Load::Moderate:
            return "Moderate";
        case Load::Rated:
            return "Rated";
        case Load::Overload:
            return "Overload";
    }

    return nullptr;
}
