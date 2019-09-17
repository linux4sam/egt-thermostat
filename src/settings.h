/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <egt/utils.h>
#include <string>
#include <memory>
#include "logic.h"
#include <vector>

struct Settings : public egt::detail::noncopyable
{
    Settings();

    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);

    int get(const std::string& key, int default_value);

    void temp_log(float temp);
    void status_log(Logic::status status, bool fan);

    struct settings_impl;
    std::unique_ptr<settings_impl> m_impl;
};

Settings& settings();

void get_timezones(std::vector<std::string>& timezones);

#endif
