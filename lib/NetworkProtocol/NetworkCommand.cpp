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

#include "NetworkCommand.hpp"

#include <Print.h>

NetworkCommand::NetworkCommand(const Command command, const IPAddress &ip, const uint16_t port) :
    valid(true), ip(ip), port(port), command(command)
{
    argsSize = 0;
}

NetworkCommand::NetworkCommand() : valid(false), ip(), port(0), command(Command::Null) { argsSize = 0; }

NetworkCommand::NetworkCommand(const NetworkCommand &other) = default;

NetworkCommand::~NetworkCommand() = default;

bool NetworkCommand::isValid() const { return valid; }

const IPAddress &NetworkCommand::getIp() const { return ip; }

uint16_t NetworkCommand::getPort() const { return port; }

Command NetworkCommand::getCommand() const { return command; }

uint8_t NetworkCommand::getSingleArg(const size_t pos) const { return args[pos]; }

void NetworkCommand::setSingleArg(const size_t pos, const uint8_t val)
{
    args[pos] = val;
    if (pos >= argsSize)
        argsSize = pos + 1;
}

void NetworkCommand::getArg(const size_t pos, void *val, const size_t size) const { memcpy(val, args + pos, size); }

void NetworkCommand::setArg(const size_t pos, const void *val, const size_t size)
{
    memcpy(args + pos, val, size);
    if (pos + size > argsSize)
        argsSize = pos + size;
}

size_t NetworkCommand::getArgsSize() const { return argsSize; }

void NetworkCommand::serialize(uint8_t *buffer) const
{
    memset(buffer, '\0', 1 + argsSize);
    buffer[0] = commandToByte(command);
    memcpy(buffer + 1, args, argsSize);
}

size_t NetworkCommand::printTo(Print &p) const
{
    size_t ln = p.print(commandToString(command));
    ln += p.print(" command from ");
    ln += p.print(ip);
    ln += p.print(":");
    ln += p.print(port);
    return ln;
}

Command NetworkCommand::commandFromByte(uint8_t b) { return static_cast<Command>(b); }

uint8_t NetworkCommand::commandToByte(Command c) { return static_cast<uint8_t>(c); }

const char *NetworkCommand::commandToString(Command c)
{
    switch (c)
    {

    case Command::Null:
        return "Null";
    case Command::Ping:
        return "Ping";
    case Command::Reset:
        return "Reset";
    case Command::Telemetry:
        return "Telemetry";
    case Command::RTCRead:
        return "RTCRead";
    case Command::RTCSet:
        return "RTCSet";
    case Command::ConfigRead:
        return "ConfigRead";
    case Command::ConfigSet:
        return "ConfigSet";
    case Command::OutputRead:
        return "OutputRead";
    case Command::OutputSet:
        return "OutputSet";

    default:
        return "";
    }
}
