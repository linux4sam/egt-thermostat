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
{
    m_target = std::stof(settings().get("target_temp"));
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
    if (egt::detail::change_if_diff<>(m_target, value))
    {
        Settings::AutoTransaction tx(settings());

        settings().set("target_temp", std::to_string(m_target));

        process();
        on_logic_change.invoke();
    }
}

void Logic::change_current(float value)
{
    if (egt::detail::change_if_diff<>(m_current, value))
    {
        Settings::AutoTransaction tx(settings());

        process();
        on_temperature_change.invoke();
    }
}

void Logic::process()
{
    auto current = std::round(m_current);
    auto target = std::round(m_target);

    if (settings().get("degrees") == "f")
    {
        current = std::round(CtoF(m_current));
        target = std::round(CtoF(m_target));
    }

    // change status based on mode and current temps
    switch (m_mode)
    {
    case mode::automatic:
        if (current > target)
            change_status(status::cooling, true);
        else if (current < target)
            change_status(status::heating, true);
        else
            change_status(status::off, m_fan_mode == fanmode::on);
        break;
    case mode::cooling:
        if (current > target)
            change_status(status::cooling, true);
        else
            change_status(status::off, m_fan_mode == fanmode::on);
        break;
    case mode::heating:
        if (current < target)
            change_status(status::heating, true);
        else
            change_status(status::off, m_fan_mode == fanmode::on);
        break;
    case mode::off:
        change_status(status::off, m_fan_mode == fanmode::on);
        break;
    };
}

void Logic::set_mode(mode m)
{
    if (egt::detail::change_if_diff<>(m_mode, m))
    {
        process();
        on_logic_change.invoke();
    }
}

void Logic::set_fan_mode(fanmode m)
{
    if (egt::detail::change_if_diff<>(m_fan_mode, m))
    {
        process();
        on_logic_change.invoke();
    }
}

void Logic::change_status(status s, bool fan)
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
        on_logic_change.invoke();
    }
}

void Logic::refresh()
{
    on_logic_change.invoke();
}
