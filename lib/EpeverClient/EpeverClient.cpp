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

#include "EpeverClient.hpp"

EpeverClient *EpeverClient::instance = nullptr;

EpeverClient *EpeverClient::getInstance(Pin DI, Pin DE, Pin RE, Pin RO) {
    if (EpeverClient::instance == nullptr)
        EpeverClient::instance = new EpeverClient(DI, DE, RE, RO);

    return EpeverClient::instance;
}

EpeverClient::EpeverClient(Pin DI, Pin DE, Pin RE, Pin RO) :
        DI(DI), DE(DE), RE(RE), RO(RO){
//        softwareSerial(EpeverClient::RO, EpeverClient::DI) {
    data = {};
    status = {};

    node.preTransmission(preTransmissionProxy);
    node.postTransmission(postTransmissionProxy);
}

EpeverClient::~EpeverClient() = default;

const EpeverData &EpeverClient::getData() const {
    return data;
}

const EpeverStatus &EpeverClient::getStatus() const {
    return status;
}

void EpeverClient::begin() {
    pinMode(EpeverClient::RE, OUTPUT);
    pinMode(EpeverClient::DE, OUTPUT);

    postTransmission();

    Serial2.begin(MODBUS_BAUDRATE);

    node.begin(MODBUS_CLIENT_ID, Serial2);
}

void EpeverClient::preTransmission() const {
    digitalWrite(EpeverClient::RE, HIGH);
    digitalWrite(EpeverClient::DE, HIGH);
}

void EpeverClient::postTransmission() const {
    digitalWrite(EpeverClient::RE, LOW);
    digitalWrite(EpeverClient::DE, LOW);
}

void preTransmissionProxy() {
    EpeverClient::instance->preTransmission();
}

void postTransmissionProxy() {
    EpeverClient::instance->postTransmission();
}

void EpeverClient::readData() {
    uint8_t result = node.readInputRegisters(0x3100, 6);
    if (result != ModbusMaster::ku8MBSuccess) {
        data = {};
        return;
    }

    float panelsVoltage = node.getResponseBuffer(0x00) / 100.0f;
    float panelsCurrent = node.getResponseBuffer(0x01) / 100.0f;
    float batteryVoltage = node.getResponseBuffer(0x04) / 100.0f;
    float batteryChargeCurrent = node.getResponseBuffer(0x05) / 100.0f;

    data = {panelsVoltage, panelsCurrent, batteryVoltage, batteryChargeCurrent};
}

void EpeverClient::readStatus() {
    uint8_t result = node.readInputRegisters(0x3200, 3);
    if (result != ModbusMaster::ku8MBSuccess) {
        status = {};
        return;
    }

    uint16_t tempBuffer;
    uint8_t tempData;

    tempBuffer = node.getResponseBuffer(0x00);
    bool wrongVoltageIdentification = tempBuffer & 0x8000;

    tempData = (tempBuffer & 0x00F0) >> 4; // D7-D4 shifted down
    auto temperature = static_cast<Temperature>(tempData);

    tempData = (tempBuffer & 0x000F);   // D3-D0
    auto battery = static_cast<Battery>(tempData);

    tempBuffer = node.getResponseBuffer(0x01);

    tempData = (tempBuffer & 0x000C) >> 2; // D3-D2 shifted down
    auto charging = static_cast<Charging>(tempData);

    tempData = (tempBuffer & 0xC000) >> 14; // D15-D14 shifted down
    auto arrays = static_cast<Arrays>(tempData);

    tempBuffer = node.getResponseBuffer(0x02);
    tempData = (tempBuffer & 0x3000) >> 12; // D3-12 shifted down
    auto load = static_cast<Load>(tempData);

    status = {wrongVoltageIdentification, temperature, battery, charging, arrays, load};
}
