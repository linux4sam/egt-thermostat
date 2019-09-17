/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "window.h"
#include "pages.h"

using namespace egt;
using namespace std;

ThermostatWindow::ThermostatWindow()
{
    auto hsizer = make_shared<BoxSizer>(orientation::horizontal);
    add(expand(hsizer));

    notebook = make_shared<Notebook>();

    m_pages["idle"] = make_shared<IdlePage>(*this, m_logic);
    m_pages["main"] = make_shared<MainPage>(*this, m_logic);
    m_pages["menu"] = make_shared<MenuPage>(*this, m_logic);
    m_pages["mode"] = make_shared<ModePage>(*this, m_logic);
    m_pages["homecontent"] = make_shared<HomeContentPage>(*this, m_logic);
    m_pages["idlesettings"] = make_shared<IdleSettingsPage>(*this, m_logic);
    m_pages["fan"] = make_shared<FanPage>(*this, m_logic);
    m_pages["screenbrightness"] = make_shared<ScreenBrightnessPage>(*this, m_logic);
    m_pages["sensors"] = make_shared<SensorsPage>(*this, m_logic);
    m_pages["schedule"] = make_shared<SchedulePage>(*this, m_logic);

    for (auto& i : m_pages)
        notebook->add(i.second);

    hsizer->add(expand(notebook));

    auto idle = m_pages["idle"];
    idle->on_event([this, idle](Event&)
    {
        goto_page("main");
    }, {eventid::raw_pointer_down});

    goto_page("main");
}

void ThermostatWindow::idle()
{
    m_queue.clear();
    notebook->set_selected(page_to_idx("idle"));
}

void ThermostatWindow::goto_page(const std::string& name)
{
    m_queue.clear();
    m_queue.push_back(name);
    notebook->set_selected(page_to_idx(name));
}

void ThermostatWindow::push_page(const std::string& name)
{
    m_queue.push_back(name);
    notebook->set_selected(page_to_idx(name));
}

void ThermostatWindow::pop_page()
{
    assert(!m_queue.empty());
    m_queue.pop_back();
    auto prev = m_queue.back();
    notebook->set_selected(page_to_idx(prev));
}

int ThermostatWindow::page_to_idx(const std::string& name)
{
    return std::distance(m_pages.begin(), m_pages.find(name));
}
