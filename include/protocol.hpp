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

#ifndef STATION_MGMT__PROTOCOL__H
#define STATION_MGMT__PROTOCOL__H

#define PROTOCOL_PING 'p'
#define PROTOCOL_RESET 'X'
#define PROTOCOL_TELEMETRY 't'
#define PROTOCOL_RTC_READ 'r'
#define PROTOCOL_RTC_SET 'R'
#define PROTOCOL_CONFIG_READ 'c'
#define PROTOCOL_CONFIG_SET 'C'
#define PROTOCOL_OUTPUT_READ 'o'
#define PROTOCOL_OUTPUT_SET 'O'

#define PROTOCOL_NACK 'N'

#endif
