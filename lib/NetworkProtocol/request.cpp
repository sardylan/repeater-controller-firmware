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

#include "request.hpp"
#include "utilities.hpp"

RequestNetworkCommand::RequestNetworkCommand() = default;

RequestNetworkCommand::RequestNetworkCommand(const IPAddress& ip, const uint16_t& port) : NetworkCommand(ip, port) {
}

RequestNetworkCommand::RequestNetworkCommand(const RequestNetworkCommand& other) = default;

RequestNetworkCommand::~RequestNetworkCommand() = default;

CommandType RequestNetworkCommand::getCommandType() const {
    return commandType;
}

void RequestNetworkCommand::setCommandType(const CommandType& commandType) {
    this->commandType = commandType;
}

size_t RequestNetworkCommand::printTo(Print& p) const {
    size_t ln = p.print(commandTypeToString(commandType));
    ln += p.print(" command of ");
    ln += p.print(getArgsSize());
    ln += p.print(" bytes from ");
    ln += p.print(getIp());
    ln += p.print(":");
    ln += p.print(getPort());
    return ln;
}
