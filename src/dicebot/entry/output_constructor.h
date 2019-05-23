#pragma once

#include <sstream>

namespace dicebot {

    class output_constructor {
    private:
        std::ostringstream _ot;

    public:
        output_constructor(std::string const &nickname) {
            _ot.clear();
            _ot << " * " << nickname << " ";
        }
        output_constructor(std::string const &nickname, std::string const &message) {
            _ot.clear();
            _ot << " * " << nickname << " ";
            if (!message.empty()) _ot << message << " ";
        }

        void append_message(std::string const &message) { _ot << " " << message; }

        template <class any_t>
        void append_message(any_t &&message) {
            _ot << std::forward<decltype(message)>(message) << " ";
        }

        template <class any_t>
        void append_roll(std::string const &roll_command, std::string const &detail, any_t &&roll_result) {
            _ot << u8"掷骰: " << roll_command;
            if (!detail.empty()) _ot << " = " << detail;
            _ot << " = " << std::forward<decltype(roll_result)>(roll_result);
        }

        std::string str() noexcept { return _ot.str(); }

        template <class any_t>
        output_constructor &operator<<(any_t &&message) {
            _ot << std::forward<decltype(message)>(message);
            return *this;
        }
    };
} // namespace dicebot
