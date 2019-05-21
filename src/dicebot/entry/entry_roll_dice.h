#pragma once

#include "../common.h"
#include "./entry_base.h"

namespace dicebot::entry {
    class entry_roll_dice : public entry_base {
    private:
        std::regex filter_command;

    public:
        entry_roll_dice();
        bool resolve_request(std::string const& message, event_info& event, std::string& response) override;
    };
} // namespace dicebot::entry
