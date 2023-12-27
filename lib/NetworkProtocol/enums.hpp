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

#ifndef STATION_MGMT__NETWORK_PROTOCOL__ENUMS__H
#define STATION_MGMT__NETWORK_PROTOCOL__ENUMS__H

#include <stdint.h>

enum class CommandType : uint8_t {
    Null = '\0',
    Ping = 'p',
    Reset = 'X',
    Telemetry = 't',
    RTCRead = 'r',
    RTCSet = 'R',
    ConfigRead = 'c',
    ConfigSet = 'C',
    OutputRead = 'o',
    OutputSet = 'O'
};

enum class ResponseType : uint8_t {
    Ack = 'A',
    Nack = 'N'
};

enum class ConfigParam : uint8_t {
    MainVoltageOff = 'o',
    MainVoltageOn = 'O'
};

#endif
