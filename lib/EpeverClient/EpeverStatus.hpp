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

#ifndef STATION_MGMT__EPEVER_CLIENT__EPEVER_STATUS__H
#define STATION_MGMT__EPEVER_CLIENT__EPEVER_STATUS__H

enum class Temperature {
    Normal = 0x00,
    OverTemp = 0x01,
    LowTemp = 0x02
};

enum class Battery {
    Normal = 0x00,
    OverVoltage = 0x01,
    UnderVoltage = 0x02,
    OverDischarge = 0x03,
    Fault = 0x04
};

enum class Charging {
    NoCharging = 0x00,
    Float = 0x01,
    Boost = 0x02,
    Equalization = 0x03
};

enum class Arrays {
    Normal = 0x00,
    NoInputPower = 0x01,
    HigherVoltage = 0x02,
    VoltageError = 0x03
};

enum class Load {
    Light = 0x00,
    Moderate = 0x01,
    Rated = 0x02,
    Overload = 0x03
};

class EpeverStatus {
    public:

        EpeverStatus(
            bool wrongVoltageIdentification,
            Temperature temperature,
            Battery battery,
            Charging charging,
            Arrays arrays,
            Load load);

        EpeverStatus();

        EpeverStatus(const EpeverStatus& other);

        [[nodiscard]]
        bool isValid() const;

        [[nodiscard]]
        bool isWrongVoltageIdentification() const;

        [[nodiscard]]
        Temperature getTemperature() const;

        [[nodiscard]]
        Battery getBattery() const;

        [[nodiscard]]
        Charging getCharging() const;

        [[nodiscard]]
        Arrays getArrays() const;

        [[nodiscard]]
        Load getLoad() const;

    private:

        bool valid;

        bool wrongVoltageIdentification;
        Temperature temperature;
        Battery battery;
        Charging charging;
        Arrays arrays;
        Load load;
};

const char* temperatureToString(const Temperature& temperature);

const char* batteryToString(const Battery& battery);

const char* chargingToString(const Charging& charging);

const char* arraysToString(const Arrays& arrays);

const char* loadToString(const Load& load);

#endif
