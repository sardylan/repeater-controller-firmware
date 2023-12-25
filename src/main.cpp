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
#include <Wire.h>

#include <ClockUtility.hpp>
#include <EpeverClient.hpp>
#include <NetworkProtocol.hpp>
#include <utilities.hpp>

#include "config.hpp"
#include "const.hpp"
#include "relais.hpp"
#include "utils.hpp"

Config config;

EpeverClient* epeverClient;
NetworkProtocol* networkProtocol;
ClockUtility* clockUtility;
Relais* relais;

declareLastExecution(ReceiveCommand);
declareLastExecution(ReadEpeverData);
declareLastExecution(ReadEpeverStatus);
declareLastExecution(EvaluateGlobalStatus);
declareLastExecution(EvaluateRelais);

bool globalStatus;

int relaisPins[RELAIS_NUMBER] RELAIS_CHANNEL_PINS;

void setup() {
    Serial.begin(115200);

    Serial.println("################################");
    Serial.println(__TIMESTAMP__);
    Serial.println();

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
    networkProtocol = NetworkProtocol::getInstance(mac, NETWORK_IP, NETWORK_DNS, NETWORK_GATEWAY, NETWORK_SUBNET);
    networkProtocol->setUdpPort(NETWORK_UDP_PORT);
    networkProtocol->begin();
    serialDebugln("done");

    serialDebug("Configuring ClockUtility... ");
    clockUtility = ClockUtility::getInstance();
    clockUtility->begin();
    serialDebugln("done");

    serialDebug("Configuring Relais... ");
    relais = Relais::getInstance();
    globalStatus = false;
    serialDebugln("done");
}

void loop() {
    getCurrentMillis();

    executeEvery(ReceiveCommand, 250);
    executeEvery(ReadEpeverData, 1000);
    executeEvery(ReadEpeverStatus, 1000);
    executeEvery(EvaluateGlobalStatus, 1000);
    executeEvery(EvaluateRelais, 1000);
}

void doReceiveCommand() {
    const RequestNetworkCommand request = networkProtocol->receive();
    if (!request.isValid())
        return;

    serialDebugln(request);

    ResponseNetworkCommand response(request.getIp(), request.getPort());
    response.setResponseType(ResponseType::Ack);

    switch (request.getCommandType()) {
        case CommandType::Ping:
            {
                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();

                swapEndian(unixtime);
                response.writeArgs(0, reinterpret_cast<uint8_t*>(&unixtime), sizeof(uint32_t));
            }
            break;

        case CommandType::Reset:
            {
                resetFunc();
            }
            break;

        case CommandType::Telemetry:
            {
                const EpeverData& data = epeverClient->getData();

                float panelVoltage = data.getPanelVoltage();
                float panelCurrent = data.getPanelCurrent();
                float batteryVoltage = data.getBatteryVoltage();
                float batteryChargeCurrent = data.getBatteryChargeCurrent();

                swapEndian(panelVoltage);
                swapEndian(panelCurrent);
                swapEndian(batteryVoltage);
                swapEndian(batteryChargeCurrent);

                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();

                swapEndian(unixtime);
                response.writeArgs(0, reinterpret_cast<uint8_t*>(&unixtime), sizeof(uint32_t));
                response.writeArgs(4, reinterpret_cast<uint8_t*>(&panelVoltage), sizeof(float));
                response.writeArgs(8, reinterpret_cast<uint8_t*>(&panelCurrent), sizeof(float));
                response.writeArgs(12, reinterpret_cast<uint8_t*>(&batteryVoltage), sizeof(float));
                response.writeArgs(16, reinterpret_cast<uint8_t*>(&batteryChargeCurrent), sizeof(float));
            }
            break;

        case CommandType::RTCRead:
            {
                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();

                swapEndian(unixtime);
                response.writeArgs(0, reinterpret_cast<uint8_t*>(&unixtime), sizeof(uint32_t));
            }
            break;

        case CommandType::RTCSet:
            {
                time_t newUnixtime;
                request.readArgs(reinterpret_cast<uint8_t*>(&newUnixtime), 0, sizeof(time_t));
                swapEndian(newUnixtime);
                clockUtility->setEpoch(newUnixtime);

                const CustomDateTime dateTime = ClockUtility::now();
                uint32_t unixtime = dateTime.unixtime();

                swapEndian(unixtime);
                response.writeArgs(0, reinterpret_cast<uint8_t*>(&unixtime), sizeof(uint32_t));
            }
            break;

        case CommandType::ConfigRead:
            {
                const uint8_t b = request.readArg(0);
                response.writeArg(0, b);

                const ConfigParam configParam = configParamParse(b);
                switch (configParam) {
                    case ConfigParam::MainVoltageOff:
                        {
                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            response.writeArgs(1, reinterpret_cast<uint8_t*>(&mainVoltageOff), sizeof(float));
                        }
                        break;

                    case ConfigParam::MainVoltageOn:
                        {
                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            response.writeArgs(1, reinterpret_cast<uint8_t*>(&mainVoltageOn), sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case CommandType::ConfigSet:
            {
                const uint8_t b = request.readArg(0);
                response.writeArg(0, b);

                const ConfigParam configParam = configParamParse(b);
                switch (configParam) {
                    case ConfigParam::MainVoltageOff:
                        {
                            float value;
                            request.readArgs(reinterpret_cast<uint8_t*>(&value), 1, sizeof(float));
                            config.setMainVoltageOff(value);

                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            response.writeArgs(1, reinterpret_cast<uint8_t*>(&mainVoltageOff), sizeof(float));
                        }
                        break;

                    case ConfigParam::MainVoltageOn:
                        {
                            float value;
                            request.readArgs(reinterpret_cast<uint8_t*>(&value), 1, sizeof(float));
                            config.setMainVoltageOn(value);

                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            response.writeArgs(1, reinterpret_cast<uint8_t*>(&mainVoltageOn), sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case CommandType::OutputRead:
            {
                const uint8_t outputNumber = request.readArg(0);
                response.writeArg(0, outputNumber);
                response.writeArg(1, relais->getStatus(outputNumber) ? 1 : 0);
            }
            break;

        case CommandType::OutputSet:
            {
                const uint8_t outputNumber = request.readArg(0);
                const bool newStatus = request.readArg(1) > 0;

                serialDebug("Output: ");
                serialDebug(outputNumber);
                serialDebug(" - New value: ");
                serialDebugln(newStatus ? "TRUE" : "FALSE");

                relais->setStatus(outputNumber, newStatus);

                response.writeArg(0, outputNumber);
                response.writeArg(1, relais->getStatus(outputNumber) ? 0x01 : 0x00);
            }
            break;

        default:
            {
                response.setResponseType(ResponseType::Nack);
            }
    }

    serialDebug("Sending ");
    serialDebugln(response);

    networkProtocol->send(response);
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
