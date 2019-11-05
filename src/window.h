/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef WINDOW_H
#define WINDOW_H

#include "logic.h"
#include <egt/ui>
#include <map>
#include <memory>
#include <string>
#include <deque>

class ThermostatWindow : public egt::TopWindow
{
public:

    ThermostatWindow();

    void idle();

    void goto_page(const std::string& name);

    void push_page(const std::string& name);

    void pop_page();

    int page_to_idx(const std::string& name);

    std::shared_ptr<egt::Notebook> notebook;
    std::map<std::string, std::shared_ptr<egt::NotebookTab>> m_pages;
    Logic m_logic;
    std::deque<std::string> m_queue;
    egt::Timer m_screen_brightness_timer{std::chrono::seconds(5)};
    egt::PeriodicTimer m_idle_timer;
    egt::detail::Object::handle_t m_handle{0};

    virtual ~ThermostatWindow();
};

#endif
