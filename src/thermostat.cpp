/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/imagecache.h>
#include <egt/ui>
#include <iostream>

#include "logic.h"
#include "pages.h"
#include "window.h"
#include "sensors.h"
#include "settings.h"

using namespace std;
using namespace egt;

/*
static std::string current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%A, %B %e %r");
    return ss.str();
}
*/

static std::string current_time()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%r");
    return ss.str();
}

static std::string current_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%A, %B %e");
    return ss.str();
}

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    detail::add_search_path("./images");

    global_theme().palette().set(Palette::ColorId::label_text, Palette::white);
    global_theme().palette().set(Palette::ColorId::label_bg, Color(Palette::cyan, 30), Palette::GroupId::active);
    global_theme().palette().set(Palette::ColorId::button_text, Palette::white);
    global_theme().palette().set(Palette::ColorId::border, Palette::cyan);
    global_theme().palette().set(Palette::ColorId::border, Palette::cyan, Palette::GroupId::active);
    global_theme().palette().set(Palette::ColorId::border, Palette::cyan, Palette::GroupId::checked);
    global_theme().palette().set(Palette::ColorId::button_bg, Color(Palette::cyan, 30), Palette::GroupId::active);

    auto b = settings().get("normal_brightness");
    if (b.empty())
        settings().set("normal_brightness", std::to_string(main_screen()->max_brightness()));

    main_screen()->set_brightness(std::stoi(settings().get("normal_brightness")));

    ThermostatWindow win;
    win.show();

    auto l1 = win.find_child<Label>("date_label1");
    assert(l1);
    l1->set_text(current_date());
    auto l2 = win.find_child<Label>("date_label2");
    assert(l2);
    l2->set_text(current_date());

    auto l3 = win.find_child<Label>("time_label1");
    assert(l3);
    l3->set_text(current_time());
    auto l4 = win.find_child<Label>("time_label2");
    assert(l4);
    l4->set_text(current_time());

    PeriodicTimer time_timer(std::chrono::seconds(1));
    time_timer.on_timeout([&win]()
    {
        auto l1 = win.find_child<Label>("date_label1");
        assert(l1);
        l1->set_text(current_date());
        auto l2 = win.find_child<Label>("date_label2");
        assert(l2);
        l2->set_text(current_date());

        auto l3 = win.find_child<Label>("time_label1");
        assert(l3);
        l3->set_text(current_time());
        auto l4 = win.find_child<Label>("time_label2");
        assert(l4);
        l4->set_text(current_time());
    });
    time_timer.start();

    // TODO: hard code?
    Timer screen_brightness_timer(std::chrono::seconds(10));
    screen_brightness_timer.on_timeout([&win]()
    {
        auto b = settings().get("sleep_brightness",
                                main_screen()->max_brightness() / 2);

        main_screen()->set_brightness(b);
    });

    // TODO: this needs to be use and be reset with setting
    PeriodicTimer idle_timer(std::chrono::seconds(10));
    idle_timer.on_timeout([&win, &screen_brightness_timer]()
    {
        win.idle();
        screen_brightness_timer.start();
    });
    idle_timer.start();

    // on any input, reset idle timer
    Input::global_input().on_event([&idle_timer, &screen_brightness_timer](Event & event)
    {
        screen_brightness_timer.cancel();
        main_screen()->set_brightness(std::stoi(settings().get("normal_brightness")));
        idle_timer.start();
    }, {eventid::raw_pointer_down,
        eventid::raw_pointer_up,
        eventid::raw_pointer_move
       });

    // automatically select the first temp sensor if one is available
    if (settings().get("temp_sensor").empty())
    {
        const auto sensors = enumerate_temp_sensors();
        if (!sensors.empty())
        {
            settings().set("temp_sensor", sensors[0]);
        }
    }

    win.m_logic.change_current(get_temp_sensor(settings().get("temp_sensor")));

    PeriodicTimer sensor_timer(std::chrono::seconds(1));
    sensor_timer.on_timeout([&win]()
    {
        auto temp = get_temp_sensor(settings().get("temp_sensor"));
        win.m_logic.change_current(temp);
        settings().temp_log(temp);
    });
    sensor_timer.start();

    return app.run();
}
