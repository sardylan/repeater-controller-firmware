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

#ifndef STATION_MGMT__NETWORK_PROTOCOL__PROTOCOL__H
#define STATION_MGMT__NETWORK_PROTOCOL__PROTOCOL__H

#include <Ethernet.h>
#include <WString.h>

#include "request.hpp"
#include "response.hpp"

#define DEFAULT_UDP_PORT 8888

class NetworkProtocol {
    public:

        static NetworkProtocol* getInstance(
            const uint8_t* mac, const String& ip, const String& dns, const String& gateway, const String& subnet);

        [[nodiscard]]
        uint16_t getUdpPort() const;

        void setUdpPort(uint16_t newValue);

        void begin();

        RequestNetworkCommand receive();

        void send(const ResponseNetworkCommand& command);

    private:

        NetworkProtocol(
            const uint8_t* mac, const String& ip, const String& dns, const String& gateway, const String& subnet);

        ~NetworkProtocol();

        static NetworkProtocol* instance;

        uint8_t mac[6] {};
        IPAddress ip;
        IPAddress dns;
        IPAddress gateway;
        IPAddress subnet;

        uint16_t udpPort;
        EthernetUDP udp;
};

#endif
