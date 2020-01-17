/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "logic.h"
#include "pages.h"
#include "sensors.h"
#include "settings.h"
#include "window.h"
#include <egt/detail/imagecache.h>
#include <egt/ui>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace egt;

static std::string current_time()
{
    const auto now = std::chrono::system_clock::now();
    const auto in_time_t = std::chrono::system_clock::to_time_t(now);
    const auto local = std::localtime(&in_time_t);
    std::stringstream ss;
    if (settings().get("time_format") == "24")
        ss << std::put_time(local, "%H:%M:%S %p");
    else
        ss << std::put_time(local, "%I:%M:%S %p");
    auto result = ss.str();
    if (!result.empty() && result[0] == '0')
        result.erase(0, 1);
    return result;
}

static std::string current_date()
{
    const auto now = std::chrono::system_clock::now();
    const auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%A, %B %e");
    return ss.str();
}

static void update_time(ThermostatWindow& win)
{
    auto l1 = win.find_child<Label>("date_label1");
    if (l1)
        l1->text(current_date());
    auto l2 = win.find_child<Label>("date_label2");
    if (l2)
        l2->text(current_date());

    auto l3 = win.find_child<Label>("time_label1");
    if (l3)
        l3->text(current_time());
    auto l4 = win.find_child<Label>("time_label2");
    if (l4)
        l4->text(current_time());
}

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    add_search_path(DATADIR "/egt/thermostat/");
    add_search_path("./images");

    global_theme().palette().set(Palette::ColorId::label_text, Palette::white);
    global_theme().palette().set(Palette::ColorId::label_text, Palette::cyan, Palette::GroupId::active);
    global_theme().palette().set(Palette::ColorId::label_text, Palette::cyan, Palette::GroupId::checked);
    global_theme().palette().set(Palette::ColorId::label_bg, Color(Palette::cyan, 55), Palette::GroupId::active);
    global_theme().palette().set(Palette::ColorId::border, Palette::cyan);
    global_theme().palette().set(Palette::ColorId::border, Palette::cyan, Palette::GroupId::active);
    global_theme().palette().set(Palette::ColorId::border, Palette::cyan, Palette::GroupId::checked);
    global_theme().palette().set(Palette::ColorId::button_text, Palette::white);
    global_theme().palette().set(Palette::ColorId::button_text, Palette::white, Palette::GroupId::disabled);
    global_theme().palette().set(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::active);
    global_theme().palette().set(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::normal);
    global_theme().palette().set(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::checked);
    global_theme().palette().set(Palette::ColorId::button_bg, Color(Palette::black, 20), Palette::GroupId::disabled);

    /*
     * In some cases defaults would be defined by the database itself.  However,
     * we want to dynamically probe hardware for default values so Settings
     * provides a method to give us an opportunity to supply a default for any
     * "empty" value in the database.
     */
    settings().set_default_callback([](const std::string & key) -> std::string
    {

        if (key == "mode")
            return "auto";
        else if (key == "fan")
            return "auto";
        else if (key == "normal_brightness")
            return std::to_string(Application::instance().screen()->max_brightness());
        else if (key == "sleep_brightness")
            return std::to_string(Application::instance().screen()->max_brightness() / 2);
        else if (key == "temp_sensor")
        {
            const auto sensors = enumerate_temp_sensors();
            if (!sensors.empty())
                return sensors[0];
        }
        else if (key == "outside")
            return "on";
        else if (key == "background")
            return "on";
        else if (key == "sleep_timeout")
            return "20";
        else if (key == "target_temp")
            return "20";

        return std::string();
    });

    // set initial screen brightness
    Application::instance().screen()->brightness(std::stoi(settings().get("normal_brightness")));

    ThermostatWindow win;
    win.show();

    // update time labels periodically
    update_time(win);
    PeriodicTimer time_timer(std::chrono::seconds(1));
    time_timer.on_timeout([&win]()
    {
        update_time(win);
    });

    // update temp sensors periodically
    win.m_logic.change_current(get_temp_sensor(settings().get("temp_sensor")));
    PeriodicTimer sensor_timer(std::chrono::seconds(1));
    sensor_timer.on_timeout([&win]()
    {
        win.m_logic.change_current(get_temp_sensor(settings().get("temp_sensor")));
    });

    auto ret = app.run();

    Application::instance().screen()->brightness(
        Application::instance().screen()->max_brightness());

    return ret;
}
