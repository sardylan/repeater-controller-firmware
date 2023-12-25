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

#ifndef STATION_MGMT__NETWORK_PROTOCOL__RESPONSE__H
#define STATION_MGMT__NETWORK_PROTOCOL__RESPONSE__H

#include <IPAddress.h>
#include <Printable.h>
#include <stddef.h>
#include <stdint.h>

#include "command.hpp"
#include "enums.hpp"

class ResponseNetworkCommand final : public NetworkCommand {
    public:

        ResponseNetworkCommand();

        explicit ResponseNetworkCommand(const IPAddress& ip, const uint16_t& port);

        ResponseNetworkCommand(const ResponseNetworkCommand& other);

        ~ResponseNetworkCommand() override;

        ResponseType getResponseType() const;

        void setResponseType(const ResponseType& responseType);

        size_t printTo(Print& p) const override;

    private:

        ResponseType responseType {};
};

#endif