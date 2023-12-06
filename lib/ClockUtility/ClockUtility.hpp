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

#ifndef STATION_MGMT__CLOCK_UTILITY__H
#define STATION_MGMT__CLOCK_UTILITY__H

#include <DS3231.h>

#include "CustomDateTime.hpp"

class ClockUtility {

public:

    static ClockUtility *getInstance();

    void begin();

    void setEpoch(time_t newTimestamp);

    [[nodiscard]] static CustomDateTime now();

private:

    ClockUtility();

    ~ClockUtility();

    static ClockUtility *instance;

    DS3231 rtc;

};

#endif