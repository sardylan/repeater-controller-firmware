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

Relais *Relais::instance = nullptr;

Relais *Relais::getInstance() {
    if (instance == nullptr)
        instance = new Relais();
    return instance;
}

Relais::Relais() {
    for (size_t B = 0; B <= (RELAIS_NUMBER - 1) / 8; B++) {
        const uint8_t data = EEPROM.read(EEPROM_ADDRESS_RELAIS_START + B);

        for (size_t bt = 0; B * 8 + bt < RELAIS_NUMBER; bt++) {
            status[B * 8 + bt] = static_cast<bool>(data & 1 << bt);
        }
    }
}

Relais::~Relais() = default;

bool Relais::getStatus(const int item) const {
    return status[item];
}

void Relais::setStatus(const int item, const bool newStatus) {
    status[item] = newStatus;

    const size_t B = item / 8;
    uint8_t data = EEPROM.read(EEPROM_ADDRESS_RELAIS_START + B);

    if (status[item])
        data |= 0b00000001 << item % 8;
    else
        data &= ~0b00000001 << item % 8;

    EEPROM.write(EEPROM_ADDRESS_RELAIS_START + B, data);
}
