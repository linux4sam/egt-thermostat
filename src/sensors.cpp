/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sensors.h"
#include <iostream>

#ifdef HAVE_SENSORS
#include <sensors/sensors.h>
#else
#warning "libsensors not found.  Can't read sensors."
#endif

using namespace std;

static void init_sensors()
{
    static bool i = false;
    if (!i)
    {
#ifdef HAVE_SENSORS
        sensors_init(NULL);
#endif
        i = true;
    }
}

std::vector<std::string> enumerate_temp_sensors()
{
    init_sensors();

    std::vector<std::string> res;

    // add fake sensor
    res.push_back("fake");

#ifdef HAVE_SENSORS
    sensors_chip_name const* cn;
    int c = 0;
    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        std::cout << "Chip: " << cn->prefix << cn->path << std::endl;

        sensors_feature const* feat;
        int f = 0;
        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            std::cout << f << ": " << feat->name << std::endl;

            switch (feat->type)
            {
            case SENSORS_FEATURE_TEMP:
                res.push_back(std::string(cn->prefix) + std::string(cn->path) + "/" + std::to_string(f));
                break;
            default:
                break;
            }

            sensors_subfeature const* subf;
            int s = 0;
            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0)
            {
                std::cout << f << ":" << s << ":" << subf->name
                          << "/" << subf->number << " = ";
                double val;
                if (subf->flags & SENSORS_MODE_R)
                {
                    int rc = sensors_get_value(cn, subf->number, &val);
                    if (rc < 0)
                    {
                        std::cout << "err: " << rc;
                    }
                    else
                    {
                        std::cout << val;
                    }
                }
                std::cout << std::endl;
            }
        }
    }
#endif

    return res;
}

double get_temp_sensor(const std::string& name)
{
    init_sensors();

    double res = 0.;

    if (name == "fake")
    {
        res = 22.;
        return res;
    }

#ifdef HAVE_SENSORS
    sensors_chip_name const* cn;
    int c = 0;
    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        sensors_feature const* feat;
        int f = 0;
        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            switch (feat->type)
            {
            case SENSORS_FEATURE_TEMP:
                if (name == std::string(cn->prefix) + std::string(cn->path) + "/" + std::to_string(f))
                {
                    // @todo probably a good idea to check
                    // SENSORS_SUBFEATURE_TEMP_FAULT before reading the value as good

                    sensors_subfeature const* subf =
                        sensors_get_subfeature(cn, feat, SENSORS_SUBFEATURE_TEMP_INPUT);
                    if (subf && sensors_get_value(cn, subf->number, &res) == 0)
                    {
                        return res;
                    }
                }

                break;
            default:
                break;
            }
        }
    }
#endif

    return res;
}
