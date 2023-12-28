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

#ifndef STATION_MGMT__MAIN__H
#define STATION_MGMT__MAIN__H

#include <IPAddress.h>
#include <stddef.h>

#include "const.hpp"

#define declareLastExecution(x) unsigned long lastExecution##x = 0

#define getCurrentMillis() unsigned long clockNow = millis()

#define executeEvery(jobName, timeSpan) \
    if (clockNow < lastExecution##jobName) \
        lastExecution##jobName = 0; \
    if (clockNow - lastExecution##jobName > timeSpan) { \
        lastExecution##jobName = clockNow; \
        do \
            ##jobName(); \
    };

#ifdef DEBUG
    #define serialDebug(x) Serial.print(x)
    #define serialDebugln(x) \
        Serial.println(x); \
        Serial.flush()
#else
    #define serialDebug(x)
    #define serialDebugln(x)
#endif

#define swapEndian(x) swapEndianness(&x, sizeof(x));

#define delayRainbow() delay(RAINBOW_DELAY)

#define printTXDebug(payload, payloadSize, remoteIp, remotePort) \
    printNetworkDebug(true, payload, payloadSize, remoteIp, remotePort)
#define printRXDebug(payload, payloadSize, remoteIp, remotePort) \
    printNetworkDebug(false, payload, payloadSize, remoteIp, remotePort)

#define serialDebugHeader(x) \
    serialDebug(RTClib::now().unixtime()); \
    serialDebug(" ["); \
    serialDebug(x); \
    serialDebug("] ");

void (*resetFunc)() = nullptr;

void doReceiveCommand();

void doReadEpeverData();

void doReadEpeverStatus();

void doEvaluateGlobalStatus();

void doEvaluateRelais();

void rainbow();

void modbusPreTransmission();

void modbusPostTransmission();

#ifdef DEBUG
void printNetworkDebug(
    bool isTx, const char* payload, size_t payloadSize, const IPAddress& remoteIp, uint16_t remotePort);
#else
#define printNetworkDebug(isTx, payload, payloadSize, remoteIp, remotePort)
#endif

#endif
