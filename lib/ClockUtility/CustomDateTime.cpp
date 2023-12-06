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

#include "CustomDateTime.hpp"

#include <Print.h>

CustomDateTime::CustomDateTime(const DateTime &dateTime) : DateTime(dateTime) {}

CustomDateTime::~CustomDateTime() = default;

size_t CustomDateTime::printTo(Print &p) const {
    struct tm tm{};
    uint32_t ts = unixtime();
    localtime_r(&ts, &tm);

    char buffer[20];
    size_t n = strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", &tm);
    p.print(buffer);
    return n;
}
