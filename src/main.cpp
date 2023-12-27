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

#include "main.hpp"

#include <Arduino.h>
#include <Ethernet.h>
#include <Wire.h>

#include <ClockUtility.hpp>
#include <EpeverClient.hpp>
#include <request.hpp>
#include <response.hpp>
#include <utilities.hpp>

#include "config.hpp"
#include "const.hpp"
#include "relais.hpp"
#include "utils.hpp"
#include "version.hpp"

Config config;

EpeverClient* epeverClient;
EthernetUDP udp;
ClockUtility* clockUtility;
Relais* relais;

declareLastExecution(ReceiveCommand);
declareLastExecution(ReadEpeverData);
declareLastExecution(ReadEpeverStatus);
declareLastExecution(EvaluateGlobalStatus);
declareLastExecution(EvaluateRelais);

bool globalStatus;

int relaisPins[RELAIS_NUMBER] RELAIS_CHANNEL_PINS;

bool executeReset;

void setup() {
    Serial.begin(115200);

    Serial.println("################################");
    Serial.println(__TIMESTAMP__);
    Serial.print("Firmware version: ");
    Serial.println(FIRMWARE_VERSION);
    Serial.println();
    Serial.flush();

    serialDebug("Configuring Relais pins... ");
    for (const int pin : relaisPins) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
    rainbow();
    serialDebugln("done");

    serialDebug("Configuring Ethernet shield pins... ");
    pinMode(PIN_ETHERNET_SD_ENABLE, OUTPUT);
    pinMode(PIN_ETHERNET_NET_ENABLE, OUTPUT);

    digitalWrite(PIN_ETHERNET_SD_ENABLE, LOW);
    digitalWrite(PIN_ETHERNET_NET_ENABLE, HIGH);
    serialDebugln("done");

    serialDebug("Configuring Wire... ");
    Wire.begin();
    serialDebugln("done");

    serialDebug("Configuring EpeverClient... ");
    epeverClient = EpeverClient::getInstance(PIN_EPEVER_DI, PIN_EPEVER_DE, PIN_EPEVER_RE, PIN_EPEVER_RO);
    epeverClient->begin();
    serialDebugln("done");

    serialDebug("Configuring NetworkProtocol... ");
    constexpr uint8_t mac[] NETWORK_MAC_ADDRESS;
    IPAddress networkIp;
    networkIp.fromString(NETWORK_IP);
    IPAddress networkDns;
    networkDns.fromString(NETWORK_DNS);
    IPAddress networkGateway;
    networkGateway.fromString(NETWORK_GATEWAY);
    IPAddress networkSubnet;
    networkSubnet.fromString(NETWORK_SUBNET);
    EthernetClass::begin(const_cast<uint8_t*>(mac), networkIp, networkDns, networkGateway, networkSubnet);
    udp.begin(NETWORK_UDP_PORT);
    serialDebugln("done");

    serialDebug("Configuring ClockUtility... ");
    clockUtility = ClockUtility::getInstance();
    clockUtility->begin();
    serialDebugln("done");

    serialDebug("Configuring Relais... ");
    relais = Relais::getInstance();
    globalStatus = false;
    serialDebugln("done");

    executeReset = false;
}

void loop() {
    getCurrentMillis();

    executeEvery(ReceiveCommand, 250);
    executeEvery(ReadEpeverData, 1000);
    executeEvery(ReadEpeverStatus, 1000);
    executeEvery(EvaluateGlobalStatus, 1000);
    executeEvery(EvaluateRelais, 1000);

    if (executeReset) {
        executeReset = false;
        // resetFunc();
    }
}

void doReceiveCommand() {
    if (udp.parsePacket() == 0)
        return;

    const IPAddress remoteIp = udp.remoteIP();
    const uint16_t remotePort = udp.remotePort();

    uint8_t packetBuffer[64];
    memset(packetBuffer, '\0', 64);

    const int len = udp.read(reinterpret_cast<unsigned char*>(packetBuffer), 64);
    if (len == 0)
        return;

    const int requestArgsSize = len - 1;
    int responseArgsSize = 0;

    // RequestNetworkCommand request(remoteIp, remotePort);

    const CommandType commandType = commandTypeParse(packetBuffer[0]);
    // request.setCommandType(commandType);
    // request.writeArgs(0, packetBuffer + 1, len - 1);
    if (commandType == CommandType::Null)
        return;

    serialDebug(commandTypeToString(commandType));
    serialDebug(" command of ");
    serialDebug(requestArgsSize);
    serialDebug(" bytes from ");
    serialDebug(remoteIp);
    serialDebug(":");
    serialDebugln(remotePort);

    auto responseType = ResponseType::Ack;

    uint8_t args[32];
    memset(args, '\0', 32);

    switch (commandType) {
        case CommandType::Ping:
            {
                responseArgsSize = 4;

                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();

                swapEndian(unixtime);
                memcpy(reinterpret_cast<void*>(args[0]), reinterpret_cast<uint8_t*>(&unixtime), sizeof(uint32_t));
            }
            break;

        case CommandType::Reset:
            {
                serialDebugln("Preparing for executing reset");
                executeReset = true;
            }
            break;

        case CommandType::Telemetry:
            {
                responseArgsSize = 21;

                const EpeverData& data = epeverClient->getData();

                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();
                swapEndian(unixtime);

                float panelVoltage = data.getPanelVoltage();
                swapEndian(panelVoltage);

                float panelCurrent = data.getPanelCurrent();
                swapEndian(panelCurrent);

                float batteryVoltage = data.getBatteryVoltage();
                swapEndian(batteryVoltage);

                float batteryChargeCurrent = data.getBatteryChargeCurrent();
                swapEndian(batteryChargeCurrent);

                memcpy(reinterpret_cast<void*>(args[0]), &unixtime, sizeof(uint32_t));
                memcpy(reinterpret_cast<void*>(args[4]), &panelVoltage, sizeof(float));
                memcpy(reinterpret_cast<void*>(args[8]), &panelCurrent, sizeof(float));
                memcpy(reinterpret_cast<void*>(args[12]), &batteryVoltage, sizeof(float));
                memcpy(reinterpret_cast<void*>(args[16]), &batteryChargeCurrent, sizeof(float));
                args[20] = globalStatus ? 0x01 : 0x00;
            }
            break;

        case CommandType::RTCRead:
            {
                responseArgsSize = 4;

                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();

                swapEndian(unixtime);
                memcpy(reinterpret_cast<void*>(args[0]), &unixtime, sizeof(uint32_t));
            }
            break;

        case CommandType::RTCSet:
            {
                responseArgsSize = 4;

                time_t newUnixtime;
                memcpy(&newUnixtime, packetBuffer + 1, sizeof(time_t));
                swapEndian(newUnixtime);
                clockUtility->setEpoch(newUnixtime);

                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();

                swapEndian(unixtime);
                memcpy(reinterpret_cast<void*>(args[0]), &unixtime, sizeof(uint32_t));
            }
            break;

        case CommandType::ConfigRead:
            {
                responseArgsSize = 1;

                const uint8_t b = packetBuffer[1];
                args[0] = b;

                const ConfigParam configParam = configParamParse(b);
                switch (configParam) {
                    case ConfigParam::MainVoltageOff:
                        {
                            responseArgsSize += 4;
                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            memcpy(
                                reinterpret_cast<void*>(args[1]),
                                reinterpret_cast<uint8_t*>(&mainVoltageOff),
                                sizeof(float));
                        }
                        break;

                    case ConfigParam::MainVoltageOn:
                        {
                            responseArgsSize += 4;
                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            memcpy(
                                reinterpret_cast<void*>(args[1]),
                                reinterpret_cast<uint8_t*>(&mainVoltageOn),
                                sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case CommandType::ConfigSet:
            {
                responseArgsSize = 1;

                const uint8_t b = packetBuffer[1];
                args[0] = b;

                const ConfigParam configParam = configParamParse(b);
                switch (configParam) {
                    case ConfigParam::MainVoltageOff:
                        {
                            responseArgsSize += 4;

                            float value;
                            memcpy(reinterpret_cast<uint8_t*>(&value), packetBuffer + 2, sizeof(float));
                            config.setMainVoltageOff(value);

                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            memcpy(
                                reinterpret_cast<void*>(args[1]),
                                reinterpret_cast<uint8_t*>(&mainVoltageOff),
                                sizeof(float));
                        }
                        break;

                    case ConfigParam::MainVoltageOn:
                        {
                            responseArgsSize += 4;

                            float value;
                            memcpy(reinterpret_cast<uint8_t*>(&value), packetBuffer + 2, sizeof(float));
                            config.setMainVoltageOn(value);

                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            memcpy(
                                reinterpret_cast<void*>(args[1]),
                                reinterpret_cast<uint8_t*>(&mainVoltageOn),
                                sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case CommandType::OutputRead:
            {
                responseArgsSize = 2;

                const uint8_t outputNumber = packetBuffer[1];
                args[0] = outputNumber;
                args[1] = relais->getStatus(outputNumber) ? 0x01 : 0x00;
            }
            break;

        case CommandType::OutputSet:
            {
                responseArgsSize = 2;

                const uint8_t outputNumber = packetBuffer[1];
                const bool newStatus = packetBuffer[2] > 0;

                serialDebug("Output: ");
                serialDebug(outputNumber);
                serialDebug(" - New value: ");
                serialDebugln(newStatus ? "TRUE" : "FALSE");

                relais->setStatus(outputNumber, newStatus);

                args[0] = outputNumber;
                args[1] = relais->getStatus(outputNumber) ? 0x01 : 0x00;
            }
            break;

        default:
            {
                responseType = ResponseType::Nack;
            }
    }

    serialDebug("Sending ");
    serialDebug(responseTypeToString(responseType));
    serialDebug(" response of ");
    serialDebug(responseArgsSize);
    serialDebug(" bytes to ");
    serialDebug(remoteIp);
    serialDebug(":");
    serialDebugln(remotePort);

    const size_t payloadSize = responseArgsSize + 1;

    uint8_t payload[payloadSize];
    payload[0] = responseTypeSerialize(responseType);
    memcpy(payload + 1, args, responseArgsSize);

    char hexPayload[payloadSize * 3];
    payloadToHex(hexPayload, reinterpret_cast<const char*>(payload), payloadSize);

    serialDebug("UDP Sending: ");
    serialDebugln(hexPayload);

    udp.beginPacket(remoteIp, remotePort);
    udp.write(payload, payloadSize);
    udp.endPacket();
}

void doReadEpeverData() {
    epeverClient->readData();
}

void doReadEpeverStatus() {
    epeverClient->readStatus();
}

void doEvaluateGlobalStatus() {
    const EpeverData& data = epeverClient->getData();
    if (!data.isValid())
        return;

    const float& currentVoltage = data.getBatteryVoltage();
    const float& onVoltage = config.getMainVoltageOn();
    const float& offVoltage = config.getMainVoltageOff();

    globalStatus = currentVoltage >= onVoltage || (globalStatus && !(currentVoltage <= offVoltage));

    serialDebug("currentVoltage: ");
    serialDebugln(currentVoltage);
    serialDebug("onVoltage: ");
    serialDebugln(onVoltage);
    serialDebug("offVoltage: ");
    serialDebugln(offVoltage);
    serialDebug("globalStatus: ");
    serialDebugln(globalStatus);
}

void doEvaluateRelais() {
    serialDebug("Relais: ");
    for (int i = 0; i < RELAIS_NUMBER; i++) {
        serialDebug(relais->getStatus(i) ? "1" : "0");
        digitalWrite(relaisPins[i], globalStatus && relais->getStatus(i) ? LOW : HIGH);
    }
    serialDebugln();
}

void rainbow() {
    for (const int pin : relaisPins) {
        digitalWrite(pin, LOW);
        delayRainbow();
    }

    for (const int pin : relaisPins) {
        digitalWrite(pin, HIGH);
        delayRainbow();
    }
}
