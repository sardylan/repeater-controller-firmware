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

#ifndef STATION_MGMT__OUTPUT__H
#define STATION_MGMT__OUTPUT__H

#include "const.hpp"

#define RELAIS_NUMBER 8

class Relais {

public:

    static Relais *getInstance();

    [[nodiscard]] bool getStatus(int item) const;

    void setStatus(int item, const bool &newStatus);

private:

    static Relais *instance;

    explicit Relais();

    ~Relais();

    bool status[RELAIS_NUMBER]{};

};

#endif
