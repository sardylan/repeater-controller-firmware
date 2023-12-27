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

// #include "command.hpp"
//
// #include <IPAddress.h>
// #include <stdint.h>
//
// NetworkCommand::NetworkCommand(IPAddress ip, uint16_t port) : ip(ip), port(port) {
//     argsSize = 0;
// }
//
// NetworkCommand::NetworkCommand() : port(0) {
//     argsSize = 0;
// }
//
// NetworkCommand::NetworkCommand(const NetworkCommand& other) = default;
//
// NetworkCommand::~NetworkCommand() = default;
//
// const IPAddress& NetworkCommand::getIp() const {
//     return ip;
// }
//
// uint16_t NetworkCommand::getPort() const {
//     return port;
// }
//
// size_t NetworkCommand::getArgsSize() const {
//     return argsSize;
// }
//
// void NetworkCommand::readArgs(uint8_t* dest, size_t pos, size_t size) const {
//     memcpy(dest, reinterpret_cast<void*>(args[pos]), size);
// }
//
// void NetworkCommand::writeArgs(size_t pos, uint8_t* src, size_t size) {
//     if (size > 0)
//         memcpy(reinterpret_cast<void*>(args[pos]), src, size);
//
//     const size_t newSize = pos + size;
//     if (argsSize < newSize)
//         argsSize = newSize;
// }
//
// const uint8_t& NetworkCommand::readArg(size_t pos) const {
//     return args[pos];
// }
//
// void NetworkCommand::writeArg(size_t pos, uint8_t value) {
//     args[pos] = value;
//
//     const size_t newSize = pos + 1;
//     if (argsSize < newSize)
//         argsSize = newSize;
// }
