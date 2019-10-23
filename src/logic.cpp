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
{}

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
    if (egt::detail::change_if_diff<>(m_target, value))
    {
        process();
        invoke_handlers({eventid::event1});
    }
}

void Logic::change_current(float value)
{
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

void Logic::set_status(status s, bool fan)
{
    auto f1 = egt::detail::change_if_diff<>(m_fan_status, fan);
    if (f1)
    {
        cout << "fan: " << m_fan_status << endl;

        // set fan output to m_fan_status
    }

    auto s1 = egt::detail::change_if_diff<>(m_status, s);
    if (s1)
    {
        cout << "status: " << status_str(m_status) << endl;

        // set cool/heat output to m_status
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
