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
        color(egt::Palette::ColorId::bg, egt::Color(38, 50, 56));
    }

    virtual ~ThermostatPage() = default;

    virtual void shrink_camera() {}

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
    virtual void shrink_camera() override;

    void apply_logic_change(Logic::status status);

    std::shared_ptr<egt::ImageButton> m_menu;
    std::shared_ptr<egt::Label> m_temp;
    std::shared_ptr<egt::Label> m_status;
    std::shared_ptr<egt::ImageButton> m_mode;
    std::shared_ptr<egt::ImageButton> m_fan;
    std::shared_ptr<egt::VerticalBoxSizer> m_layout;
    std::shared_ptr<egt::CameraWindow> m_camera;
    std::shared_ptr<egt::ImageLabel> m_otemp;
    bool m_camera_fullscreen{true};
    egt::PeriodicTimer m_background_timer{std::chrono::seconds(5)};
    int m_background_index{0};
};

struct SettingsPage : public ThermostatPage
{
    SettingsPage(ThermostatWindow& window, Logic& logic)
        : ThermostatPage(window, logic)
    {}

    std::shared_ptr<egt::VerticalBoxSizer> create_layout(const std::string& title);

    virtual ~SettingsPage() = default;
};

struct ModePage : public SettingsPage
{
    ModePage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::unique_ptr<egt::ButtonGroup> m_button_group;
};

struct SchedulePage : public SettingsPage
{
    SchedulePage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::shared_ptr<egt::ToggleBox> m_enabled;
};

struct FanPage : public SettingsPage
{
    FanPage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::unique_ptr<egt::ButtonGroup> m_button_group;
};

struct IdleSettingsPage : public SettingsPage
{
    IdleSettingsPage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::shared_ptr<egt::Slider> m_idle_timeout;
    std::shared_ptr<egt::Slider> m_sleep_brightness;
};

struct ScreenBrightnessPage : public SettingsPage
{
    ScreenBrightnessPage(ThermostatWindow& window, Logic& logic);
};

struct MenuPage : public SettingsPage
{
    MenuPage(ThermostatWindow& window, Logic& logic);
};

struct HomeContentPage : public SettingsPage
{
    HomeContentPage(ThermostatWindow& window, Logic& logic);

    virtual bool leave() override;

    std::shared_ptr<egt::ToggleBox> m_degrees;
    std::shared_ptr<egt::ToggleBox> m_usebackground;
    std::shared_ptr<egt::ToggleBox> m_showoutside;
    std::shared_ptr<egt::ToggleBox> m_time_format;
};

struct SensorsPage : public SettingsPage
{
    SensorsPage(ThermostatWindow& window, Logic& logic);

    std::unique_ptr<egt::ButtonGroup> m_checkbox_group;

    virtual bool leave() override;
};

struct AboutPage : public SettingsPage
{
    AboutPage(ThermostatWindow& window, Logic& logic);
};

#endif
