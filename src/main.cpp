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

#include <EpeverClient.hpp>
#include <NetworkProtocol.hpp>
#include <ClockUtility.hpp>

#include "const.hpp"
#include "config.hpp"

Config config;

EpeverClient *epeverClient;
NetworkProtocol *networkProtocol;
ClockUtility *clockUtility;

declareLastExecution(ReceiveCommand);
declareLastExecution(ReadEpeverData);
declareLastExecution(ReadEpeverStatus);
declareLastExecution(Evaluate);

void setup() {
    Serial.begin(9600);

    serialDebugln("Build 4");

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
    constexpr uint8_t mac[]NETWORK_MAC_ADDRESS;
    networkProtocol = NetworkProtocol::getInstance(mac, NETWORK_IP, NETWORK_DNS, NETWORK_GATEWAY, NETWORK_SUBNET);
    networkProtocol->setUdpPort(NETWORK_UDP_PORT);
    networkProtocol->begin();
    serialDebugln("done");

    serialDebug("Configuring ClockUtility... ");
    clockUtility = ClockUtility::getInstance();
    clockUtility->begin();
    serialDebugln("done");
}

void loop() {
    getCurrentMillis();

    executeEvery(ReceiveCommand, 1000)
    executeEvery(ReadEpeverData, 1000)
    executeEvery(ReadEpeverStatus, 1000)
    executeEvery(Evaluate, 5000)
}

void doReceiveCommand() {
    const NetworkCommand request = networkProtocol->receiveCommand();
    if (!request.isValid())
        return;

    serialDebugln(request);

    NetworkCommand response(request.getCommand(), request.getIp(), request.getPort());

    switch (request.getCommand()) {

        case Command::Ping: {
            const CustomDateTime dateTime = clockUtility->now();
            const uint32_t unixtime = dateTime.unixtime();
            response.setSingleArg(0, static_cast<uint8_t>(unixtime >> 0));
            response.setSingleArg(1, static_cast<uint8_t>(unixtime >> 8));
            response.setSingleArg(2, static_cast<uint8_t>(unixtime >> 16));
            response.setSingleArg(3, static_cast<uint8_t>(unixtime >> 24));
        }
            break;

        case Command::Reset: {
            resetFunc();
        }
            break;

        case Command::Telemetry: {
            const EpeverData& data = epeverClient->getData();

            float panelVoltage = data.getPanelVoltage();
            float panelCurrent = data.getPanelCurrent();
            float batteryVoltage = data.getBatteryVoltage();
            float batteryChargeCurrent = data.getBatteryChargeCurrent();

            response.setArg(0, reinterpret_cast<uint8_t *>(&panelVoltage), sizeof(panelVoltage));
            response.setArg(3, reinterpret_cast<uint8_t *>(&panelCurrent), sizeof(panelCurrent));
            response.setArg(7, reinterpret_cast<uint8_t *>(&batteryVoltage), sizeof(batteryVoltage));
            response.setArg(11, reinterpret_cast<uint8_t *>(&batteryChargeCurrent), sizeof(batteryChargeCurrent));
        }
            break;

        case Command::RTCRead: {
            const CustomDateTime dateTime = clockUtility->now();
            const uint32_t unixtime = dateTime.unixtime();
            response.setSingleArg(0, static_cast<uint8_t>(unixtime >> 0));
            response.setSingleArg(1, static_cast<uint8_t>(unixtime >> 8));
            response.setSingleArg(2, static_cast<uint8_t>(unixtime >> 16));
            response.setSingleArg(3, static_cast<uint8_t>(unixtime >> 24));
        }
            break;

        case Command::RTCSet: {
            time_t unixtime = (static_cast<time_t>(request.getSingleArg(0)))
                              + (static_cast<time_t>(request.getSingleArg(1)) << 8)
                              + (static_cast<time_t>(request.getSingleArg(2)) << 16)
                              + (static_cast<time_t>(request.getSingleArg(3)) << 24);
            clockUtility->setEpoch(unixtime);

            const CustomDateTime dateTime = clockUtility->now();
            unixtime = dateTime.unixtime();
            response.setSingleArg(0, static_cast<uint8_t>(unixtime >> 0));
            response.setSingleArg(1, static_cast<uint8_t>(unixtime >> 8));
            response.setSingleArg(2, static_cast<uint8_t>(unixtime >> 16));
            response.setSingleArg(3, static_cast<uint8_t>(unixtime >> 24));
        }
            break;

        case Command::ConfigRead: {
            const uint8_t configParam = request.getSingleArg(0);
            response.setSingleArg(0, configParam);

            switch (configParam) {

                case CONFIG_MAIN_VOLTAGE_OFF_PARAM:
                    response.setArg(1, reinterpret_cast<uint8_t *>(config.getMainVoltageOff()), sizeof(uint16_t));
                    break;

                case CONFIG_MAIN_VOLTAGE_ON_PARAM:
                    response.setArg(1, reinterpret_cast<uint8_t *>(config.getMainVoltageOn()), sizeof(uint16_t));
                    break;

                default:
                    break;
            }
        }
            break;

        case Command::ConfigSet: {
            const uint8_t configParam = request.getSingleArg(0);
            response.setSingleArg(0, configParam);

            switch (configParam) {

                case CONFIG_MAIN_VOLTAGE_OFF_PARAM: {
                    uint16_t value;
                    request.getArg(0, &value, sizeof(uint16_t));
                    config.setMainVoltageOff(value);
                    response.setArg(1, reinterpret_cast<uint8_t *>(config.getMainVoltageOff()), sizeof(uint16_t));
                }
                    break;

                case CONFIG_MAIN_VOLTAGE_ON_PARAM: {
                    uint16_t value;
                    request.getArg(0, &value, sizeof(uint16_t));
                    config.setMainVoltageOn(value);
                    response.setArg(1, reinterpret_cast<uint8_t *>(config.getMainVoltageOn()), sizeof(uint16_t));
                }
                    break;

                default:
                    break;
            }

        }
            break;

        case Command::OutputRead: {
            const uint8_t outputNumber = request.getSingleArg(0);
            response.setSingleArg(0, outputNumber);

        }
            break;

        case Command::OutputSet: {
            const uint8_t outputNumber = request.getSingleArg(0);
            response.setSingleArg(0, outputNumber);

        }
            break;

        default:
            return;
    }

    networkProtocol->send(response);
}

void doReadEpeverData() {
    epeverClient->readData();
}

void doReadEpeverStatus() {
    epeverClient->readStatus();
}

void doEvaluate() {
    const EpeverData& data = epeverClient->getData();

    serialDebug("Data: ");

    if (data.isValid()) {
        serialDebugln("Valid");

        serialDebug("- Panel voltage: ");
        serialDebugln(data.getPanelVoltage());
        serialDebug(" - Panel current: ");
        serialDebugln(data.getPanelCurrent());
        serialDebug(" - Battery voltage: ");
        serialDebugln(data.getBatteryVoltage());
        serialDebug(" - Battery charge current: ");
        serialDebugln(data.getBatteryChargeCurrent());
    } else {
        serialDebugln("INVALID !!!");
    }

    const EpeverStatus& status = epeverClient->getStatus();

    serialDebug("Status: ");

    if (status.isValid()) {
        serialDebugln("Valid");

        serialDebug(" - Wrong Voltage Identification: ");
        serialDebugln(status.isWrongVoltageIdentification());
        serialDebug(" - Panel current: ");
        serialDebugln(temperatureToString(status.getTemperature()));
        serialDebug(" - Battery voltage: ");
        serialDebugln(batteryToString(status.getBattery()));
        serialDebug(" - Charging method: ");
        serialDebugln(chargingToString(status.getCharging()));
        serialDebug(" - Panel Status: ");
        serialDebugln(arraysToString(status.getArrays()));
        serialDebug(" - Load Status: ");
        serialDebugln(loadToString(status.getLoad()));
    } else {
        serialDebugln("INVALID !!!");
    }
}
