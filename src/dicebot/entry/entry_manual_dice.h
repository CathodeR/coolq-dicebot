#pragma once

#include <functional>
#include "../common.h"
#include "./entry_base.h"

namespace dicebot::entry {
    class entry_manual_dice : public entry_base {
        using manual_dice_call = std::function<bool(
            std::string const &message, const event_info &, std::string &)>;

        using manual_map = std::map<std::string, manual_dice_call>;
        using manual_pair = manual_map::value_type;

    private:
        manual_map method_map;

    public:
        std::regex filter_manual_dice;
        std::regex filter_integer;
        std::regex filter_command;
        entry_manual_dice();
        bool resolve_request(std::string const &message, event_info &event,
                             std::string &response) override;
    };
} // namespace dicebot::entry