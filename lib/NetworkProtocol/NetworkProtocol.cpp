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

#include "request.hpp"
#include "response.hpp"
#include "utilities.hpp"

NetworkProtocol* NetworkProtocol::instance = nullptr;

NetworkProtocol* NetworkProtocol::getInstance(
    const uint8_t* mac, const String& ip, const String& dns, const String& gateway, const String& subnet) {
    if (instance == nullptr)
        instance = new NetworkProtocol(mac, ip, dns, gateway, subnet);
    return instance;
}

NetworkProtocol::NetworkProtocol(
    const uint8_t* mac, const String& ip, const String& dns, const String& gateway, const String& subnet) {
    memcpy(NetworkProtocol::mac, mac, 6);
    NetworkProtocol::ip.fromString(ip);
    NetworkProtocol::dns.fromString(dns);
    NetworkProtocol::gateway.fromString(gateway);
    NetworkProtocol::subnet.fromString(subnet);

    udpPort = DEFAULT_UDP_PORT;
}

NetworkProtocol::~NetworkProtocol() = default;

uint16_t NetworkProtocol::getUdpPort() const {
    return udpPort;
}

void NetworkProtocol::setUdpPort(uint16_t newValue) {
    NetworkProtocol::udpPort = newValue;
}

void NetworkProtocol::begin() {
    EthernetClass::begin(const_cast<uint8_t*>(mac), ip, dns, gateway, subnet);

    udp.begin(udpPort);
}

RequestNetworkCommand NetworkProtocol::receive() {
    const int packetSize = udp.parsePacket();

    if (packetSize == 0)
        return {};

    const IPAddress remoteIp = udp.remoteIP();
    const uint16_t remotePort = udp.remotePort();

    uint8_t packetBuffer[64];
    memset(packetBuffer, '\x00', 64);

    const int len = udp.read(reinterpret_cast<unsigned char*>(packetBuffer), 64);
    if (len == 0)
        return {};

    RequestNetworkCommand command(remoteIp, remotePort);

    const CommandType& commandType = commandTypeParse(packetBuffer[0]);
    command.setCommandType(commandType);
    command.writeArgs(0, packetBuffer + 1, len - 1);
    return command;
}

void NetworkProtocol::send(const ResponseNetworkCommand& command) {
    if (!command.isValid())
        return;

    const size_t argsSize = command.getArgsSize();
    const size_t payloadSize = argsSize + 1;

    uint8_t buffer[payloadSize];
    buffer[0] = responseTypeSerialize(command.getResponseType());
    command.readArgs(buffer + 1, 0, argsSize);

    udp.beginPacket(command.getIp(), command.getPort());
    udp.write(buffer, payloadSize);
    udp.endPacket();
}
