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
#include <iomanip>
#include <iostream>
#include <libintl.h>

using namespace egt;
using namespace egt::experimental;
using namespace std;

/**
 * This is a wrapper around gettext().
 */
#define _(String) gettext(String)

template<class T>
constexpr static inline T f2c(T f)
{
    return f * 5. / 9.;
}

static inline std::string format_temp(float temp)
{
    ostringstream ss;
    if (settings().get("degrees") == "f")
        ss << std::round(CtoF(temp)) << "°";
    else
        ss << std::round(temp) << "°";
    return ss.str();
}

static void selectable_btn_setup(const shared_ptr<ImageButton>& button)
{
    button->color(Palette::ColorId::button_bg, Palette::transparent);
    button->color(Palette::ColorId::button_bg, Color(0, 255, 255, 30), Palette::GroupId::checked);
    button->color(Palette::ColorId::button_bg, Palette::transparent, Palette::GroupId::active);
    button->color(Palette::ColorId::border, Palette::transparent);
    button->fill_flags(Theme::FillFlag::blend);
    button->border(2);
    button->margin(5);
    button->padding(10);
    button->image_align(AlignFlag::center | AlignFlag::top);

    std::weak_ptr<ImageButton> weak_button(button);
    button->on_click([weak_button](Event&)
    {
        auto button = weak_button.lock();
        if (button)
            button->checked(true);
    });
}

IdlePage::IdlePage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    color(Palette::ColorId::bg, Palette::black);

    auto date = make_shared<Label>("",
                                   AlignFlag::center);
    date->color(Palette::ColorId::bg, Palette::transparent);
    date->align(AlignFlag::center | AlignFlag::top);
    date->margin(10);
    date->name("date_label1");
    add(date);

    auto leftbox = make_shared<BoxSizer>(Orientation::vertical);
    leftbox->align(AlignFlag::center | AlignFlag::left);
    leftbox->margin(10);
    add(leftbox);

    auto time = make_shared<Label>();
    time->name("time_label1");
    leftbox->add(egt::center(time));

    m_otemp = make_shared<ImageLabel>(Image("file:02d.png"), "Outside");
    m_otemp->font(Font(16));
    m_otemp->image_align(AlignFlag::center | AlignFlag::top);
    leftbox->add(egt::center(m_otemp));

    auto logo = std::make_shared<ImageButton>(Image("icon:egt_logo_white.png;128"));
    logo->fill_flags().clear();
    logo->align(AlignFlag::center | AlignFlag::bottom);
    logo->margin(10);
    add(logo);

    auto hsizer = make_shared<BoxSizer>(Orientation::vertical);
    hsizer->align(AlignFlag::center | AlignFlag::expand_horizontal);
    add(hsizer);

    m_temp = make_shared<Label>();
    m_temp->font(Font(200));
    m_temp->margin(10);
    hsizer->add(m_temp);

    m_status = make_shared<Label>();
    m_status->font(Font(24));
    hsizer->add(m_status);

    apply_logic_change(m_logic.current_status());
    logic.on_change([this]()
    {
        apply_logic_change(m_logic.current_status());
    });
}

void IdlePage::enter()
{
    if (settings().get("outside") == "on")
    {
        m_otemp->text(std::string(_("Outside")) + " " + format_temp(30));
        m_otemp->show();
    }
    else
    {
        m_otemp->hide();
    }
}

void IdlePage::apply_logic_change(Logic::status status)
{
    m_temp->text(format_temp(m_logic.current()));

    switch (status)
    {
    case Logic::status::off:
    {
        if (m_logic.get_mode() == Logic::mode::off)
            m_status->text(Logic::status_str(m_logic.current_status()));
        else
            m_status->text(Logic::status_str(m_logic.current_status()) + " at " + format_temp(m_logic.target()));
        break;
    }
    case Logic::status::cooling:
    case Logic::status::heating:
    {
        m_status->text(Logic::status_str(m_logic.current_status()) + " to " + format_temp(m_logic.target()));
        break;
    }
    }
}

class TempLabel : public Label
{
    using Label::Label;

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        detail::ignoreparam(rect);

        draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

        auto color = this->color(Palette::ColorId::label_text).color();
        auto shadow = color.shade(0.5);
        shadow.alphaf(0.3);
        painter.set(shadow);

        detail::draw_text(painter,
                          content_area() + Point(4, 4),
                          text(),
                          font(),
                          {TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap},
                          text_align(),
                          Justification::middle,
                          shadow);

        detail::draw_text(painter,
                          content_area(),
                          text(),
                          font(),
                          {TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap},
                          text_align(),
                          Justification::middle,
                          color);
    }
};

MainPage::MainPage(ThermostatWindow& window, Logic& logic)
    : ThermostatPage(window, logic)
{
    m_menu = make_shared<ImageButton>(Image("file:menu.png"));
    m_menu->fill_flags().clear();
    m_menu->padding(0);
    m_menu->move(Point(0, 20));
    add(m_menu);
    m_menu->on_click([this](Event&)
    {
        m_window.push_page("menu");
    });

    auto date = make_shared<Label>("",
                                   AlignFlag::center);
    date->color(Palette::ColorId::bg, Palette::transparent);
    date->align(AlignFlag::center | AlignFlag::top);
    date->margin(10);
    date->name("date_label2");
    add(date);

    auto leftbox = make_shared<BoxSizer>(Orientation::vertical);
    leftbox->align(AlignFlag::center | AlignFlag::left);
    leftbox->margin(10);
    add(leftbox);

    auto time = make_shared<Label>();
    time->name("time_label2");
    leftbox->add(egt::center(time));

    m_otemp = make_shared<ImageLabel>(Image("file:02d.png"), "Outside");
    m_otemp->font(Font(16));
    m_otemp->image_align(AlignFlag::center | AlignFlag::top);
    leftbox->add(egt::center(m_otemp));

    auto hsizer = make_shared<BoxSizer>(Orientation::vertical);
    hsizer->align(AlignFlag::center | AlignFlag::expand_horizontal);
    add(hsizer);

    m_temp = make_shared<TempLabel>();
    m_temp->font(Font(200));
    m_temp->margin(10);
    hsizer->add(m_temp);

    m_status = make_shared<Label>();
    m_status->font(Font(24));
    hsizer->add(m_status);

    m_layout = make_shared<VerticalBoxSizer>(Justification::justify);
    m_layout->align(AlignFlag::right | AlignFlag::center);
    m_layout->margin(50);
    add(m_layout);

    static auto btn_setup = [](shared_ptr<ImageButton>& button)
    {
        button->color(Palette::ColorId::button_bg, Palette::transparent);
        button->color(Palette::ColorId::button_bg, Palette::transparent, Palette::GroupId::active);
        button->color(Palette::ColorId::border, Palette::transparent);
        button->fill_flags().clear();
    };

    auto up = make_shared<ImageButton>(Image("file:up.png"));
    btn_setup(up);
    m_layout->add(up);
    m_layout->add(make_shared<ImageLabel>(Image("file:dots.png")));
    auto down = make_shared<ImageButton>(Image("file:down.png"));
    btn_setup(down);
    m_layout->add(down);

    std::weak_ptr<ImageButton> weak_up(up);
    up->on_event([this, weak_up](Event & event)
    {
        auto up = weak_up.lock();
        if (up)
        {
            switch (event.id())
            {
            case EventId::raw_pointer_down:
                up->image(Image("file:up2.png"));
                break;
            case EventId::raw_pointer_up:
                up->image(Image("file:up.png"));
                break;
            case EventId::pointer_click:
            case EventId::pointer_hold:
                if (settings().get("degrees") == "f")
                    m_logic.change_target(m_logic.target() + f2c<float>(1.));
                else
                    m_logic.change_target(m_logic.target() + 1.);
                break;
            default:
                break;
            }
        }
    });

    std::weak_ptr<ImageButton> weak_down(down);
    down->on_event([this, weak_down](Event & event)
    {
        auto down = weak_down.lock();
        if (down)
        {
            switch (event.id())
            {
            case EventId::raw_pointer_down:
                down->image(Image("file:down2.png"));
                break;
            case EventId::raw_pointer_up:
                down->image(Image("file:down.png"));
                break;
            case EventId::pointer_click:
            case EventId::pointer_hold:
                if (settings().get("degrees") == "f")
                    m_logic.change_target(m_logic.target() - f2c<float>(1.));
                else
                    m_logic.change_target(m_logic.target() - 1.);
                break;
            default:
                break;
            }
        }
    });

    apply_logic_change(m_logic.current_status());
    logic.on_change([this]()
    {
        apply_logic_change(m_logic.current_status());
    });

    auto sizer = make_shared<HorizontalBoxSizer>();
    add(expand_horizontal(bottom(sizer)));

    m_mode = make_shared<ImageButton>();
    m_mode->padding(10);
    m_mode->fill_flags(Theme::FillFlag::blend);
    m_mode->image_align(AlignFlag::center | AlignFlag::left);
    m_mode->color(Palette::ColorId::button_bg, Color(255, 255, 255, 75));
    m_mode->color(Palette::ColorId::button_bg, Color(Palette::cyan, 75), Palette::GroupId::active);
    sizer->add(expand(m_mode));

    m_mode->on_click([this](Event&)
    {
        m_window.push_page("mode");
    });

    auto line = make_shared<LineWidget>();
    line->color(Palette::ColorId::border, Palette::transparent);
    line->horizontal(false);
    line->width(2);
    sizer->add(expand_vertical(line));

    m_fan = make_shared<ImageButton>();
    m_fan->padding(10);
    m_fan->fill_flags(Theme::FillFlag::blend);
    m_fan->image_align(AlignFlag::center | AlignFlag::left);
    m_fan->color(Palette::ColorId::button_bg, Color(255, 255, 255, 75));
    m_fan->color(Palette::ColorId::button_bg, Color(Palette::cyan, 75), Palette::GroupId::active);
    sizer->add(expand(m_fan));

    m_fan->on_click([this](Event&)
    {
        m_window.push_page("fan");
    });

#ifdef EGT_HAS_CAMERA
    m_camera = make_shared<CameraWindow>(Size(320, 240));
    shrink_camera();
    add(m_camera);

    m_camera->on_click([this]()
    {
        if (!m_camera_fullscreen)
        {
            m_camera->move(Point(0, 0));
            m_camera->scale(2.5, 2.5);
            m_camera_fullscreen = true;
        }
        else
        {
            shrink_camera();
        }
    });

    m_camera->on_error([this](const std::string&)
    {
        cout << "camera error, hiding" << endl;
        m_camera->hide();
    });
#endif

    m_background_timer.on_timeout([this]()
    {
        m_window.background(Image("file:background" + std::to_string(m_background_index) + ".png"));
        if (++m_background_index > 5)
            m_background_index = 0;
    });
}

static inline std::string capitalize(const std::string& s)
{
    std::string result = s;
    if (!result.empty())
        result[0] = toupper(result[0]);
    return result;
}

void MainPage::shrink_camera()
{
    if (m_camera_fullscreen)
    {
#ifdef EGT_HAS_CAMERA
	m_camera->scale(.5, .5);
	m_camera->move(Point(m_menu->width() + 10, 10));
#endif
        m_camera_fullscreen = false;
    }
}

void MainPage::enter()
{
    if (settings().get("background") == "on")
    {
        m_window.background(Image("file:background" + std::to_string(m_background_index) + ".png"));
        fill_flags().clear();
        m_background_timer.start();
    }
    else
    {
        m_background_timer.stop();
        m_window.background(Image());
        fill_flags(Theme::FillFlag::blend);
    }

    if (settings().get("outside") == "on")
    {
        m_otemp->text(std::string(_("Outside")) + " " + format_temp(30));
        m_otemp->show();
    }
    else
    {
        m_otemp->hide();
    }

    m_layout->visible(settings().get("mode") != "off");

    auto mode = settings().get("mode");
    m_mode->text(string("System ") + capitalize(mode));
    m_mode->image(Image("file:" + mode + ".png", 0.3));
    auto fan = settings().get("fan");
    m_fan->text(string("Fan ") + capitalize(fan));
    m_fan->image(Image("file:fan_" + fan + ".png", 0.3));

#ifdef EGT_HAS_CAMERA
    if (m_camera->start())
        m_camera->show();
#endif

    // hack
    m_logic.refresh();
}

bool MainPage::leave()
{
#ifdef EGT_HAS_CAMERA
    shrink_camera();
    m_camera->stop();
    m_camera->hide();
#endif
    return true;
}

void MainPage::apply_logic_change(Logic::status status)
{
    m_temp->text(format_temp(m_logic.current()));

    switch (status)
    {
    case Logic::status::off:
    {
        color(Palette::ColorId::bg, Palette::gray);
        if (m_logic.get_mode() == Logic::mode::off)
            m_status->text(Logic::status_str(m_logic.current_status()));
        else
            m_status->text(Logic::status_str(m_logic.current_status()) + " at " + format_temp(m_logic.target()));
        break;
        break;
    }
    case Logic::status::cooling:
    {
        color(Palette::ColorId::bg, Color::css("#0289cd"));
        m_status->text(Logic::status_str(m_logic.current_status()) + " to " + format_temp(m_logic.target()));
        break;
    }
    case Logic::status::heating:
    {
        color(Palette::ColorId::bg, Color::css("#f76707"));
        m_status->text(Logic::status_str(m_logic.current_status()) + " to " + format_temp(m_logic.target()));
        break;
    }
    }
}

std::shared_ptr<VerticalBoxSizer> SettingsPage::create_layout(const std::string& title)
{
    static const auto TITLE_FONT_SIZE = 24;

    auto layout = make_shared<VerticalBoxSizer>(Justification::start);
    add(expand(layout));

    auto title_frame = make_shared<Frame>();
    title_frame->fill_flags(Theme::FillFlag::blend);
    title_frame->color(Palette::ColorId::bg, Color(55, 71, 79));
    title_frame->height(50);
    layout->add(expand_horizontal(title_frame));

    auto title_menu = make_shared<Label>(title);
    title_menu->font(Font(TITLE_FONT_SIZE));
    title_frame->add(egt::expand_horizontal(egt::center(title_menu)));

    auto title_back = make_shared<ImageButton>(Image("file:back.png"));
    title_back->fill_flags().clear();
    title_frame->add(left(egt::center(title_back)));
    title_back->on_click([this](Event&) { m_window.pop_page(); });

    return layout;
}

ModePage::ModePage(ThermostatWindow& window, Logic& logic)
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("System Mode"));

    auto sizer = make_shared<HorizontalBoxSizer>();
    layout->add(expand(sizer));

    auto mode_auto = make_shared<ImageButton>(Image("file:auto.png"), _("Automatic"));
    mode_auto->name("auto");
    mode_auto->checked(settings().get("mode") == "auto");
    selectable_btn_setup(mode_auto);
    sizer->add(mode_auto);

    auto mode_heating = make_shared<ImageButton>(Image("file:heat.png"), _("Heating"));
    mode_heating->name("heat");
    mode_heating->checked(settings().get("mode") == "heat");
    selectable_btn_setup(mode_heating);
    sizer->add(mode_heating);

    auto mode_cooling = make_shared<ImageButton>(Image("file:cool.png"), _("Cooling"));
    mode_cooling->name("cool");
    mode_cooling->checked(settings().get("mode") == "cool");
    selectable_btn_setup(mode_cooling);
    sizer->add(mode_cooling);

    auto mode_off = make_shared<ImageButton>(Image("file:off.png"), _("Off"));
    mode_off->name("off");
    mode_off->checked(settings().get("mode") == "off");
    selectable_btn_setup(mode_off);
    sizer->add(mode_off);

    m_button_group = std::make_unique<ButtonGroup>(true, true);
    m_button_group->add(mode_auto);
    m_button_group->add(mode_heating);
    m_button_group->add(mode_cooling);
    m_button_group->add(mode_off);
}

bool ModePage::leave()
{
    Settings::AutoTransaction tx(settings());

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
    : SettingsPage(window, logic)
{
    auto layout = create_layout("Fan Mode");

    auto sizer = make_shared<HorizontalBoxSizer>();
    layout->add(expand(sizer));

    auto mode_auto = make_shared<ImageButton>(Image("file:fan_auto.png"), _("Auto"));
    mode_auto->name("auto");
    selectable_btn_setup(mode_auto);
    mode_auto->checked(settings().get("fan") == "auto");
    sizer->add(mode_auto);

    auto mode_off = make_shared<ImageButton>(Image("file:fan_on.png"), _("On"));
    mode_off->name("on");
    selectable_btn_setup(mode_off);
    mode_auto->checked(settings().get("fan") == "on");
    sizer->add(mode_off);

    m_button_group = std::make_unique<ButtonGroup>(true, true);
    m_button_group->add(mode_auto);
    m_button_group->add(mode_off);
}

bool FanPage::leave()
{
    Settings::AutoTransaction tx(settings());

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
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("Idle Mode"));

    auto sizer = make_shared<VerticalBoxSizer>();
    sizer->margin(50);
    layout->add(expand(sizer));

    sizer->add(make_shared<Label>(_("Seconds idle before entering idle mode")));

    m_idle_timeout = std::make_shared<Slider>(10, 120,
                     std::stoi(settings().get("sleep_timeout")));
    m_idle_timeout->height(100);
    m_idle_timeout->align(AlignFlag::expand_horizontal);
    m_idle_timeout->slider_flags().set({Slider::SliderFlag::round_handle, Slider::SliderFlag::show_label});
    sizer->add(m_idle_timeout);

    sizer->add(make_shared<Label>("Sleep screen brightness"));

    m_sleep_brightness = std::make_shared<Slider>(3,
                         Application::instance().screen()->max_brightness(),
                         std::stoi(settings().get("sleep_brightness")));
    m_sleep_brightness->height(100);
    m_sleep_brightness->align(AlignFlag::expand_horizontal);
    m_sleep_brightness->slider_flags().set({Slider::SliderFlag::round_handle, Slider::SliderFlag::show_label});
    sizer->add(m_sleep_brightness);
}

bool IdleSettingsPage::leave()
{
    Settings::AutoTransaction tx(settings());

    settings().set("sleep_brightness", std::to_string(m_sleep_brightness->value()));
    settings().set("sleep_timeout", std::to_string(m_idle_timeout->value()));

    m_window.m_idle_timer.change_duration(
        std::chrono::seconds(std::stoi(settings().get("sleep_timeout"))));

    return true;
}

ScreenBrightnessPage::ScreenBrightnessPage(ThermostatWindow& window, Logic& logic)
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("Screen Brightness"));

    auto sizer = make_shared<VerticalBoxSizer>();
    sizer->margin(50);
    layout->add(expand(sizer));

    sizer->add(make_shared<Label>(_("Screen brightness")));

    auto normal_brightness = std::make_shared<Slider>(3,
                             Application::instance().screen()->max_brightness(),
                             std::stoi(settings().get("normal_brightness")));
    normal_brightness->height(100);
    normal_brightness->align(AlignFlag::expand_horizontal);
    normal_brightness->slider_flags().set({Slider::SliderFlag::round_handle, Slider::SliderFlag::show_label});
    sizer->add(normal_brightness);

    std::weak_ptr<Slider> weak_normal_brightness(normal_brightness);
    normal_brightness->on_value_changed([weak_normal_brightness]()
    {
        auto normal_brightness = weak_normal_brightness.lock();
        if (normal_brightness)
        {
            Application::instance().screen()->brightness(normal_brightness->value());
            settings().set("normal_brightness", std::to_string(normal_brightness->value()));
        }
    });
}

template<typename T, typename... Args>
static std::shared_ptr<T> make_setup_button(Args&& ... args)
{
    auto button = make_shared<T>(std::forward<Args>(args)...);
    button->align(AlignFlag::expand);
    button->image_align(AlignFlag::top);
    button->color(Palette::ColorId::button_bg, Color(59, 70, 76));
    return button;
}

MenuPage::MenuPage(ThermostatWindow& window, Logic& logic)
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("Menu"));

    auto grid = make_shared<StaticGrid>(StaticGrid::GridSize(4, 2));
    grid->border(40);
    layout->add(expand(grid));

    auto time = make_setup_button<ImageButton>(Image("file:schedule.png"), _("Schedule"));
    grid->add(time);
    time->on_click([this](Event&)
    {
        m_window.push_page("schedule");
    });

    auto sleep_mode = make_setup_button<ImageButton>(Image("file:sleep.png"), _("Idle Mode"));
    grid->add(sleep_mode);
    sleep_mode->on_click([this](Event&)
    {
        m_window.push_page("idlesettings");
    });

    auto screen_brightness = make_setup_button<ImageButton>(Image("file:brightness.png"), _("Screen\nBrightness"));
    grid->add(screen_brightness);
    screen_brightness->on_click([this](Event&)
    {
        m_window.push_page("screenbrightness");
    });

    auto home_content = make_setup_button<ImageButton>(Image("file:home.png"), _("Home Screen\nContent"));
    grid->add(home_content);
    home_content->on_click([this](Event&)
    {
        m_window.push_page("homecontent");
    });

    auto sensors = make_setup_button<ImageButton>(Image("file:sensors.png"), _("Sensors"));
    grid->add(sensors);
    sensors->on_click([this](Event&)
    {
        m_window.push_page("sensors");
    });

    auto wifi = make_setup_button<ImageButton>(Image("file:wifi.png"), _("Wi-Fi"));
    wifi->disabled(true);
    grid->add(wifi);

    auto hvac = make_setup_button<ImageButton>(Image("file:hvac.png"), _("HVAC\nEquipment"));
    hvac->disabled(true);
    grid->add(hvac);

    auto about = make_setup_button<ImageButton>(Image("file:about.png"), _("About\nThermostat"));
    grid->add(about);
    about->on_click([this](Event&)
    {
        m_window.push_page("about");
    });
}

HomeContentPage::HomeContentPage(ThermostatWindow& window, Logic& logic)
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("Home Screen Content"));

    auto form = make_shared<Form>();
    form->margin(50);
    layout->add(expand(form));

    m_showoutside = make_shared<ToggleBox>();
    m_showoutside->border_radius(4.0);
    m_showoutside->toggle_text(_("Off"), _("On"));
    m_showoutside->enable_disable(false);
    m_showoutside->checked(settings().get("outside") == "on");
    form->add_option(_("Outside temp"), m_showoutside);

    m_degrees = make_shared<ToggleBox>();
    m_degrees->border_radius(4.0);
    m_degrees->toggle_text(_("Fahrenheit"), _("Celsius"));
    m_degrees->enable_disable(false);
    m_degrees->checked(settings().get("degrees") == "c");
    form->add_option(_("Display degrees"), m_degrees);

    m_usebackground = make_shared<ToggleBox>();
    m_usebackground->border_radius(4.0);
    m_usebackground->toggle_text(_("Off"), _("On"));
    m_usebackground->enable_disable(false);
    m_usebackground->checked(settings().get("background") == "on");
    form->add_option(_("Background Image"), m_usebackground);

    m_time_format = make_shared<ToggleBox>();
    m_time_format->border_radius(4.0);
    m_time_format->toggle_text(_("12 Hour"), _("24 Hour"));
    m_time_format->enable_disable(false);
    m_time_format->checked(settings().get("time_format") == "24");
    form->add_option(_("Time format"), m_time_format);

#if 0
    std::vector<std::string> timezones;
    get_timezones(timezones);

    if (!timezones.empty())
    {
        auto timezone = std::make_shared<Scrollwheel>(timezones);
        timezone->orient(Orientation::horizontal);
        timezone->image(Image("file:wheel_down.png"),  Image("file:wheel_up.png"));
        form->add_option(_("Timezone"), timezone);
    }
#endif
}

bool HomeContentPage::leave()
{
    Settings::AutoTransaction tx(settings());

    if (m_degrees->checked())
        settings().set("degrees", "c");
    else
        settings().set("degrees", "f");

    if (m_usebackground->checked())
        settings().set("background", "on");
    else
        settings().set("background", "off");

    if (m_showoutside->checked())
        settings().set("outside", "on");
    else
        settings().set("outside", "off");

    if (m_time_format->checked())
        settings().set("time_format", "24");
    else
        settings().set("time_format", "12");

    // hack
    m_logic.refresh();

    return true;
}

SensorsPage::SensorsPage(ThermostatWindow& window, Logic& logic)
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("Sensors"));

    layout->add(make_shared<Label>(_("Select the sensor to use for internal temperature")));

    m_checkbox_group = std::make_unique<ButtonGroup>(true, true);

    const auto sensors = enumerate_temp_sensors();
    for (const auto& sensor : sensors)
    {
        auto checkbox = std::make_shared<CheckBox>(sensor);
        if (settings().get("temp_sensor") == sensor)
            checkbox->checked(true);
        checkbox->align(AlignFlag::left);
        checkbox->margin(5);
        checkbox->name(sensor);
        layout->add(checkbox);
        m_checkbox_group->add(checkbox);
    }
}

bool SensorsPage::leave()
{
    m_checkbox_group->foreach_checked([](Button & button)
    {
        settings().set("temp_sensor", button.name());
    });

    return true;
}

static inline std::string lowercase(const std::string& str)
{
    std::string result;

    result.resize(str.size());

    std::transform(str.begin(),
                   str.end(),
                   result.begin(),
                   ::tolower);

    return result;
}

SchedulePage::SchedulePage(ThermostatWindow& window, Logic& logic)
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("Schedule"));

    auto form = make_shared<Form>();
    form->height(50);
    form->fill_flags(Theme::FillFlag::blend);
    form->color(Palette::ColorId::bg, Palette::gray);
    layout->add(expand_horizontal(form));

    m_enabled = make_shared<ToggleBox>();
    m_enabled->border_radius(4.0);
    m_enabled->margin(2);
    m_enabled->toggle_text(_("Off"), _("On"));
    m_enabled->enable_disable(false);
    m_enabled->checked(settings().get("schedule_enabled") == "on");
    form->name_align(AlignFlag::center);
    form->add_option(_("Schedule Enabled"), m_enabled);

    auto grid = make_shared<StaticGrid>(StaticGrid::GridSize(3, 4));
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

    const auto names = { _("Wake"), _("Leave"), _("Return"), _("Sleep") };

    std::weak_ptr<ToggleBox> weak_enabled(m_enabled);

    for (auto& name : names)
    {
        grid->add(expand(make_shared<ImageLabel>(Image("file:" + lowercase(name) + ".png"), name)));
        auto time1 = std::make_shared<Scrollwheel>(times);
        time1->orient(Orientation::horizontal);
        time1->image_down(Image("file:wheel_down.png"));
        time1->image_up(Image("file:wheel_up.png"));
        grid->add(expand(time1));

        std::weak_ptr<Scrollwheel> weak_time1(time1);
        auto time_invoke = [weak_time1, weak_enabled]()
        {
            auto time1 = weak_time1.lock();
            auto enabled = weak_enabled.lock();
            if (time1 && enabled)
                time1->disabled(!enabled->checked());
        };
        time_invoke();
        m_enabled->on_checked_changed([time_invoke]() { time_invoke(); });

        auto temp1 = std::make_shared<Scrollwheel>(temps);
        temp1->orient(Orientation::horizontal);
        temp1->image_down(Image("file:wheel_down.png"));
        temp1->image_up(Image("file:wheel_up.png"));
        grid->add(expand(temp1));

        std::weak_ptr<Scrollwheel> weak_temp1(temp1);
        auto temp_invoke = [weak_temp1, weak_enabled]()
        {
            auto temp1 = weak_temp1.lock();
            auto enabled = weak_enabled.lock();
            if (temp1 && enabled)
                temp1->disabled(!enabled->checked());
        };
        temp_invoke();
        m_enabled->on_checked_changed([temp_invoke]() { temp_invoke(); });
    }

    std::weak_ptr<StaticGrid> weak_grid(grid);
    auto grid_invoke = [weak_grid, weak_enabled]()
    {
        auto grid = weak_grid.lock();
        auto enabled = weak_enabled.lock();
        if (grid && enabled)
        {
            if (enabled->checked())
                grid->alpha(1.0);
            else
                grid->alpha(0.3);
        }
    };
    grid_invoke();
    m_enabled->on_checked_changed([grid_invoke]()
    {
        grid_invoke();
    });
}

bool SchedulePage::leave()
{
    Settings::AutoTransaction tx(settings());

    if (m_enabled->checked())
        settings().set("schedule_enabled", "on");
    else
        settings().set("schedule_enabled", "off");

    return true;
}

AboutPage::AboutPage(ThermostatWindow& window, Logic& logic)
    : SettingsPage(window, logic)
{
    auto layout = create_layout(_("About"));

    auto sizer = make_shared<HorizontalBoxSizer>();
    layout->add(egt::center(sizer));

    auto logo = std::make_shared<ImageButton>(Image("icon:egt_logo_white.png;128"));
    logo->fill_flags().clear();
    logo->margin(10);
    sizer->add(logo);

    auto mlogo = std::make_shared<ImageButton>(Image("icon:microchip_logo_white.png;128"));
    mlogo->fill_flags().clear();
    mlogo->margin(10);
    sizer->add(mlogo);

    // TODO: i18n support
    auto text = std::make_shared<TextBox>(
                    "This is a thermostat application demonstrating use of the "
                    "Ensemble Graphics Toolkit (EGT) by Microchip.\n\n"
                    "Features:\n"
                    " - SQLite database for all settings and temp logging.\n"
                    " - lm-sensors support for enumerating and using any sensor.\n"
                    " - Live camera feed with full screen scaling.\n"
                    " - Configure Fahrenheit or Celsius display.\n"
                    " - Configurable idle screen.\n"
                    " - Configure screen brightness in and out of idle mode.\n"
                    " - Configurable home screen including background image.\n"
                    "\n"
                    "Source code available at: https://github.com/linux4sam/egt-thermostat\n",
                    TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}));
    text->readonly(true);
    text->margin(20);
    text->border(0);
    text->color(Palette::ColorId::text, Palette::white);
    text->color(Palette::ColorId::bg, Palette::transparent);

    layout->add(expand(text));
}
