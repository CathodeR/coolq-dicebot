#pragma once

#include "../common.h"

namespace dicebot::entry {

    using dice_request = std::string *(std::string const &, event_info const &, std::string &);

    class output_constructor {
    private:
        ostrs _ot;

    public:
        output_constructor(std::string const &nickname) {
            _ot.str("");
            _ot << " * " << nickname;
        }

        void append_message(std::string const &message) { _ot << " " << message; }

        template <class any_t>
        void append_message(any_t &&message) {
            _ot << " " << message;
        }

        template <class any_t>
        void append_roll(std::string const &roll_command, std::string const &detail, any_t &&roll_result) {
            _ot << u8" 掷骰: " << roll_command;
            if (!detail.empty()) _ot << " = " << detail;
            _ot << " = " << roll_result;
        }

        std::string str() noexcept { return _ot.str(); }
    };

    class entry_base {
    public:
        bool is_stand_alone = false;
        std::string identifier_regex;
        std::list<std::string> identifier_list;
        std::string help_message;
        virtual bool resolve_request(std::string const &message, event_info &event, std::string &response) { return false; }
    };

    using p_entry = std::shared_ptr<entry_base>;
} // namespace dicebot::entry
