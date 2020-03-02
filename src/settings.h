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

struct Settings : private egt::detail::NonCopyable<Settings>
{
    /// this does not currently support nesting of transactions
    struct AutoTransaction
    {
        explicit AutoTransaction(Settings& settings)
            : m_settings(settings)
        {
            m_settings.begin_tx();
        }

        ~AutoTransaction()
        {
            m_settings.end_tx();
        }

        Settings& m_settings;
    };

    using default_value_callback_t = std::function<std::string(const std::string&)>;

    Settings();

    void set_default_callback(default_value_callback_t callback);

    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);

    void temp_log(float temp);
    void status_log(Logic::status status, bool fan);

    void begin_tx();
    void end_tx();

    struct settings_impl;
    std::unique_ptr<settings_impl> m_impl;
    default_value_callback_t m_default_callback;
};

Settings& settings();

void get_timezones(std::vector<std::string>& timezones);

#endif
