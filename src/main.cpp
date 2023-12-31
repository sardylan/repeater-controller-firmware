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
// #include <DS3231.h>
#include <Ethernet.h>
#include <ModbusMaster.h>
#include <Wire.h>

#include "config.hpp"
#include "const.hpp"
#include "enums.hpp"
#include "protocol.hpp"
#include "relais.hpp"
#include "utils.hpp"
#include "version.hpp"

// DS3231 rtc;

Config config;

ModbusMaster node;
float panelVoltage;
float panelCurrent;
float batteryVoltage;
float batteryChargeCurrent;

bool wrongVoltageIdentification;
Temperature temperature;
Battery battery;
Charging charging;
Arrays arrays;
Load load;

EthernetUDP udp;

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
    pinMode(PIN_EPEVER_RE, OUTPUT);
    pinMode(PIN_EPEVER_DE, OUTPUT);

    modbusPostTransmission();

    Serial2.begin(MODBUS_BAUDRATE);

    node.preTransmission(modbusPreTransmission);
    node.postTransmission(modbusPostTransmission);
    node.begin(MODBUS_CLIENT_ID, Serial2);
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

    // serialDebug("Configuring Clock... ");
    // rtc.setClockMode(false);
    // serialDebugln("done");

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
        resetFunc();
    }
}

void doReceiveCommand() {
    if (udp.parsePacket() == 0)
        return;

    const IPAddress remoteIp = udp.remoteIP();
    const uint16_t remotePort = udp.remotePort();

    char requestPacket[NETWORK_BUFFER_SIZE];
    char responsePacket[NETWORK_BUFFER_SIZE];
    char hexPayload[NETWORK_BUFFER_SIZE * 3];
    memset(requestPacket, '\0', NETWORK_BUFFER_SIZE);
    memset(responsePacket, '\0', NETWORK_BUFFER_SIZE);
    memset(hexPayload, '\0', NETWORK_BUFFER_SIZE);

    const int requestSize = udp.read(requestPacket, NETWORK_BUFFER_SIZE);
    if (requestSize == 0)
        return;

    size_t responseSize = 1;

    printRXDebug(requestPacket, requestSize, remoteIp, remotePort);

    responsePacket[0] = requestPacket[0];

    switch (requestPacket[0]) {
        case PROTOCOL_PING:
            {
                // responseSize += 4;

                // const DateTime dateTime = RTClib::now();
                // uint32_t unixtime = dateTime.unixtime();

                // swapEndian(unixtime);
                // memcpy(responsePacket + 1, &unixtime, sizeof(uint32_t));
            }
            break;

        case PROTOCOL_RESET:
            {
                serialDebugln("Preparing for executing reset");
                executeReset = true;
            }
            break;

        case PROTOCOL_TELEMETRY:
            {
                responseSize += 17;

                // const DateTime dateTime = RTClib::now();
                // uint32_t unixtime = dateTime.unixtime();
                // swapEndian(unixtime);

                float tempPanelVoltage = panelVoltage;
                swapEndian(tempPanelVoltage);

                float tempPanelCurrent = panelCurrent;
                swapEndian(tempPanelCurrent);

                float tempBatteryVoltage = batteryVoltage;
                swapEndian(tempBatteryVoltage);

                float tempBatteryChargeCurrent = batteryChargeCurrent;
                swapEndian(tempBatteryChargeCurrent);

                // memcpy(responsePacket + 1, &unixtime, sizeof(uint32_t));
                memcpy(responsePacket + 0, &tempPanelVoltage, sizeof(float));
                memcpy(responsePacket + 4, &tempPanelCurrent, sizeof(float));
                memcpy(responsePacket + 8, &tempBatteryVoltage, sizeof(float));
                memcpy(responsePacket + 12, &tempBatteryChargeCurrent, sizeof(float));
                responsePacket[16] = globalStatus ? 0x01 : 0x00;
            }
            break;

            // case PROTOCOL_RTC_READ:
            //     {
            //         responseSize += 4;
            //
            //         const DateTime dateTime = RTClib::now();
            //         uint32_t unixtime = dateTime.unixtime();
            //         swapEndian(unixtime);
            //         memcpy(responsePacket + 1, &unixtime, sizeof(uint32_t));
            //     }
            //     break;

            // case PROTOCOL_RTC_SET:
            //     {
            //         responseSize += 4;
            //
            //         time_t newUnixtime;
            //         memcpy(&newUnixtime, requestPacket + 1, sizeof(time_t));
            //         swapEndian(newUnixtime);
            //         rtc.setEpoch(newUnixtime);
            //
            //         const DateTime dateTime = RTClib::now();
            //         uint32_t unixtime = dateTime.unixtime();
            //         swapEndian(unixtime);
            //         memcpy(responsePacket + 1, &unixtime, sizeof(uint32_t));
            //     }
            //     break;

        case PROTOCOL_CONFIG_READ:
            {
                responseSize += 1;

                responsePacket[1] = requestPacket[1];

                switch (requestPacket[1]) {
                    case CONFIG_MAIN_VOLTAGE_OFF_PARAM:
                        {
                            responseSize += sizeof(float);
                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            memcpy(responsePacket + 2, &mainVoltageOff, sizeof(float));
                        }
                        break;

                    case CONFIG_MAIN_VOLTAGE_ON_PARAM:
                        {
                            responseSize += sizeof(float);
                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            memcpy(responsePacket + 2, &mainVoltageOn, sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case PROTOCOL_CONFIG_SET:
            {
                responseSize += 1;

                responsePacket[1] = requestPacket[1];

                switch (requestPacket[1]) {
                    case CONFIG_MAIN_VOLTAGE_OFF_PARAM:
                        {
                            responseSize += sizeof(float);

                            float value;
                            memcpy(&value, requestPacket + 2, sizeof(float));
                            swapEndian(value);
                            serialDebug("new Main Voltage OFF: ");
                            serialDebugln(value);
                            config.setMainVoltageOff(value);

                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            memcpy(responsePacket + 2, &mainVoltageOff, sizeof(float));
                        }
                        break;

                    case CONFIG_MAIN_VOLTAGE_ON_PARAM:
                        {
                            responseSize += sizeof(float);

                            float value;
                            memcpy(&value, requestPacket + 2, sizeof(float));
                            swapEndian(value);
                            serialDebug("new Main Voltage ON: ");
                            serialDebugln(value);
                            config.setMainVoltageOn(value);

                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            memcpy(responsePacket + 2, &mainVoltageOn, sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case PROTOCOL_OUTPUT_READ:
            {
                responseSize += 2;

                const uint8_t outputNumber = requestPacket[1];
                responsePacket[1] = outputNumber;
                responsePacket[2] = relais->getStatus(outputNumber) ? 0x01 : 0x00;
            }
            break;

        case PROTOCOL_OUTPUT_SET:
            {
                responseSize += 2;

                const uint8_t outputNumber = requestPacket[1];
                const bool newStatus = requestPacket[2] > 0;

                relais->setStatus(outputNumber, newStatus);

                responsePacket[1] = outputNumber;
                responsePacket[2] = relais->getStatus(outputNumber) ? 0x01 : 0x00;
            }
            break;

        default:
            {
                responseSize += 1;

                responsePacket[0] = PROTOCOL_NACK;
                responsePacket[1] = requestPacket[0];
            }
    }

    printTXDebug(responsePacket, responseSize, remoteIp, remotePort);

    udp.beginPacket(remoteIp, remotePort);
    udp.write(responsePacket, responseSize);
    udp.endPacket();
}

void doReadEpeverData() {
    const uint8_t result = node.readInputRegisters(0x3100, 6);
    if (result != ModbusMaster::ku8MBSuccess) {
        panelVoltage = 0;
        panelCurrent = 0;
        batteryVoltage = 0;
        batteryChargeCurrent = 0;
        return;
    }

    panelVoltage = node.getResponseBuffer(0x00) / 100.0f;
    panelCurrent = node.getResponseBuffer(0x01) / 100.0f;
    batteryVoltage = node.getResponseBuffer(0x04) / 100.0f;
    batteryChargeCurrent = node.getResponseBuffer(0x05) / 100.0f;
}

void doReadEpeverStatus() {
    const uint8_t result = node.readInputRegisters(0x3200, 3);
    if (result != ModbusMaster::ku8MBSuccess) {
        return;
    }

    uint16_t tempBuffer = node.getResponseBuffer(0x00);
    wrongVoltageIdentification = tempBuffer & 0x8000;

    uint8_t tempData = (tempBuffer & 0x00F0) >> 4;    // D7-D4 shifted down
    temperature = static_cast<Temperature>(tempData);

    tempData = (tempBuffer & 0x000F);    // D3-D0
    battery = static_cast<Battery>(tempData);

    tempBuffer = node.getResponseBuffer(0x01);

    tempData = (tempBuffer & 0x000C) >> 2;    // D3-D2 shifted down
    charging = static_cast<Charging>(tempData);

    tempData = (tempBuffer & 0xC000) >> 14;    // D15-D14 shifted down
    arrays = static_cast<Arrays>(tempData);

    tempBuffer = node.getResponseBuffer(0x02);
    tempData = (tempBuffer & 0x3000) >> 12;    // D3-12 shifted down
    load = static_cast<Load>(tempData);
}

void doEvaluateGlobalStatus() {
    const float& onVoltage = config.getMainVoltageOn();
    const float& offVoltage = config.getMainVoltageOff();

    globalStatus = batteryVoltage >= onVoltage || (globalStatus && !(batteryVoltage <= offVoltage));

    serialDebugHeader("STATUS");
    serialDebug("BV: ");
    serialDebug(batteryVoltage);
    serialDebug(" - on: ");
    serialDebug(onVoltage);
    serialDebug(" - off: ");
    serialDebug(offVoltage);
    serialDebug(" - GS: ");
    serialDebugln(globalStatus);
}

void doEvaluateRelais() {
    serialDebugHeader("RELAIS");
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

void modbusPreTransmission() {
    digitalWrite(PIN_EPEVER_RE, HIGH);
    digitalWrite(PIN_EPEVER_DE, HIGH);
}

void modbusPostTransmission() {
    digitalWrite(PIN_EPEVER_RE, LOW);
    digitalWrite(PIN_EPEVER_DE, LOW);
}

#ifdef DEBUG
void printNetworkDebug(
    const bool isTx,
    const char* payload,
    const size_t payloadSize,
    const IPAddress& remoteIp,
    const uint16_t remotePort) {
    char hexPayload[NETWORK_BUFFER_SIZE * 3];
    memset(hexPayload, '\0', NETWORK_BUFFER_SIZE);
    payloadToHex(hexPayload, payload, payloadSize);

    serialDebugHeader("NET");

    if (isTx)
        serialDebug(" TX \"");
    else
        serialDebug(" RX \"");

    serialDebug(payload[0]);

    if (isTx)
        serialDebug("\" response of ");
    else
        serialDebug("\" request of ");

    serialDebug(payloadSize);
    serialDebug(" bytes [");
    serialDebug(hexPayload);

    if (isTx)
        serialDebug("] to ");
    else
        serialDebug("] from ");

    serialDebug(remoteIp);
    serialDebug(":");
    serialDebugln(remotePort);
}
#endif