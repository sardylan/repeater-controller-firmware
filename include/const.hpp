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

#ifndef STATION_MGMT__CONST__H
#define STATION_MGMT__CONST__H

#define PIN_EPEVER_RO 0    // RX
#define PIN_EPEVER_RE 3
#define PIN_EPEVER_DE 2
#define PIN_EPEVER_DI 0    // TX

#define MODBUS_BAUDRATE 115200
#define MODBUS_CLIENT_ID 1

#define PIN_ETHERNET_SD_ENABLE 4
#define PIN_ETHERNET_NET_ENABLE 10

#define NETWORK_MAC_ADDRESS \
    { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed }

#define NETWORK_IP "172.29.10.66"
#define NETWORK_DNS "172.29.10.254"
#define NETWORK_GATEWAY "172.29.10.254"
#define NETWORK_SUBNET "172.29.10.0"
#define NETWORK_UDP_PORT 8888
#define NETWORK_BUFFER_SIZE 64

#define RELAIS_CHANNEL_PINS \
    { 23, 25, 27, 29, 31, 33, 35, 37 }

#define RAINBOW_DELAY 75

#endif
