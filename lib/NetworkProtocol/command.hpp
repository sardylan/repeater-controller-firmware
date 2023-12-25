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

#ifndef STATION_MGMT__NETWORK_PROTOCOL__COMMAND__H
#define STATION_MGMT__NETWORK_PROTOCOL__COMMAND__H

#include <IPAddress.h>
#include <Print.h>
#include <Printable.h>
#include <stdint.h>

#include "enums.hpp"

class NetworkCommand : public Printable {
    public:

        NetworkCommand();

        explicit NetworkCommand(const IPAddress& ip, uint16_t port);

        NetworkCommand(const NetworkCommand& other);

        virtual ~NetworkCommand();

        [[nodiscard]]
        bool isValid() const;

        [[nodiscard]]
        const IPAddress& getIp() const;

        [[nodiscard]]
        uint16_t getPort() const;

        [[nodiscard]]
        size_t getArgsSize() const;

        void readArgs(uint8_t* dest, const size_t& pos, const size_t& size) const;

        void writeArgs(const size_t& pos, const uint8_t* src, const size_t& size);

        const uint8_t & readArg(const size_t& pos) const;

        void writeArg(const size_t& pos, const uint8_t &value);

        size_t printTo(Print& p) const override = 0;

    protected:

        uint8_t args[32] {};
        size_t argsSize {};

    private:

        const bool valid;

        const IPAddress ip;
        const uint16_t port;
};

#endif
