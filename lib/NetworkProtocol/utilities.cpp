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

#include "utilities.hpp"

#include <stdint.h>

CommandType commandTypeParse(uint8_t b) {
    return static_cast<CommandType>(b);
}

uint8_t commandTypeSerialize(CommandType c) {
    return static_cast<uint8_t>(c);
}

const char* commandTypeToString(const CommandType& c) {
    switch (c) {
        case CommandType::Ping:
            return "Ping";
        case CommandType::Reset:
            return "Reset";
        case CommandType::Telemetry:
            return "Telemetry";
        case CommandType::RTCRead:
            return "RTCRead";
        case CommandType::RTCSet:
            return "RTCSet";
        case CommandType::ConfigRead:
            return "ConfigRead";
        case CommandType::ConfigSet:
            return "ConfigSet";
        case CommandType::OutputRead:
            return "OutputRead";
        case CommandType::OutputSet:
            return "OutputSet";

        default:
            return "";
    }
}

ResponseType responseTypeParse(uint8_t b) {
    return static_cast<ResponseType>(b);
}

uint8_t responseTypeSerialize(ResponseType r) {
    return static_cast<uint8_t>(r);
}

const char* responseTypeToString(const ResponseType& r) {
    switch (r) {
        case ResponseType::Ack:
            return "Ack";
        case ResponseType::Nack:
            return "Nack";

        default:
            return "";
    }
}

ConfigParam configParamParse(uint8_t b) {
    return static_cast<ConfigParam>(b);
}

uint8_t configParamSerialize(ConfigParam p) {
    return static_cast<uint8_t>(p);
}

const char* configParamToString(const ConfigParam& p) {
    switch (p) {
        case ConfigParam::MainVoltageOff:
            return "MainVoltageOff";
        case ConfigParam::MainVoltageOn:
            return "MainVoltageOn";

        default:
            return "";
    }
}