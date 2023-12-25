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

#ifndef STATION_MGMT__NETWORK_PROTOCOL__REQUEST__H
#define STATION_MGMT__NETWORK_PROTOCOL__REQUEST__H

#include <IPAddress.h>
#include <Printable.h>
#include <stddef.h>
#include <stdint.h>

#include "command.hpp"
#include "enums.hpp"

class RequestNetworkCommand final : public NetworkCommand {
    public:

        RequestNetworkCommand();

        explicit RequestNetworkCommand(const IPAddress& ip, const uint16_t &port);

        RequestNetworkCommand(const RequestNetworkCommand& other);

        ~RequestNetworkCommand() override;

        CommandType getCommandType() const;

        void setCommandType(const CommandType& command);

        size_t printTo(Print& p) const override;

    private:

        CommandType commandType {};
};

#endif
