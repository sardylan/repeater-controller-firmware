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

#ifndef STATION_MGMT__CONFIG__H
#define STATION_MGMT__CONFIG__H

#include <stdint.h>

#define CONFIG_MAIN_VOLTAGE_OFF_PARAM 'o'
#define CONFIG_MAIN_VOLTAGE_ON_PARAM 'O'

class Config {

public:

    Config();

    ~Config();

    [[nodiscard]] uint16_t getMainVoltageOff() const;

    void setMainVoltageOff(uint16_t newValue);

    [[nodiscard]] uint16_t getMainVoltageOn() const;

    void setMainVoltageOn(uint16_t newValue);

private:

    uint16_t mainVoltageOff;
    uint16_t mainVoltageOn;

};

#endif
