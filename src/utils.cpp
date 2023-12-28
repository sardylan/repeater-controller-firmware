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

#include "utils.hpp"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void swapEndianness(void* var, const size_t size) {
    uint8_t tmp[size];
    memcpy(tmp, var, size);
    auto* dst = static_cast<uint8_t*>(var);
    for (size_t i = 0; i < size; i++)
        dst[size - i - 1] = tmp[i];
}

void payloadToHex(char* dest, const char* payload, const size_t& size) {
    char* output = dest;
    for (size_t i = 0; i < size; i++) {
        if (i > 0) {
            *output = ' ';
            output += 1;
        }
        sprintf(output, "%02x", static_cast<unsigned char>(payload[i]));
        output += 2;
    }
    *output = '\0';
}
