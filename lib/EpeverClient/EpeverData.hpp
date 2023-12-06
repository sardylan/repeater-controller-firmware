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

#ifndef STATION_MGMT__EPEVER_DATA__H
#define STATION_MGMT__EPEVER_DATA__H

class EpeverData {

public:

    EpeverData(float panelVoltage, float panelCurrent, float batteryVoltage, float batteryChargeCurrent);

    EpeverData();

    EpeverData(const EpeverData &other);

    [[nodiscard]] bool isValid() const;

    [[nodiscard]] float getPanelVoltage() const;

    [[nodiscard]] float getPanelCurrent() const;

    [[nodiscard]] float getBatteryVoltage() const;

    [[nodiscard]] float getBatteryChargeCurrent() const;

private:

    bool valid;

    float panelVoltage;
    float panelCurrent;
    float batteryVoltage;
    float batteryChargeCurrent;

};

#endif
