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
#include <ModbusMaster.h>
#include <Wire.h>

#include "config.hpp"
#include "const.hpp"
#include "enums.hpp"
#include "protocol.hpp"
#include "relais.hpp"
#include "utils.hpp"
#include "version.hpp"

#ifdef RTC_DS3231_ENABLED
    #include <DS3231.h>
DS3231 rtc;
#endif

#ifdef SENSOR_BMP280_ENABLED
    #include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;
#endif

Config config;

ModbusMaster node;

float panelVoltage;
float panelCurrent;
float batteryVoltage;
float batteryChargeCurrent;

float bmpTemp;
float bmpPressure;

bool statusWrongVoltageIdentification;
Temperature statusTemperature;
Battery statusBattery;
Charging statusCharging;
Arrays statusArrays;
Load statusLoad;

EthernetUDP udp;

Relais* relais;

declareLastExecution(ReceiveCommand);

#ifdef SENSOR_BMP280_ENABLED
declareLastExecution(ReadBMP);
#endif

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

#ifdef RTC_DS3231_ENABLED
    serialDebug("Configuring Clock... ");
    rtc.setClockMode(false);
    serialDebugln("done");
#endif

#ifdef SENSOR_BMP280_ENABLED
    serialDebug("Configuring BMP280... ");
    bmp.begin(SENSOR_BMP280_ADDRESS);

    serialDebug("Sensor ID: ");
    serialDebug(bmp.sensorID());

    bmp.setSampling(
        Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
    serialDebugln(" | done");
#endif

    serialDebug("Configuring Relais... ");
    relais = Relais::getInstance();
    globalStatus = false;
    serialDebugln("done");

    serialDebug("Configuring Relais pins... ");
    for (const int pin : relaisPins) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
    rainbow();
    serialDebugln("done");

    executeReset = false;
}

void loop() {
    getCurrentMillis();

    executeEvery(ReceiveCommand, 250);

#ifdef SENSOR_BMP280_ENABLED
    executeEvery(ReadBMP, 1000);
#endif

    executeEvery(ReadEpeverData, 1000);
    executeEvery(ReadEpeverStatus, 5000);
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

    printRXDebug(requestPacket, requestSize, remoteIp, remotePort);

    size_t responseSize = 1;
    responsePacket[0] = requestPacket[0];

    switch (requestPacket[0]) {
        case PROTOCOL_PING:
            {
                serialDebugln("Command PING");

                // responseSize += 4;

                // const DateTime dateTime = RTClib::now();
                // uint32_t unixtime = dateTime.unixtime();

                // swapEndian(unixtime);
                // memcpy(responsePacket + 1, &unixtime, sizeof(uint32_t));
            }
            break;

        case PROTOCOL_RESET:
            {
                serialDebugln("Command RESET");
                executeReset = true;
            }
            break;

        case PROTOCOL_TELEMETRY:
            {
                serialDebugln("Command TELEMETRY");

                // const DateTime dateTime = RTClib::now();
                // uint32_t unixtime = dateTime.unixtime();
                // swapEndian(unixtime);
                // memcpy(responsePacket + 1, &unixtime, sizeof(uint32_t));

                float tempPanelVoltage = panelVoltage;
                swapEndian(tempPanelVoltage);
                memcpy(responsePacket + responseSize, &tempPanelVoltage, sizeof(float));
                responseSize += 4;

                float tempPanelCurrent = panelCurrent;
                swapEndian(tempPanelCurrent);
                memcpy(responsePacket + responseSize, &tempPanelCurrent, sizeof(float));
                responseSize += 4;

                float tempBatteryVoltage = batteryVoltage;
                swapEndian(tempBatteryVoltage);
                memcpy(responsePacket + responseSize, &tempBatteryVoltage, sizeof(float));
                responseSize += 4;

                float tempBatteryChargeCurrent = batteryChargeCurrent;
                swapEndian(tempBatteryChargeCurrent);
                memcpy(responsePacket + responseSize, &tempBatteryChargeCurrent, sizeof(float));
                responseSize += 4;

                responsePacket[responseSize] = globalStatus ? 0x01 : 0x00;
                responseSize += 1;
            }
            break;

        case PROTOCOL_STATUS:
            {
                serialDebugln("Command STATUS");

                *(responsePacket + responseSize) = statusWrongVoltageIdentification ? 0x01 : 0x00;
                responseSize += 1;

                *(responsePacket + responseSize) = static_cast<char>(statusTemperature);
                responseSize += 1;

                *(responsePacket + responseSize) = static_cast<char>(statusBattery);
                responseSize += 1;

                *(responsePacket + responseSize) = static_cast<char>(statusCharging);
                responseSize += 1;

                *(responsePacket + responseSize) = static_cast<char>(statusArrays);
                responseSize += 1;

                *(responsePacket + responseSize) = static_cast<char>(statusLoad);
                responseSize += 1;
            }
            break;

        case PROTOCOL_METEO:
            {
                serialDebugln("Command METEO");

                float tempBmpPressure = bmpPressure;
                swapEndian(tempBmpPressure);
                memcpy(responsePacket + responseSize, &tempBmpPressure, sizeof(float));
                responseSize += 4;

                float tempBmpTemp = bmpTemp;
                swapEndian(tempBmpTemp);
                memcpy(responsePacket + responseSize, &tempBmpTemp, sizeof(float));
                responseSize += 4;
            }
            break;

#ifdef RTC_DS3231_ENABLED
        case PROTOCOL_RTC_READ:
            {
                const DateTime dateTime = RTClib::now();
                uint32_t unixtime = dateTime.unixtime();
                swapEndian(unixtime);
                memcpy(responsePacket + responseSize, &unixtime, sizeof(uint32_t));
                responseSize += sizeof(uint32_t);
            }
            break;

        case PROTOCOL_RTC_SET:
            {
                time_t newUnixtime;
                memcpy(&newUnixtime, requestPacket + 1, sizeof(time_t));
                swapEndian(newUnixtime);
                rtc.setEpoch(newUnixtime);

                const DateTime dateTime = RTClib::now();
                uint32_t unixtime = dateTime.unixtime();
                swapEndian(unixtime);
                memcpy(responsePacket + responseSize, &unixtime, sizeof(uint32_t));
                responseSize += sizeof(uint32_t);
            }
            break;
#endif

        case PROTOCOL_CONFIG_READ:
            {
                serialDebugln("Command CONFIG_READ");

                responsePacket[1] = requestPacket[1];
                responseSize += 1;

                switch (requestPacket[1]) {
                    case CONFIG_MAIN_VOLTAGE_OFF_PARAM:
                        {
                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            memcpy(responsePacket + responseSize, &mainVoltageOff, sizeof(float));
                            responseSize += sizeof(float);
                        }
                        break;

                    case CONFIG_MAIN_VOLTAGE_ON_PARAM:
                        {
                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            memcpy(responsePacket + responseSize, &mainVoltageOn, sizeof(float));
                            responseSize += sizeof(float);
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case PROTOCOL_CONFIG_SET:
            {
                serialDebugln("Command CONFIG_SET");

                responsePacket[1] = requestPacket[1];
                responseSize += 1;

                switch (requestPacket[1]) {
                    case CONFIG_MAIN_VOLTAGE_OFF_PARAM:
                        {
                            float value;
                            memcpy(&value, requestPacket + 2, sizeof(float));
                            swapEndian(value);
                            serialDebug("New Main Voltage OFF: ");
                            serialDebugln(value);
                            config.setMainVoltageOff(value);

                            float mainVoltageOff = config.getMainVoltageOff();
                            swapEndian(mainVoltageOff);
                            memcpy(responsePacket + responseSize, &mainVoltageOff, sizeof(float));
                            responseSize += sizeof(float);
                        }
                        break;

                    case CONFIG_MAIN_VOLTAGE_ON_PARAM:
                        {
                            float value;
                            memcpy(&value, requestPacket + 2, sizeof(float));
                            swapEndian(value);
                            serialDebug("New Main Voltage ON: ");
                            serialDebugln(value);
                            config.setMainVoltageOn(value);

                            float mainVoltageOn = config.getMainVoltageOn();
                            swapEndian(mainVoltageOn);
                            memcpy(responsePacket + responseSize, &mainVoltageOn, sizeof(float));
                            responseSize += sizeof(float);
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case PROTOCOL_OUTPUT_READ:
            {
                serialDebugln("Command OUTPUT_READ");

                const uint8_t outputNumber = requestPacket[1];
                responsePacket[1] = outputNumber;
                responsePacket[2] = relais->getStatus(outputNumber) ? 0x01 : 0x00;
                responseSize += 2;
            }
            break;

        case PROTOCOL_OUTPUT_SET:
            {
                serialDebugln("Command OUTPUT_SET");

                const uint8_t outputNumber = requestPacket[1];
                const bool newStatus = requestPacket[2] > 0;

                relais->setStatus(outputNumber, newStatus);

                responsePacket[1] = outputNumber;
                responsePacket[2] = relais->getStatus(outputNumber) ? 0x01 : 0x00;
                responseSize += 2;
            }
            break;

        default:
            {
                serialDebugln("Command not recognized!!! Sending NACK!!!");

                responsePacket[0] = PROTOCOL_NACK;
                responsePacket[1] = requestPacket[0];
                responseSize += 2;
            }
    }

    printTXDebug(responsePacket, responseSize, remoteIp, remotePort);

    udp.beginPacket(remoteIp, remotePort);
    udp.write(responsePacket, responseSize);
    udp.endPacket();
}

#ifdef SENSOR_BMP280_ENABLED
void doReadBMP() {
    bmpTemp = bmp.readTemperature();
    bmpPressure = bmp.readPressure() / 100.0f;

    serialDebugHeader("BMP280");
    serialDebug("Temp: ");
    serialDebug(bmpTemp);
    serialDebug(" - Pressure: ");
    serialDebugln(bmpPressure);
}
#endif

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
    statusWrongVoltageIdentification = tempBuffer & 0x8000;

    uint8_t tempData = (tempBuffer & 0x00F0) >> 4;    // D7-D4 shifted down
    statusTemperature = static_cast<Temperature>(tempData);

    tempData = (tempBuffer & 0x000F);    // D3-D0
    statusBattery = static_cast<Battery>(tempData);

    tempBuffer = node.getResponseBuffer(0x01);

    tempData = (tempBuffer & 0x000C) >> 2;    // D3-D2 shifted down
    statusCharging = static_cast<Charging>(tempData);

    tempData = (tempBuffer & 0xC000) >> 14;    // D15-D14 shifted down
    statusArrays = static_cast<Arrays>(tempData);

    tempBuffer = node.getResponseBuffer(0x02);
    tempData = (tempBuffer & 0x3000) >> 12;    // D3-12 shifted down
    statusLoad = static_cast<Load>(tempData);
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