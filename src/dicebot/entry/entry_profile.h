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
        using list_call_t = std::function<bool(entry_list const &self, std::string const &, event_info const &, std::string &)>;

        using call_map_t = std::map<std::string, list_call_t>;
        using call_map_value_t = typename call_map_t::value_type;

        template <class map_t, class value_t = typename map_t::value_type>
        using get_func_template = std::function<void(map_t const &, std::string const &, std::string const &, output_constructor &)>;

    private:
        std::regex filter_command;
        call_map_t call_map;

    public:
        entry_list();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };

    class entry_delete : public entry_base {
        using delete_call_t = std::function<bool(entry_delete const &self, std::string const &, event_info const &, std::string &)>;
        using call_map_t = std::map<std::string, delete_call_t>;
        using call_map_value_t = call_map_t::value_type;

    private:
        std::regex filter_command;
        call_map_t call_map;

    public:
        entry_delete();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };
} // namespace dicebot::entry