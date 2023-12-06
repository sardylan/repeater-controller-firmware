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

#include "NetworkProtocol.hpp"

#include <Ethernet.h>

NetworkProtocol* NetworkProtocol::instance = nullptr;

NetworkProtocol* NetworkProtocol::getInstance(const uint8_t* mac,
                                              const String& ip,
                                              const String& dns,
                                              const String& gateway,
                                              const String& subnet)
{
    if (NetworkProtocol::instance == nullptr)
        NetworkProtocol::instance = new NetworkProtocol(mac, ip, dns, gateway, subnet);
    return NetworkProtocol::instance;
}

NetworkProtocol::NetworkProtocol(const uint8_t* mac,
                                 const String& ip,
                                 const String& dns,
                                 const String& gateway,
                                 const String& subnet)
{
    memcpy(NetworkProtocol::mac, mac, 6);
    NetworkProtocol::ip.fromString(ip);
    NetworkProtocol::dns.fromString(dns);
    NetworkProtocol::gateway.fromString(gateway);
    NetworkProtocol::subnet.fromString(subnet);

    NetworkProtocol::udpPort = DEFAULT_UDP_PORT;
}

NetworkProtocol::~NetworkProtocol() = default;

uint16_t NetworkProtocol::getUdpPort() const
{
    return udpPort;
}

void NetworkProtocol::setUdpPort(uint16_t newValue)
{
    NetworkProtocol::udpPort = newValue;
}

void NetworkProtocol::begin()
{
    EthernetClass::begin(const_cast<uint8_t*>(mac), ip, dns, gateway, subnet);

    udp.begin(udpPort);
}

NetworkCommand NetworkProtocol::receiveCommand()
{
    const int packetSize = udp.parsePacket();

    if (packetSize == 0 || packetSize > NETWORK_COMMAND_SIZE)
        return {};

    const IPAddress remoteIp = udp.remoteIP();
    const uint16_t remotePort = udp.remotePort();

    uint8_t packetBuffer[NETWORK_COMMAND_SIZE];
    memset(packetBuffer, '\x00', NETWORK_COMMAND_SIZE);

    const int len = udp.read(reinterpret_cast<unsigned char*>(packetBuffer), NETWORK_COMMAND_SIZE);
    if (len == 0)
        return {};

    const Command command = NetworkCommand::commandFromByte(packetBuffer[0]);
    NetworkCommand networkCommand(command, remoteIp, remotePort);
    for (int i = 1; i < len; i++)
        networkCommand.setSingleArg(i - 1, packetBuffer[i]);

    return networkCommand;
}

void NetworkProtocol::send(const NetworkCommand& networkCommand)
{
    if (!networkCommand.isValid())
        return;

    uint8_t buffer[NETWORK_COMMAND_SIZE];
    networkCommand.serialize(buffer);

    udp.beginPacket(networkCommand.getIp(), networkCommand.getPort());
    udp.write(buffer, NETWORK_COMMAND_SIZE);
    udp.endPacket();
}
