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

#include "relais.hpp"

#include <Arduino.h>
#include <EEPROM.h>

#include "eeprom.hpp"

Relais* Relais::instance = nullptr;

Relais* Relais::getInstance() {
    if (instance == nullptr)
        instance = new Relais();
    return instance;
}

Relais::Relais() {
    for (int i = 0; i < RELAIS_NUMBER; i++)
        status[i] = EEPROM.read(EEPROM_ADDRESS_RELAIS_START + i) > 0;
}

Relais::~Relais() = default;

bool Relais::getStatus(const int item) const {
    return status[item];
}

void Relais::setStatus(const int item, const bool newStatus) {
    status[item] = newStatus;
    EEPROM.write(EEPROM_ADDRESS_RELAIS_START + item, newStatus ? 0x01 : 0x00);
}
