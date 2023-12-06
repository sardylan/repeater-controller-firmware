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

#ifndef STATION_MGMT__EPEVER_CLIENT__H
#define STATION_MGMT__EPEVER_CLIENT__H

#include <ModbusMaster.h>

#include "EpeverData.hpp"
#include "EpeverStatus.hpp"

#define Pin uint8_t

#define MODBUS_BAUDRATE 115200
#define MODBUS_CLIENT_ID 1

class EpeverClient {

public:

    static EpeverClient *getInstance(Pin DI, Pin DE, Pin RE, Pin RO);

    [[nodiscard]] const EpeverData &getData() const;

    [[nodiscard]] const EpeverStatus &getStatus() const;

    void begin();

    void readData();

    void readStatus();

private:

    static EpeverClient *instance;

    const Pin DI;
    const Pin DE;
    const Pin RE;
    const Pin RO;

    ModbusMaster node;

    explicit EpeverClient(Pin DI, Pin DE, Pin RE, Pin RO);

    ~EpeverClient();

    EpeverData data;
    EpeverStatus status;

    void preTransmission() const;

    void postTransmission() const;

    friend void preTransmissionProxy();

    friend void postTransmissionProxy();

};

void preTransmissionProxy();

void postTransmissionProxy();

#endif
