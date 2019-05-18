#pragma once

#include "../common.h"
#include "../data/profile.h"
#include "./protocol_base.h"
namespace dicebot::protocol {
    class protocol_set_roll : public protocol_base {
        std::regex filter_name;

    public:
        protocol_set_roll();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };

    class protocol_list : public protocol_base {
        using list_call_t = std::function<bool(protocol_list const &self, std::string const &, event_info const &, std::string &)>;

        using call_map_t = std::map<std::string, list_call_t>;
        using call_map_value_t = typename call_map_t::value_type;

        template <class map_t, class value_t = typename map_t::value_type>
        using get_func_template = std::function<void(map_t const &, std::string const &, std::string const &, output_constructor &)>;

        using gen_macro_t = get_func_template<profile::user_profile::mac_roll_map_t>;

        using gen_defr_t = get_func_template<profile::user_profile::def_roll_map_t>;

    private:
        std::regex filter_command;
        call_map_t call_map;

    public:
        static gen_macro_t macro_msg;
        static gen_defr_t defr_msg;
        protocol_list();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };

    class protocol_delete : public protocol_base {
        using delete_call_t = std::function<bool(protocol_delete const &self, std::string const &, event_info const &, std::string &)>;
        using call_map_t = std::map<std::string, delete_call_t>;
        using call_map_value_t = call_map_t::value_type;

    private:
        std::regex filter_command;
        call_map_t call_map;

    public:
        protocol_delete();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };
} // namespace dicebot::protocol