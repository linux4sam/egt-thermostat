/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef LOGIC_H
#define LOGIC_H

#include <egt/ui>
#include <string>

/**
 *
 */
class Logic : public egt::detail::Object
{
public:
    enum class mode
    {
        off,
        automatic,
        cooling,
        heating,
    };

    enum class fanmode
    {
        on,
        automatic,
    };

    enum class status
    {
        off,
        cooling,
        heating,
    };

    Logic();

    static std::string status_str(status s);

    void change_target(float value);

    inline float target() const { return m_target; }

    void change_current(float value);

    inline float current() const { return m_current; }

    void process();

    void set_mode(mode m);

    void set_fan_mode(fanmode m);

    inline status current_status() const { return m_status; }

    void refresh();

    virtual ~Logic() = default;

protected:

    void set_status(status s, bool fan);

    float m_current{0.f};
    float m_target{20.f};

    mode m_mode{mode::automatic};
    status m_status{status::off};

    fanmode m_fan_mode{fanmode::automatic};
    bool m_fan_status{false};
};

#endif
