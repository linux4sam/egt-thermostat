/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "logic.h"
#include "settings.h"
#include <chrono>
#include <egt/detail/math.h>
#include <iostream>

using namespace egt;
using namespace std;

Logic::Logic()
    : m_timer(std::chrono::seconds(1))
{
    // process() every second
    m_timer.on_timeout([this]()
    {
        process();
    });
    m_timer.start();
}

std::string Logic::status_str(status s)
{
    switch (s)
    {
    case status::off:
        return "Idle";
    case status::cooling:
        return "Cooling";
    case status::heating:
        return "Heating";
    }

    return {};
}

void Logic::change_target(float value)
{
    value = std::round(value);

    if (egt::detail::change_if_diff<>(m_target, value))
    {
        process();
        invoke_handlers({eventid::event1});
    }
}

void Logic::change_current(float value)
{
    value = std::round(value);

    if (egt::detail::change_if_diff<>(m_current, value))
    {
        process();
        invoke_handlers({eventid::event1});
    }
}

void Logic::process()
{
    if (detail::float_compare(m_current, 0))
        return;

    static const auto TEMP_EPSILON = 0.5f;

    // change status based on mode and current temps
    switch (m_mode)
    {
    case mode::automatic:
        if (m_current > m_target &&
            std::abs(m_current - m_target) > TEMP_EPSILON)
            set_status(status::cooling, true);
        else if (m_current < m_target &&
                 std::abs(m_current - m_target) > TEMP_EPSILON)
            set_status(status::heating, true);
        else
            set_status(status::off, m_fan_mode == fanmode::on);
        break;
    case mode::cooling:
        if (m_current > m_target &&
            std::abs(m_current - m_target) > TEMP_EPSILON)
            set_status(status::cooling, true);
        else
            set_status(status::off, m_fan_mode == fanmode::on);
        break;
    case mode::heating:
        if (m_current < m_target &&
            std::abs(m_current - m_target) > TEMP_EPSILON)
            set_status(status::heating, true);
        else
            set_status(status::off, m_fan_mode == fanmode::on);
        break;
    case mode::off:
        set_status(status::off, m_fan_mode == fanmode::on);
        break;
    };
}

void Logic::set_mode(mode m)
{
    if (egt::detail::change_if_diff<>(m_mode, m))
    {
        process();
        invoke_handlers({eventid::event1});
    }
}

void Logic::set_fan_mode(fanmode m)
{
    if (egt::detail::change_if_diff<>(m_fan_mode, m))
    {
        process();
        invoke_handlers({eventid::event1});
    }
}

#if 0
struct Output
{
    static const auto COMPRESSOR_SAFETY_DELTA = std::chrono::seconds(60);

    // TODO: hardware output

    // we can't do anything if not outside of safety delta
    if (m_last_compressor_time.time_since_epoch().count())
    {
        auto now = std::chrono::steady_clock::now();
        auto diff = now - m_last_compressor_time;
        if (diff <= COMPRESSOR_SAFETY_DELTA)
            return;
    }

    m_last_compressor_time = std::chrono::steady_clock::now();

    std::chrono::time_point<std::chrono::steady_clock> m_last_compressor_time{};

};
#endif

void Logic::set_status(status s, bool fan)
{
    auto f1 = egt::detail::change_if_diff<>(m_fan_status, fan);
    if (f1)
    {
        cout << "fan: " << m_fan_status << endl;

        //static Output output("fan");
        //output.go(m_status);
    }

    auto s1 = egt::detail::change_if_diff<>(m_status, s);
    if (s1)
    {
        cout << "status: " << status_str(m_status) << endl;

        //static Output output("ac/heat");
        //output.go(m_status);
    }

    if (f1 || s1)
    {
        settings().status_log(m_status, m_fan_status);
        invoke_handlers({eventid::event1});
    }
}

void Logic::refresh()
{
    invoke_handlers({eventid::event1});
}
