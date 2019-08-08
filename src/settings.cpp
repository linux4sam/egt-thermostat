/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "settings.h"
#include <sqlite3pp.h>

struct Settings::settings_impl
{
    sqlite3pp::database db{"thermostat.db"};
};

Settings::Settings()
    : m_impl(new settings_impl)
{}

void Settings::set(const std::string& key, const std::string& value)
{
    sqlite3pp::command cmd(m_impl->db,
                           "REPLACE INTO config (key, value) VALUES (:key, :value)");
    cmd.bind(":key", key, sqlite3pp::nocopy);
    cmd.bind(":value", value, sqlite3pp::nocopy);
    cmd.execute();
}

std::string Settings::get(const std::string& key)
{
    sqlite3pp::query qry(m_impl->db, "SELECT value FROM config WHERE key = :key");
    qry.bind(":key", key, sqlite3pp::nocopy);

    auto i = qry.begin();
    if (i != qry.end())
    {
        // careful for NULL values
        auto v = (*i).get<char const*>(0);
        if (v)
            return v;
    }

    return {};
}

int Settings::get(const std::string& key, int default_value)
{
    sqlite3pp::query qry(m_impl->db, "SELECT value FROM config WHERE key = :key");
    qry.bind(":key", key, sqlite3pp::nocopy);

    auto i = qry.begin();
    if (i != qry.end())
    {
        // careful for NULL values
        auto v = (*i).get<char const*>(0);
        if (v)
            return std::stoi(v);
    }

    return default_value;
}

void Settings::temp_log(float temp)
{
    sqlite3pp::command cmd(m_impl->db,
                           "INSERT INTO temp_log (temp, datetime) VALUES (:temp, :datetime)");
    cmd.bind(":temp", temp);
    long long int since_epoch = std::chrono::steady_clock::now().time_since_epoch().count();
    cmd.bind(":datetime", since_epoch);
    cmd.execute();
}

void Settings::status_log(Logic::status status, bool fan)
{
    sqlite3pp::command cmd(m_impl->db,
                           "INSERT INTO status_log (status, fan, datetime) VALUES (:status, :fan, :datetime)");
    cmd.bind(":status", static_cast<int>(status));
    cmd.bind(":fan", static_cast<int>(fan));
    long long int since_epoch = std::chrono::steady_clock::now().time_since_epoch().count();
    cmd.bind(":datetime", since_epoch);
    cmd.execute();
}

Settings& settings()
{
    static Settings s;
    return s;
}
