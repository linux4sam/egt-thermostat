/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PAGES_H
#define PAGES_H

#include <egt/ui>
#include "logic.h"

class ThermostatWindow;

struct ThermostatPage : public egt::NotebookTab
{
    ThermostatPage(ThermostatWindow& window, Logic& logic)
        : m_window(window),
          m_logic(logic)
    {
        set_color(egt::Palette::ColorId::bg, egt::Color::css("#4a5157"));
    }

    virtual ~ThermostatPage() = default;

    ThermostatWindow& m_window;
    Logic& m_logic;
};

struct IdlePage : public ThermostatPage
{
    IdlePage(ThermostatWindow& window, Logic& logic);

    virtual void enter() override;

    void apply_logic_change(Logic::status status);

    std::shared_ptr<egt::Label> m_temp;
    std::shared_ptr<egt::Label> m_status;
    std::shared_ptr<egt::ImageLabel> m_otemp;
};

struct MainPage : public ThermostatPage
{
    MainPage(ThermostatWindow& window, Logic& logic);

    virtual void enter() override;
    virtual bool leave() override;

    void apply_logic_change(Logic::status status);

    std::shared_ptr<egt::Label> m_temp;
    std::shared_ptr<egt::Label> m_status;
    std::shared_ptr<egt::ImageButton> m_mode;
    std::shared_ptr<egt::ImageButton> m_fan;
    std::shared_ptr<egt::VerticalBoxSizer> m_layout;
    std::shared_ptr<egt::CameraWindow> m_camera;
    std::shared_ptr<egt::ImageLabel> m_otemp;
};

struct SchedulePage : public ThermostatPage
{
    SchedulePage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;
};

struct ModePage : public ThermostatPage
{
    ModePage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::unique_ptr<egt::ButtonGroup> m_button_group;
};

struct FanPage : public ThermostatPage
{
    FanPage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::unique_ptr<egt::ButtonGroup> m_button_group;
};

struct IdleSettingsPage : public ThermostatPage
{
    IdleSettingsPage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::shared_ptr<egt::Slider> m_idle_timeout;
    std::shared_ptr<egt::Slider> m_sleep_brightness;
};

struct ScreenBrightnessPage : public ThermostatPage
{
    ScreenBrightnessPage(ThermostatWindow& window, Logic& logic);
};

struct MenuPage : public ThermostatPage
{
    MenuPage(ThermostatWindow& window, Logic& logic);
};

struct HomeContentPage : public ThermostatPage
{
    HomeContentPage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::shared_ptr<egt::ToggleBox> m_degrees;
    std::shared_ptr<egt::ToggleBox> m_usebackground;
    std::shared_ptr<egt::ToggleBox> m_showoutside;
};

struct SensorsPage : public ThermostatPage
{
    SensorsPage(ThermostatWindow& window, Logic& logic);

    std::unique_ptr<egt::ButtonGroup> m_checkbox_group;

    virtual bool leave() override;
};

struct AboutPage : public ThermostatPage
{
    AboutPage(ThermostatWindow& window, Logic& logic);
};

#endif
