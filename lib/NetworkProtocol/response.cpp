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

#include "response.hpp"

#include "utilities.hpp"

ResponseNetworkCommand::ResponseNetworkCommand() = default;

ResponseNetworkCommand::ResponseNetworkCommand(const IPAddress& ip, const uint16_t& port) : NetworkCommand(ip, port) {
}

ResponseNetworkCommand::ResponseNetworkCommand(const ResponseNetworkCommand& other) = default;

ResponseNetworkCommand::~ResponseNetworkCommand() = default;

ResponseType ResponseNetworkCommand::getResponseType() const {
    return responseType;
}

void ResponseNetworkCommand::setResponseType(const ResponseType& responseType) {
    this->responseType = responseType;
}

size_t ResponseNetworkCommand::printTo(Print& p) const {
    size_t ln = p.print(responseTypeToString(responseType));
    ln += p.print(" response of ");
    ln += p.print(getArgsSize());
    ln += p.print(" bytes to ");
    ln += p.print(getIp());
    ln += p.print(":");
    ln += p.print(getPort());
    return ln;
}
