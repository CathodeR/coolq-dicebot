#pragma once

#include "../common.h"
#include "./entry_base.h"
namespace dicebot::entry {
    class entry_nickname : public entry_base {
    private:
        std::regex filter_command;

    public:
        entry_nickname();
        bool resolve_request(std::string const& message, event_info& event, std::string& response) override;
    };
} // namespace dicebot::entry
