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

#include "config.hpp"
#include "const.hpp"
#include "utils.hpp"

Config config;

EpeverClient *epeverClient;
NetworkProtocol *networkProtocol;
ClockUtility *clockUtility;

declareLastExecution(ReceiveCommand);
declareLastExecution(ReadEpeverData);
declareLastExecution(ReadEpeverStatus);
declareLastExecution(EvaluateRelais);

void setup()
{
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
    constexpr uint8_t mac[] NETWORK_MAC_ADDRESS;
    networkProtocol = NetworkProtocol::getInstance(mac, NETWORK_IP, NETWORK_DNS, NETWORK_GATEWAY, NETWORK_SUBNET);
    networkProtocol->setUdpPort(NETWORK_UDP_PORT);
    networkProtocol->begin();
    serialDebugln("done");

    serialDebug("Configuring ClockUtility... ");
    clockUtility = ClockUtility::getInstance();
    clockUtility->begin();
    serialDebugln("done");
}

void loop()
{
    getCurrentMillis();

    executeEvery(ReceiveCommand, 250)
    executeEvery(ReadEpeverData, 1000)
    executeEvery(ReadEpeverStatus, 1000)
    executeEvery(EvaluateRelais, 1000)
}

void doReceiveCommand()
{
    const NetworkCommand request = networkProtocol->receiveCommand();
    if (!request.isValid())
        return;

    serialDebugln(request);

    NetworkCommand response(request.getCommand(), request.getIp(), request.getPort());

    switch (request.getCommand())
    {
    case Command::Ping:
        {
            const CustomDateTime dateTime = ClockUtility::now();
            uint32_t unixtime = dateTime.unixtime();

            swapEndian(unixtime);
            response.setArg(0, &unixtime, sizeof(uint32_t));
        }
        break;

    case Command::Reset:
        {
            resetFunc();
        }
        break;

    case Command::Telemetry:
        {
            const EpeverData &data = epeverClient->getData();

            float panelVoltage = data.getPanelVoltage();
            float panelCurrent = data.getPanelCurrent();
            float batteryVoltage = data.getBatteryVoltage();
            float batteryChargeCurrent = data.getBatteryChargeCurrent();

            swapEndian(panelVoltage);
            swapEndian(panelVoltage);
            swapEndian(panelVoltage);
            swapEndian(panelVoltage);

            const CustomDateTime dateTime = ClockUtility::now();
            uint32_t unixtime = dateTime.unixtime();

            swapEndian(unixtime);
            response.setArg(0, &unixtime, sizeof(uint32_t));
            response.setArg(4, reinterpret_cast<uint8_t *>(&panelVoltage), sizeof(float));
            response.setArg(8, reinterpret_cast<uint8_t *>(&panelCurrent), sizeof(float));
            response.setArg(12, reinterpret_cast<uint8_t *>(&batteryVoltage), sizeof(float));
            response.setArg(16, reinterpret_cast<uint8_t *>(&batteryChargeCurrent), sizeof(float));
        }
        break;

    case Command::RTCRead:
        {
            const CustomDateTime dateTime = ClockUtility::now();
            uint32_t unixtime = dateTime.unixtime();

            swapEndian(unixtime);
            response.setArg(0, &unixtime, sizeof(uint32_t));
        }
        break;

    case Command::RTCSet:
        {
            time_t newUnixtime;
            request.getArg(0, &newUnixtime, sizeof(time_t));
            swapEndian(newUnixtime);
            clockUtility->setEpoch(newUnixtime);

            const CustomDateTime dateTime = ClockUtility::now();
            uint32_t unixtime = dateTime.unixtime();

            swapEndian(unixtime);
            response.setArg(0, &unixtime, sizeof(uint32_t));
        }
        break;

    case Command::ConfigRead:
        {
            const uint8_t configParam = request.getSingleArg(0);
            response.setSingleArg(0, configParam);

            switch (configParam)
            {
            case CONFIG_MAIN_VOLTAGE_OFF_PARAM:
                {
                    float mainVoltageOff = config.getMainVoltageOff();
                    swapEndian(mainVoltageOff);
                    response.setArg(1, reinterpret_cast<uint8_t *>(&mainVoltageOff), sizeof(float));
                }
                break;

            case CONFIG_MAIN_VOLTAGE_ON_PARAM:
                {
                    float mainVoltageOn = config.getMainVoltageOn();
                    swapEndian(mainVoltageOn);
                    response.setArg(1, reinterpret_cast<uint8_t *>(&mainVoltageOn), sizeof(float));
                }
                break;

            default:
                break;
            }
        }
        break;

    case Command::ConfigSet:
        {
            const uint8_t configParam = request.getSingleArg(0);
            response.setSingleArg(0, configParam);

            switch (configParam)
            {
            case CONFIG_MAIN_VOLTAGE_OFF_PARAM:
                {
                    float value;
                    request.getArg(1, &value, sizeof(float));
                    config.setMainVoltageOff(value);

                    float mainVoltageOff = config.getMainVoltageOff();
                    swapEndian(mainVoltageOff);
                    response.setArg(1, reinterpret_cast<uint8_t *>(&mainVoltageOff), sizeof(float));
                }
                break;

            case CONFIG_MAIN_VOLTAGE_ON_PARAM:
                {
                    float value;
                    request.getArg(1, &value, sizeof(float));
                    config.setMainVoltageOn(value);

                    float mainVoltageOn = config.getMainVoltageOn();
                    swapEndian(mainVoltageOn);
                    response.setArg(1, reinterpret_cast<uint8_t *>(&mainVoltageOn), sizeof(float));
                }
                break;

            default:
                break;
            }
        }
        break;

    case Command::OutputRead:
        {
            const uint8_t outputNumber = request.getSingleArg(0);
            response.setSingleArg(0, outputNumber);
            response.setSingleArg(1, outputNumber);
        }
        break;

    case Command::OutputSet:
        {
            const uint8_t outputNumber = request.getSingleArg(0);
            response.setSingleArg(0, outputNumber);
            response.setSingleArg(1, outputNumber);
        }
        break;

    default:
        return;
    }

    networkProtocol->send(response);
}

void doReadEpeverData() { epeverClient->readData(); }

void doReadEpeverStatus() { epeverClient->readStatus(); }

void doEvaluateRelais()
{
    const EpeverData &data = epeverClient->getData();
    if (!data.isValid())
    {
        return;
    }

    // const float batteryVoltage = data.getBatteryVoltage();
}
