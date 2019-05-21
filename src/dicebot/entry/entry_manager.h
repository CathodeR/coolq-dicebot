#pragma once

#include "../common.h"
#include "./entry_base.h"

namespace dicebot {
    class entry_manager {
        using entry_map_t = std::map<std::string, entry::p_entry>;
        using entry_pair_t = entry_map_t::value_type;

    private:
        entry_map_t entry_cmd_map;
        std::list<entry::p_entry> entry_list;
        std::regex regex_command;

    public:
        void register_dice(entry::p_entry entry);
        void finish_initialization();
        entry::p_entry const get_entry(std::string command) const;
        std::regex const* get_regex_command() const;
    };
} // namespace dicebot

namespace dicebot::entry {
    class entry_help : public entry_base {
        using help_map_t = std::map<std::string, std::string>;
        using help_pair_t = help_map_t::value_type;

    private:
        help_map_t help_map;
        std::regex filter_command;
        std::list<std::string> entry_regex_list;
        std::string default_help_message;

    public:
        entry_help();
        bool register_help(p_entry v_entry);
        void generate_filter_command();
        bool resolve_request(std::string const& message, event_info& event, std::string& response) override;
    };
} // namespace dicebot::entry