/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "config.h"
#include "settings.h"
#include <map>
#if ENABLE_DATABASE
#include <sqlite3pp.h>
#endif
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

#if ENABLE_DATABASE
static inline const char* db_path()
{
    auto install_path = DATADIR "/egt/thermostat/thermostat.db";
    if (fs::is_regular_file(install_path))
        return install_path;
    return "thermostat.db";
}
#endif

struct Settings::settings_impl
{
    std::map<std::string, std::string> cache;
#if ENABLE_DATABASE
    sqlite3pp::database db{db_path()};
    sqlite3pp::query config_qry{db,
                  "SELECT value FROM config WHERE key=:key LIMIT 1"};
    sqlite3pp::command config_cmd{db,
                  "REPLACE INTO config (key,value) VALUES (:key,:value)"};
#endif
};

Settings::Settings()
    : m_impl(new settings_impl)
{
#if ENABLE_DATABASE
    // store temp tables in memory
    m_impl->db.execute("PRAGMA temp_store = MEMORY");
#endif
}

void Settings::set_default_callback(default_value_callback_t callback)
{
    m_default_callback = callback;
}

void Settings::set(const std::string& key, const std::string& value)
{
    m_impl->cache[key] = value;

#if ENABLE_DATABASE
    m_impl->config_cmd.reset();
    m_impl->config_cmd.bind(":key", key, sqlite3pp::nocopy);
    m_impl->config_cmd.bind(":value", value, sqlite3pp::nocopy);
    m_impl->config_cmd.execute();
#endif
}

std::string Settings::get(const std::string& key)
{
    const auto c = m_impl->cache.find(key);
    if (c != m_impl->cache.end())
        return c->second;

#if ENABLE_DATABASE
    m_impl->config_qry.reset();
    m_impl->config_qry.bind(":key", key, sqlite3pp::nocopy);

    auto i = m_impl->config_qry.begin();
    if (i != m_impl->config_qry.end())
    {
        // careful for NULL values
        auto v = (*i).get<char const*>(0);
        if (v)
        {
            m_impl->cache[key] = v;
            return v;
        }
    }
#endif

    if (m_default_callback)
    {
        m_impl->cache[key] = m_default_callback(key);
        return m_impl->cache[key];
    }

    return {};
}

void Settings::temp_log(float temp)
{
#if ENABLE_DATABASE
    sqlite3pp::command cmd(m_impl->db,
                           "INSERT INTO temp_log (temp, datetime) VALUES (:temp, :datetime)");
    cmd.bind(":temp", temp);
    long long int since_epoch = std::chrono::steady_clock::now().time_since_epoch().count();
    cmd.bind(":datetime", since_epoch);
    cmd.execute();
#endif
}

void Settings::status_log(Logic::status status, bool fan)
{
#if ENABLE_DATABASE
    sqlite3pp::command cmd(m_impl->db,
                           "INSERT INTO status_log (status, fan, datetime) VALUES (:status, :fan, :datetime)");
    cmd.bind(":status", static_cast<int>(status));
    cmd.bind(":fan", static_cast<int>(fan));
    long long int since_epoch = std::chrono::steady_clock::now().time_since_epoch().count();
    cmd.bind(":datetime", since_epoch);
    cmd.execute();
#endif
}

void Settings::begin_tx()
{
#if ENABLE_DATABASE
    m_impl->db.execute("BEGIN");
#endif
}

void Settings::end_tx()
{
#if ENABLE_DATABASE
    m_impl->db.execute("COMMIT");
#endif
}

Settings& settings()
{
    static Settings s;
    return s;
}

static void get_timezones(std::vector<std::string>& timezones,
                          const std::string& dir,
                          const std::string& path)
{
    if (fs::exists(path))
    {
        for (const auto& p : fs::directory_iterator(path))
        {
            if (fs::is_regular_file(p.path()))
            {
                std::string s = p.path().filename();
                if (!dir.empty())
                    s = dir + std::string("/") + s;
                timezones.push_back(s);
            }
            else if (fs::is_directory(p.path()))
                get_timezones(timezones, dir + std::string("/") + p.path().filename().string(), p.path());
        }
    }
}

void get_timezones(std::vector<std::string>& timezones)
{
    const auto ZONEINFO = "/usr/share/zoneinfo/posix";

    get_timezones(timezones, "", ZONEINFO);

    std::sort(timezones.begin(), timezones.end());
}
