#pragma once

#include <regex>
#include <unordered_map>

#include "./entry_base.h"

namespace dicebot::entry {
    class entry_wod_dice : public entry_base {
    public:
        entry_wod_dice() noexcept;
        bool resolve_request(std::string const &message, event_info &event,
                             std::string &response) override;
    };

    class entry_coc_dice : public entry_base {
    private:
        std::regex full_dice;

    public:
        entry_coc_dice();
        bool resolve_request(std::string const &message, event_info &event,
                             std::string &response) override;
    };

    class entry_fate_dice : public entry_base {
        std::regex full_dice;

    public:
        entry_fate_dice();
        bool resolve_request(std::string const &message, event_info &event,
                             std::string &response) override;
    };
} // namespace dicebot::entry