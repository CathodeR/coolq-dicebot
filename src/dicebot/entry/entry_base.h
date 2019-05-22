#pragma once

#include <list>
#include <sstream>

#include "../event_info.h"

namespace dicebot::entry {

    using dice_request = std::string *(std::string const &, event_info const &,
                                       std::string &);

    class output_constructor {
    private:
        std::ostringstream _ot;

    public:
        output_constructor(std::string const &nickname) {
            _ot.clear();
            _ot << " * " << nickname;
        }

        void append_message(std::string const &message) {
            _ot << " " << message;
        }

        template <class any_t>
        void append_message(any_t &&message) {
            _ot << " " << std::forward<decltype(message)>(message);
        }

        template <class any_t>
        void append_roll(std::string const &roll_command,
                         std::string const &detail, any_t &&roll_result) {
            _ot << u8" 掷骰: " << roll_command;
            if (!detail.empty()) _ot << " = " << detail;
            _ot << " = " << std::forward<decltype(roll_result)>(roll_result);
        }

        std::string str() noexcept { return _ot.str(); }

        template <class any_t>
        output_constructor &operator<<(any_t &&message) {
            _ot << " " << std::forward<decltype(message)>(message);
            return *this;
        }
    };

    class entry_base {
    public:
        bool is_stand_alone = false;
        std::string identifier_regex;
        std::list<std::string> identifier_list;
        std::string help_message;
        virtual bool resolve_request(std::string const &message,
                                     event_info &event, std::string &response) {
            return false;
        }
    };

} // namespace dicebot::entry
