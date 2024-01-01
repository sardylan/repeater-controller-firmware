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

#ifndef STATION_MGMT__ENUMS__H
#define STATION_MGMT__ENUMS__H

enum class Temperature : uint8_t {
    Normal = 0x00,
    OverTemp = 0x01,
    LowTemp = 0x02
};

enum class Battery : uint8_t {
    Normal = 0x00,
    OverVoltage = 0x01,
    UnderVoltage = 0x02,
    OverDischarge = 0x03,
    Fault = 0x04
};

enum class Charging : uint8_t {
    NoCharging = 0x00,
    Float = 0x01,
    Boost = 0x02,
    Equalization = 0x03
};

enum class Arrays : uint8_t {
    Normal = 0x00,
    NoInputPower = 0x01,
    HigherVoltage = 0x02,
    VoltageError = 0x03
};

enum class Load : uint8_t {
    Light = 0x00,
    Moderate = 0x01,
    Rated = 0x02,
    Overload = 0x03
};

#endif
