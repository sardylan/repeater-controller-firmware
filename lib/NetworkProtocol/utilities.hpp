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

#ifndef STATION_MGMT__NETWORK_PROTOCOL__UTILITIES__H
#define STATION_MGMT__NETWORK_PROTOCOL__UTILITIES__H

#include <stdint.h>

#include "enums.hpp"

CommandType commandTypeParse(uint8_t b);

uint8_t commandTypeSerialize(CommandType c);

const char* commandTypeToString(const CommandType& c);

ResponseType responseTypeParse(uint8_t b);

uint8_t responseTypeSerialize(ResponseType r);

const char* responseTypeToString(const ResponseType& r);

ConfigParam configParamParse(uint8_t b);

uint8_t configParamSerialize(ConfigParam p);

const char* configParamToString(const ConfigParam& p);

#endif
