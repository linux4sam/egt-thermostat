/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "pages.h"
#include "window.h"
#include "settings.h"
#include "sensors.h"

using namespace egt;
using namespace egt::experimental;
using namespace std;

static inline int c2f(float c)
{
    return std::round((c * 1.8) + 32.0);
}

static inline std::string format_temp(float temp)
{
    ostringstream ss;
    if (settings().get("degrees") == "f")
        ss << c2f(temp) << "°";
    else
        ss << temp << "°";
    return ss.str();
}

enum {title_font_size = 24};

static auto selectable_btn_setup = [](shared_ptr<ImageButton>& button)
{
    button->set_color(Palette::ColorId::button_bg, Palette::transparent);
    button->set_color(Palette::ColorId::button_bg, Color(0, 255, 255, 30), Palette::GroupId::checked);
    button->set_color(Palette::ColorId::button_bg, Palette::transparent, Palette::GroupId::active);
    button->set_color(Palette::ColorId::border, Palette::transparent);
    button->set_boxtype(Theme::boxtype::fill);
    button->set_border(2);
    button->set_margin(5);
    button->set_padding(10);
    button->set_image_align(alignmask::center | alignmask::top);

    std::weak_ptr<ImageButton> weak_button(button);
    button->on_click([weak_button](Event&)
    {
        auto button = weak_button.lock();
        if (button)
            button->set_checked(true);
    });
};


ModePage::ModePage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    layout->set_color(Palette::ColorId::bg, Palette::gray);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<Label>("System Mode");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });

    auto sizer = make_shared<HorizontalBoxSizer>();
    layout->add(expand(sizer));

    auto mode_auto = make_shared<ImageButton>(Image("auto.png"), "Automatic");
    mode_auto->set_name("auto");
    mode_auto->set_checked(settings().get("mode") == "auto");
    selectable_btn_setup(mode_auto);
    sizer->add(mode_auto);

    auto mode_heating = make_shared<ImageButton>(Image("heating.png"), "Heating");
    mode_heating->set_name("heat");
    mode_heating->set_checked(settings().get("mode") == "heat");
    selectable_btn_setup(mode_heating);
    sizer->add(mode_heating);

    auto mode_cooling = make_shared<ImageButton>(Image("cooling.png"), "Cooling");
    mode_cooling->set_name("cool");
    mode_cooling->set_checked(settings().get("mode") == "cool");
    selectable_btn_setup(mode_cooling);
    sizer->add(mode_cooling);

    auto mode_off = make_shared<ImageButton>(Image("off.png"), "Off");
    mode_off->set_name("off");
    mode_off->set_checked(settings().get("mode") == "off");
    selectable_btn_setup(mode_off);
    sizer->add(mode_off);

    m_button_group = detail::make_unique<ButtonGroup>(true, true);
    m_button_group->add(mode_auto);
    m_button_group->add(mode_heating);
    m_button_group->add(mode_cooling);
    m_button_group->add(mode_off);
}

bool ModePage::leave()
{
    m_button_group->foreach_checked([this](Button & button)
    {
        settings().set("mode", button.name());

        if (button.name() == "auto")
            m_logic.set_mode(Logic::mode::automatic);
        else if (button.name() == "heat")
            m_logic.set_mode(Logic::mode::heating);
        else if (button.name() == "cool")
            m_logic.set_mode(Logic::mode::cooling);
        else if (button.name() == "off")
            m_logic.set_mode(Logic::mode::off);
    });

    return true;
}

FanPage::FanPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    layout->set_color(Palette::ColorId::bg, Palette::gray);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<Label>("Fan");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });

    auto sizer = make_shared<HorizontalBoxSizer>();
    layout->add(expand(sizer));

    auto mode_auto = make_shared<ImageButton>(Image("fan.png"), "Auto");
    mode_auto->set_name("auto");
    selectable_btn_setup(mode_auto);
    mode_auto->set_checked(settings().get("fan") == "auto");
    sizer->add(mode_auto);

    auto mode_off = make_shared<ImageButton>(Image("on.png"), "On");
    mode_off->set_name("on");
    selectable_btn_setup(mode_off);
    mode_auto->set_checked(settings().get("fan") == "on");
    sizer->add(mode_off);

    m_button_group = detail::make_unique<ButtonGroup>(true, true);
    m_button_group->add(mode_auto);
    m_button_group->add(mode_off);
}

bool FanPage::leave()
{
    m_button_group->foreach_checked([this](Button & button)
    {
        settings().set("fan", button.name());

        if (button.name() == "on")
            m_logic.set_fan_mode(Logic::fanmode::on);
        else
            m_logic.set_fan_mode(Logic::fanmode::automatic);
    });

    return true;
}


IdleSettingsPage::IdleSettingsPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<ImageLabel>("Sleep Mode");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });


    auto sizer = make_shared<VerticalBoxSizer>();
    sizer->set_margin(50);
    layout->add(expand(sizer));

    sizer->add(make_shared<Label>("Minutes idle before entering sleep mode"));

    m_idle_timeout = std::make_shared<Slider>(0, 60,
                     settings().get("sleep_timeout", 60));
    // idle_timeout->set_margin(50); // BROKE
    m_idle_timeout->set_height(50);
    m_idle_timeout->set_align(alignmask::expand_horizontal);
    m_idle_timeout->slider_flags().set({Slider::flag::round_handle, Slider::flag::show_label});
    sizer->add(m_idle_timeout);

    sizer->add(make_shared<Label>("Sleep screen brightness"));

    m_sleep_brightness = std::make_shared<Slider>(0,
                         main_screen()->max_brightness(),

                         settings().get("sleep_brightness",
                                        main_screen()->max_brightness()));
    m_sleep_brightness->set_height(50);
    m_sleep_brightness->set_align(alignmask::expand_horizontal);
    m_sleep_brightness->slider_flags().set({Slider::flag::round_handle, Slider::flag::show_label});
    sizer->add(m_sleep_brightness);
}

bool IdleSettingsPage::leave()
{
    settings().set("sleep_brightness", std::to_string(m_sleep_brightness->value()));
    settings().set("sleep_timeout", std::to_string(m_idle_timeout->value()));

    // TODO: have to reset idle timeout timer!

    return true;
}

ScreenBrightnessPage::ScreenBrightnessPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<ImageLabel>("Screen Brightness");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });


    auto sizer = make_shared<VerticalBoxSizer>();
    sizer->set_margin(50);
    layout->add(expand(sizer));

    sizer->add(make_shared<Label>("Screen brightness"));

    auto normal_brightness = std::make_shared<Slider>(0,
                             main_screen()->max_brightness(),
                             std::stoi(settings().get("normal_brightness")));
    normal_brightness->set_height(50);
    normal_brightness->set_align(alignmask::expand_horizontal);
    normal_brightness->slider_flags().set({Slider::flag::round_handle, Slider::flag::show_label});
    sizer->add(normal_brightness);

    // TODO weak
    normal_brightness->on_event([this, normal_brightness](Event&)
    {
        main_screen()->set_brightness(normal_brightness->value());
        settings().set("normal_brightness", std::to_string(normal_brightness->value()));
    }, {eventid::property_changed});
}

template<typename T, typename... Args>
static std::shared_ptr<T> make_setup_button(Args&& ... args)
{
    auto button = make_shared<T>(std::forward<Args>(args)...);
    button->set_align(alignmask::expand);
    button->set_image_align(alignmask::top);
    button->set_color(Palette::ColorId::button_bg, Color::css("#656e75"));
    return button;
}

MenuPage::MenuPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<ImageLabel>("Menu");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });

    auto grid = make_shared<StaticGrid>(std::make_tuple(4, 2));
    grid->set_border(40);
    layout->add(expand(grid));

    auto time = make_setup_button<ImageButton>(Image("schedule.png"), "Schedule");
    grid->add(time);
    time->on_click([this](Event&)
    {
        m_window.push_page("schedule");
    });

    auto sleep_mode = make_setup_button<ImageButton>(Image("sleep.png"), "Sleep Mode");
    grid->add(sleep_mode);
    sleep_mode->on_click([this](Event&)
    {
        m_window.push_page("idlesettings");
    });

    auto screen_brightness = make_setup_button<ImageButton>(Image("brightness.png"), "Screen\nBrightness");
    grid->add(screen_brightness);
    screen_brightness->on_click([this](Event&)
    {
        m_window.push_page("screenbrightness");
    });

    auto home_content = make_setup_button<ImageButton>(Image("home.png"), "Home Screen\nContent");
    grid->add(home_content);
    home_content->on_click([this](Event&)
    {
        m_window.push_page("homecontent");
    });

    auto wifi = make_setup_button<ImageButton>(Image("wifi.png"), "Wi-Fi");
    grid->add(wifi);

    auto sensors = make_setup_button<ImageButton>(Image("sensors.png"), "Sensors");
    grid->add(sensors);
    sensors->on_click([this](Event&)
    {
        m_window.push_page("sensors");
    });

    auto hvac = make_setup_button<ImageButton>(Image("hvac.png"), "HVAC\nEquipment");
    grid->add(hvac);
    hvac->on_click([this](Event&)
    {
        //m_window.push_page("hvac");
    });

    auto about = make_setup_button<ImageButton>(Image("about.png"), "About\nThermostat");
    grid->add(about);
    about->on_click([this](Event&)
    {
        //m_window.push_page("about");
    });
}

HomeContentPage::HomeContentPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<Label>("Home Screen Content");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });

    auto form = make_shared<Form>();
    form->set_margin(50);
    layout->add(expand(form));

    auto indoor_humidity = make_shared<ToggleBox>();
    // TODO: this should probably be default to none
    indoor_humidity->set_boxtype(Theme::boxtype::border_rounded);
    indoor_humidity->set_border(2);
    indoor_humidity->set_toggle_text("Off", "On");
    indoor_humidity->set_enable_disable(false);
    indoor_humidity->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55));
    indoor_humidity->set_color(Palette::ColorId::button_text, Palette::white);
    indoor_humidity->set_color(Palette::ColorId::button_text, Palette::white, Palette::GroupId::disabled);
    form->add_option("Indoor humidity", indoor_humidity);

    auto outdoor_humidity = make_shared<ToggleBox>();
    outdoor_humidity->set_boxtype(Theme::boxtype::border_rounded);
    outdoor_humidity->set_border(2);
    outdoor_humidity->set_toggle_text("Off", "On");
    outdoor_humidity->set_enable_disable(false);
    outdoor_humidity->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55));
    outdoor_humidity->set_color(Palette::ColorId::button_text, Palette::white);
    outdoor_humidity->set_color(Palette::ColorId::button_text, Palette::white, Palette::GroupId::disabled);
    form->add_option("Outdoor humidity", outdoor_humidity);

    m_degrees = make_shared<ToggleBox>();
    m_degrees->set_boxtype(Theme::boxtype::border_rounded);
    m_degrees->set_border(2);
    m_degrees->set_toggle_text("Fahrenheit", "Celsius");
    m_degrees->set_enable_disable(false);
    m_degrees->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55));
    m_degrees->set_color(Palette::ColorId::button_text, Palette::white);
    m_degrees->set_color(Palette::ColorId::button_text, Palette::white, Palette::GroupId::disabled);
    m_degrees->set_checked(settings().get("degrees") == "c");
    form->add_option("Display degrees", m_degrees);

    auto m_time_format = make_shared<ToggleBox>();
    m_time_format->set_boxtype(Theme::boxtype::border_rounded);
    m_time_format->set_border(2);
    m_time_format->set_toggle_text("12 Hour", "24 Hour");
    m_time_format->set_enable_disable(false);
    m_time_format->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55));
    m_time_format->set_color(Palette::ColorId::button_text, Palette::white);
    m_time_format->set_color(Palette::ColorId::button_text, Palette::white, Palette::GroupId::disabled);
    m_time_format->set_checked(settings().get("time_format") == "24");
    form->add_option("Time format", m_time_format);

    m_button_group = detail::make_unique<ButtonGroup>(true, true);
    auto frame = make_shared<Frame>();
    frame->set_border(4);
    frame->set_boxtype(Theme::boxtype::border_bottom);
    auto sizer = make_shared<HorizontalBoxSizer>();
    frame->add(expand(sizer));
    auto btn1 = make_shared<CheckButton>("Fahrenheit");
    btn1->set_checked(true);
    btn1->set_boxtype(Theme::boxtype::blank);
    btn1->set_color(Palette::ColorId::button_bg, Palette::transparent);
    btn1->set_color(Palette::ColorId::button_text, Palette::white);
    btn1->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::checked);
    auto btn2 = make_shared<CheckButton>("Celsius");
    btn2->set_boxtype(Theme::boxtype::blank);
    btn2->set_color(Palette::ColorId::button_bg, Palette::transparent);
    btn2->set_color(Palette::ColorId::button_text, Palette::white);
    btn2->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::checked);
    auto btn3 = make_shared<CheckButton>("None");
    btn3->set_boxtype(Theme::boxtype::blank);
    btn3->set_color(Palette::ColorId::button_bg, Palette::transparent);
    btn3->set_color(Palette::ColorId::button_text, Palette::white);
    btn3->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::checked);
    sizer->add(expand(btn1));
    sizer->add(expand(btn2));
    sizer->add(expand(btn3));

    m_button_group->add(btn1);
    m_button_group->add(btn2);
    m_button_group->add(btn3);

    form->add_option("Display degrees", frame);

#if 0
    auto m_timezone = std::make_shared<ListBox>(Rect(0, 0, 100, 100));
    //m_timezone->set_color(Palette::ColorId::bg, Palette::transparent);
    //m_timezone->hide();
    std::vector<std::string> timezones;
    get_timezones(timezones);
    cout << timezones.size() << " timezones" << endl;
    for (auto& t : timezones)
    {
        auto s = std::make_shared<StringItem>(t);
        s->set_color(Palette::ColorId::label_text, Palette::black);
        m_timezone->add_item(s);
    }
    //m_timezone->show();
    form->add_option("Timezone", m_timezone);
#endif

    std::vector<std::string> timezones;
    get_timezones(timezones);

    auto timezone = std::make_shared<Scrollwheel>(timezones);
    timezone->set_orient(orientation::horizontal);
    timezone->set_image(Image("wheel_down.png"),  Image("wheel_up.png"));
    form->add_option("Timezone", timezone);
}

bool HomeContentPage::leave()
{
    if (m_degrees->checked())
        settings().set("degrees", "c");
    else
        settings().set("degrees", "f");

    //hack
    m_logic.refresh();

    return true;
}

SensorsPage::SensorsPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<Label>("Sensors");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });

    layout->add(make_shared<Label>("Select the sensor to use for internal temperature"));

    m_checkbox_group = detail::make_unique<ButtonGroup>(true, true);

    const auto sensors = enumerate_temp_sensors();
    for (const auto& sensor : sensors)
    {
        auto checkbox = std::make_shared<CheckBox>(sensor);
        if (settings().get("temp_sensor") == sensor)
            checkbox->set_checked(true);
        checkbox->set_align(alignmask::left);
        checkbox->set_margin(5);
        checkbox->set_name(sensor);
        layout->add(checkbox);
        m_checkbox_group->add(checkbox);
    }
}

bool SensorsPage::leave()
{
    m_checkbox_group->foreach_checked([this](Button & button)
    {
        settings().set("temp_sensor", button.name());
    });

    return true;
}

IdlePage::IdlePage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    set_color(Palette::ColorId::bg, Palette::black);

    auto date = make_shared<Label>("", Size(250, 64),
                                   alignmask::center);
    date->set_color(Palette::ColorId::bg, Palette::transparent);
    date->set_align(alignmask::center | alignmask::top);
    date->set_margin(10);
    date->set_name("date_label1");
    add(date);

    auto leftbox = make_shared<BoxSizer>(orientation::vertical);
    leftbox->set_align(alignmask::center | alignmask::left);
    leftbox->set_margin(10);
    add(leftbox);

    auto time = make_shared<Label>();
    time->set_name("time_label1");
    leftbox->add(egt::center(time));

    auto m_otemp = make_shared<ImageLabel>(Image("02d.png"), "Outside 30°");
    m_otemp->set_font(Font(16));
    m_otemp->set_image_align(alignmask::center | alignmask::top);
    leftbox->add(egt::center(m_otemp));

    auto m_wifi = make_shared<ImageLabel>(Image("@32px/wifi.png"));
    m_wifi->set_align(alignmask::right | alignmask::top);
    m_wifi->set_margin(10);
    add(m_wifi);

    auto hsizer = make_shared<BoxSizer>(orientation::vertical);
    hsizer->set_align(alignmask::center);
    add(hsizer);

    m_temp = make_shared<Label>();
    m_temp->set_font(Font(200));
    m_temp->set_margin(10);
    hsizer->add(m_temp);

    m_status = make_shared<Label>();
    m_status->set_font(Font(24));
    hsizer->add(m_status);

    apply_logic_change(m_logic.current_status());
    logic.on_event([this](Event&)
    {
        apply_logic_change(m_logic.current_status());
    }, {eventid::event1});
}

void IdlePage::apply_logic_change(Logic::status status)
{
    m_temp->set_text(format_temp(m_logic.current()));

    switch (status)
    {
    case Logic::status::off:
    {
        //m_status->set_text(Logic::status_str(m_logic.current_status()));
        m_status->set_text("");
        break;
    }
    case Logic::status::cooling:
    {
        m_status->set_text(Logic::status_str(m_logic.current_status()) + " to " + format_temp(m_logic.target()));
        break;
    }
    case Logic::status::heating:
    {
        m_status->set_text(Logic::status_str(m_logic.current_status()) + " to " + format_temp(m_logic.target()));
        break;
    }
    }
}

MainPage::MainPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto menu = make_shared<ImageButton>(Image("menu.png"));
    menu->set_boxtype(Theme::boxtype::none);
    menu->set_padding(0);
    menu->move(Point(0, 20));
    add(menu);
    menu->on_click([this](Event&)
    {
        m_window.push_page("menu");
    });

    auto date = make_shared<Label>("",
                                   alignmask::center);
    date->set_color(Palette::ColorId::bg, Palette::transparent);
    date->set_align(alignmask::center | alignmask::top);
    date->set_margin(10);
    date->set_name("date_label2");
    add(date);

    auto leftbox = make_shared<BoxSizer>(orientation::vertical);
    leftbox->set_align(alignmask::center | alignmask::left);
    leftbox->set_margin(10);
    add(leftbox);

    auto time = make_shared<Label>();
    time->set_name("time_label2");
    leftbox->add(egt::center(time));

    auto m_otemp = make_shared<ImageLabel>(Image("02d.png"), "Outside 30°");
    m_otemp->set_font(Font(16));
    m_otemp->set_image_align(alignmask::center | alignmask::top);
    leftbox->add(egt::center(m_otemp));

    auto hsizer = make_shared<BoxSizer>(orientation::vertical);
    hsizer->set_align(alignmask::center);
    add(hsizer);

    m_temp = make_shared<Label>();
    m_temp->set_font(Font(200));
    m_temp->set_margin(10);
    hsizer->add(m_temp);

    m_status = make_shared<Label>();
    m_status->set_font(Font(24));
    hsizer->add(m_status);

    m_layout = make_shared<VerticalBoxSizer>(justification::justify);
    m_layout->set_align(alignmask::right | alignmask::center);
    m_layout->set_margin(50);
    add(m_layout);

    static auto btn_setup = [](shared_ptr<ImageButton>& button)
    {
        button->set_color(Palette::ColorId::button_bg, Palette::transparent);
        button->set_color(Palette::ColorId::button_bg, Palette::transparent, Palette::GroupId::active);
        button->set_color(Palette::ColorId::border, Palette::transparent);
        button->set_boxtype(Theme::boxtype::none);

        std::weak_ptr<ImageButton> weak_button(button);
        button->on_event([weak_button](Event & event)
        {
            auto button = weak_button.lock();
            if (button)
            {
                switch (event.id())
                {
                case eventid::raw_pointer_down:
                    button->set_alpha(0.5);
                    break;
                case eventid::raw_pointer_up:
                    button->set_alpha(1.0);
                    break;
                default:
                    break;
                }
            }
        });
    };

    auto up = make_shared<ImageButton>(Image("up.png"));
    btn_setup(up);
    m_layout->add(up);
    m_layout->add(make_shared<ImageLabel>(Image("dots.png")));
    auto down = make_shared<ImageButton>(Image("down.png"));
    btn_setup(down);
    m_layout->add(down);

    up->on_click([this](Event&)
    {
        m_logic.change_target(m_logic.target() + 1.);
    });

    down->on_click([this](Event&)
    {
        m_logic.change_target(m_logic.target() - 1.);
    });

    apply_logic_change(m_logic.current_status());
    logic.on_event([this](Event&)
    {
        apply_logic_change(m_logic.current_status());
    }, {eventid::event1});

    auto sizer = make_shared<HorizontalBoxSizer>();
    add(expand_horizontal(bottom(sizer)));

    m_mode = make_shared<ImageButton>(Image("auto.png", 0.3));
    m_mode->set_padding(10);
    m_mode->set_boxtype(Theme::boxtype::blank);
    m_mode->set_image_align(alignmask::center | alignmask::left);
    m_mode->set_color(Palette::ColorId::button_bg, Color(0, 0, 0, 55));
    m_mode->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::active);
    sizer->add(expand(m_mode));

    m_mode->on_click([this](Event&)
    {
        m_window.push_page("mode");
    });

    auto line = make_shared<LineWidget>();
    line->set_color(Palette::ColorId::border, Palette::transparent);
    line->set_horizontal(false);
    line->set_width(2);
    sizer->add(expand_vertical(line));

    m_fan = make_shared<ImageButton>(Image("fan.png", 0.3));
    m_fan->set_padding(10);
    m_fan->set_boxtype(Theme::boxtype::blank);
    m_fan->set_image_align(alignmask::center | alignmask::left);
    m_fan->set_color(Palette::ColorId::button_bg, Color(0, 0, 0, 55));
    m_fan->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55), Palette::GroupId::active);
    sizer->add(expand(m_fan));

    m_fan->on_click([this](Event&)
    {
        m_window.push_page("fan");
    });
}

static string& capitalize(string& s)
{
    if (!s.empty())
        s[0] = toupper(s[0]);
    return s;
}

void MainPage::enter()
{
    m_layout->set_visible(settings().get("mode") != "off");

    auto mode = settings().get("mode");
    m_mode->set_text(capitalize(mode) + string(" Mode"));
    auto fan = settings().get("fan");
    m_fan->set_text(string("Fan ") + capitalize(fan));
}

void MainPage::apply_logic_change(Logic::status status)
{
    m_temp->set_text(format_temp(m_logic.current()));

    switch (status)
    {
    case Logic::status::off:
    {
        set_color(Palette::ColorId::bg, Palette::gray);
        m_status->set_text(Logic::status_str(m_logic.current_status()));
        break;
    }
    case Logic::status::cooling:
    {
        set_color(Palette::ColorId::bg, Color::css("#0289cd"));
        m_status->set_text(Logic::status_str(m_logic.current_status()) + " to " + format_temp(m_logic.target()));
        break;
    }
    case Logic::status::heating:
    {
        set_color(Palette::ColorId::bg, Color::css("#f76707"));
        m_status->set_text(Logic::status_str(m_logic.current_status()) + " to " + format_temp(m_logic.target()));
        break;
    }
    }
}

SchedulePage::SchedulePage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    auto layout = make_shared<VerticalBoxSizer>(justification::start);
    add(expand(layout));

    auto title = make_shared<Frame>();
    title->set_boxtype(Theme::boxtype::blank);
    title->set_color(Palette::ColorId::bg, Color::css("#3b4248"));
    title->set_height(50);
    layout->add(expand_horizontal(title));

    auto title_menu = make_shared<Label>("Schedule");
    title_menu->set_font(Font(title_font_size));
    // TODO: clipping on end of text if we just center here
    title->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("back.png"));
    title_back->set_boxtype(Theme::boxtype::none);
    title->add(left(egt::center(title_back)));

    title_back->on_click([this](Event&)
    {
        m_window.pop_page();
    });


    auto form = make_shared<Form>();
    form->set_height(50);
    form->set_boxtype(Theme::boxtype::fill);
    form->set_color(Palette::ColorId::bg, Palette::gray);
    layout->add(expand_horizontal(form));

    auto enabled = make_shared<ToggleBox>();
    // TODO: this should probably be default to none
    enabled->set_boxtype(Theme::boxtype::border_rounded);
    enabled->set_border(2);
    enabled->set_margin(2);
    enabled->set_toggle_text("Off", "On");
    enabled->set_enable_disable(true);
    enabled->set_color(Palette::ColorId::button_bg, Color(Palette::cyan, 55));
    enabled->set_color(Palette::ColorId::button_text, Palette::white);
    enabled->set_color(Palette::ColorId::button_text, Palette::white, Palette::GroupId::disabled);
    form->set_name_align(alignmask::center);
    form->add_option("Thermostat Schedule", enabled);

    auto grid = make_shared<StaticGrid>(std::make_tuple(3, 4));
    layout->add(expand(grid));

    vector<string> times;
    for (auto m = 0; m < 24 * 60; m += 15)
    {
        ostringstream ss;
        if (((m / 60) % 12) < 1)
            ss << "12";
        else
            ss << std::to_string((m / 60) % 12);
        ss << std::string(":");
        ss << std::setfill('0') << std::setw(2) << std::to_string(m % 60);
        if (m < (12 * 60))
            ss << std::string(" AM");
        else
            ss << std::string(" PM");
        times.push_back(ss.str());
    }

    vector<string> temps;
    for (auto m = 0; m < 100; m++)
        temps.push_back(std::to_string(m) + "°");

    vector<string> names = { "Wake", "Leave", "Return", "Sleep"};

    for (auto& name : names)
    {
        grid->add(expand(make_shared<ImageLabel>(Image("setting.png"), name)));
        auto time1 = std::make_shared<Scrollwheel>(times);
        time1->set_orient(orientation::horizontal);
        time1->set_image(Image("wheel_down.png"),  Image("wheel_up.png"));
        grid->add(expand(time1));

        auto temp1 = std::make_shared<Scrollwheel>(temps);
        temp1->set_orient(orientation::horizontal);
        temp1->set_image(Image("wheel_down.png"),  Image("wheel_up.png"));
        grid->add(expand(temp1));
    }
}

bool SchedulePage::leave()
{
    return true;
}
