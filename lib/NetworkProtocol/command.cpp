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

#include "command.hpp"

#include <IPAddress.h>
#include <stdint.h>

NetworkCommand::NetworkCommand(const IPAddress& ip, const uint16_t port) : valid(true), ip(ip), port(port) {
    argsSize = 0;
}

NetworkCommand::NetworkCommand() : valid(false), port(0) {
    argsSize = 0;
}

NetworkCommand::NetworkCommand(const NetworkCommand& other) = default;

NetworkCommand::~NetworkCommand() = default;

bool NetworkCommand::isValid() const {
    return valid;
}

const IPAddress& NetworkCommand::getIp() const {
    return ip;
}

uint16_t NetworkCommand::getPort() const {
    return port;
}

size_t NetworkCommand::getArgsSize() const {
    return argsSize;
}

void NetworkCommand::readArgs(uint8_t* dest, const size_t& pos, const size_t& size) const {
    memcpy(dest, reinterpret_cast<void*>(args[pos]), size);
}

void NetworkCommand::writeArgs(const size_t& pos, const uint8_t* src, const size_t& size) {
    memcpy(reinterpret_cast<void*>(args[pos]), src, size);

    const size_t newSize = pos + size;
    if (argsSize < newSize)
        argsSize = newSize;
}

const uint8_t& NetworkCommand::readArg(const size_t& pos) const {
    return args[pos];
}

void NetworkCommand::writeArg(const size_t& pos, const uint8_t& value) {
    args[pos] = value;

    const size_t newSize = pos + 1;
    if (argsSize < newSize)
        argsSize = newSize;
}
