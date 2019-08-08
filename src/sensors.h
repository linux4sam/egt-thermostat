/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SENSORS_H
#define SENSORS_H

#include <vector>
#include <string>

std::vector<std::string> enumerate_temp_sensors();

double get_temp_sensor(const std::string& name);

#endif
