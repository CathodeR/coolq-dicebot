#pragma once

#include "../common.h"
#include "../data/profile.h"
#include "./entry_base.h"
namespace dicebot::entry {
    class entry_set_roll : public entry_base {
        std::regex filter_name;

    public:
        entry_set_roll();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };

    class entry_list : public entry_base {
    public:
        entry_list();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };

    class entry_delete : public entry_base {
    public:
        entry_delete();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };
} // namespace dicebot::entry