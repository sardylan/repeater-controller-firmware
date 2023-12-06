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

#ifndef STATION_MGMT__NETWORK_COMMAND__H
#define STATION_MGMT__NETWORK_COMMAND__H

#include <stddef.h>
#include <stdint.h>

#include <IPAddress.h>
#include <Printable.h>

#define NETWORK_COMMAND_SIZE 32

enum class Command : uint8_t {
    Null = '\0',
    Ping = 'p',
    Reset = 'X',
    Telemetry = 't',
    RTCRead = 'r',
    RTCSet = 'R',
    ConfigRead = 'c',
    ConfigSet = 'C',
    OutputRead = 'o',
    OutputSet = 'O'
};

class NetworkCommand : public Printable {

public:

    explicit NetworkCommand(Command command, IPAddress ip, uint16_t port);

    NetworkCommand();

    NetworkCommand(const NetworkCommand &other);

    ~NetworkCommand();

    bool isValid() const;

    const IPAddress &getIp() const;

    uint16_t getPort() const;

    Command getCommand() const;

    [[nodiscard]] uint8_t getSingleArg(size_t pos) const;

    void setSingleArg(size_t pos, uint8_t val);

    void getArg(size_t pos, void *val, size_t size) const;

    void setArg(size_t pos, const void *val, size_t size);

    void serialize(uint8_t *buffer) const;

    size_t printTo(Print &p) const override;

    static Command commandFromByte(uint8_t b);

    static uint8_t commandToByte(Command c);

    static const char *commandToString(Command c);

private:

    const bool valid;

    IPAddress ip;
    uint16_t port;

    const Command command;

    uint8_t args[NETWORK_COMMAND_SIZE - 1]{};

};

#endif
