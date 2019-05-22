#include "./entry_manual_dice.h"

#include <regex>

#include "../../cqsdk/utils/vendor/cpp-base64/base64.h"
#include "../data/manual_dice_control.h"
#include "../data/nick_manager.h"
#include "../dice_roller.h"
#include "../entity/manual_dice.h"
#include "../utils/utils.h"

using namespace dicebot;
using namespace dicebot::entry;

using md_ctrl = manual::manual_dice_control;

static const std::regex filter_integer("^ *(\\d+) *");
static const std::regex filter_manual_dice(
    "^ *((\\+)?\\d*d\\d+)(\\+\\d*d\\d+)* *", std::regex_constants::icase);

static auto manualdice_add = [](std::string const& message,
                                const event_info& event,
                                std::string& response) noexcept -> bool {
    std::smatch roll_match;
    std::regex_search(message, roll_match, filter_manual_dice);
    if (!roll_match.empty()) {
        std::string str_command = roll_match.str();
        std::string str_roll_message = roll_match.suffix().str();
        utils::remove_space_and_tab(str_command);

        auto md_target = md_ctrl::get_instance()->find_manual_dice(event);
        if (md_target->second) {
            md_target->second.add(str_command);
            md_ctrl::get_instance()->sync_database(md_target);

            output_constructor oc(event.nickname);
            if (roll_match.suffix().length()) oc << roll_match.suffix().str();
            oc << u8"在桌上增加了这些骰子:" << str_command << u8"当前状态:"
               << md_target->second.str();
            response = oc.str();
            return true;
        }
    }
    return false;
};

static auto manualdice_killall = [](std::string const& message,
                                    const event_info& event,
                                    std::string& response) noexcept -> bool {
    auto md_target = md_ctrl::get_instance()->find_manual_dice(event);
    if (md_target->second) {
        md_target->second.killall();
        output_constructor oc(event.nickname);
        if (!message.empty()) oc << message;
        oc << u8"杀掉了所有的骰子" << u8"当前状态:" << md_target->second.str();
        response = oc.str();
        return true;
    }
    return false;
};

static auto manualdice_kill = [](std::string const& message,
                                 const event_info& event,
                                 std::string& response) noexcept -> bool {
    std::smatch roll_match;
    std::regex_search(message, roll_match, filter_integer);
    if (!roll_match.empty()) {
        std::string str_command = roll_match.str();
        utils::remove_space_and_tab(str_command);
        auto md_target = md_ctrl::get_instance()->find_manual_dice(event);
        if (md_target->second) {
            md_target->second.kill(str_command);
            output_constructor oc(event.nickname);
            if (roll_match.suffix().length()) oc << roll_match.suffix().str();
            oc << u8"杀死桌上的第" << str_command << u8"个骰子" << u8"当前状态:"
               << md_target->second.str();
            response = oc.str();
            return true;
        }
    }
    return false;
};

static auto manualdice_roll = [](std::string const& message,
                                 const event_info& event,
                                 std::string& response) noexcept -> bool {
    std::smatch roll_match;
    std::regex_search(message, roll_match, filter_integer);
    if (!roll_match.empty()) {
        std::string str_command = roll_match.str();
        std::string str_roll_message = roll_match.suffix().str();
        utils::remove_space_and_tab(str_command);

        auto md_target = md_ctrl::get_instance()->find_manual_dice(event);
        if (md_target->second) {
            md_target->second.roll(str_command);
            md_ctrl::get_instance()->sync_database(md_target);
            output_constructor oc(event.nickname);
            if (roll_match.suffix().length()) oc << roll_match.suffix().str();
            oc << u8"重骰桌上的第" << str_command << u8"个骰子" << u8"当前状态:"
               << md_target->second.str();
            response = oc.str();
            return true;
        }
    }
    return false;
};

static auto manualdice_create = [](std::string const& message,
                                   const event_info& event,
                                   std::string& response) noexcept -> bool {
    std::smatch roll_match;
    std::regex_search(message, roll_match, filter_manual_dice);
    if (!roll_match.empty()) {
        std::string str_command = roll_match.str();
        utils::remove_space_and_tab(str_command);

        auto md_target = md_ctrl::get_instance()->find_manual_dice(event);
        if (md_target->second) {
            md_target->second.killall();
            md_target->second.add(str_command);
            output_constructor oc(event.nickname);
            if (roll_match.suffix().length()) oc << roll_match.suffix().str();
            oc << u8"在桌上放了这些骰子:" << str_command << u8"当前状态:"
               << md_target->second.str();
            response = oc.str();
            return true;
        }
    }
    return false;
};

using func_type = std::function<bool(std::string const& message,
                                     const event_info&, std::string&)>;
using func_map_t = std::map<std::string, func_type>;

static const func_map_t func_map = {{"ka", manualdice_killall},
                                    {"a", manualdice_add},
                                    {"k", manualdice_kill},
                                    {"r", manualdice_roll},
                                    {"killall", manualdice_killall},
                                    {"add", manualdice_add},
                                    {"kill", manualdice_kill},
                                    {"roll", manualdice_roll}};

std::regex filter_command("^(ka|a|k|r|killall|add|kill|roll) *",
                          std::regex_constants::icase);

entry_manual_dice::entry_manual_dice() noexcept {
    this->is_stand_alone = false;
    this->identifier_regex = "m(?:annual)?";
    this->identifier_list = {"mannual", "m"};
    this->help_message = base64_decode(
        "5omL5Yqo6aqw5a2QKC5tYW51YWzmiJbogIUubSkK"
        "5oyH5LukLm0gW2RpY2Vd77ya5Lqn55Sf5omL5Yqo"
        "6aqw5a2Q77yM5ZCO5o6l6aqw5a2Q77yM6L+Z6YeM"
        "5LiN5pSv5oyB566X5byPCuaMh+S7pC5tciBbbnVt"
        "YmVyXe+8mumqsOaMh+WumumqsOWtkO+8jOWQjuaO"
        "pemqsOWtkOW6j+WPtwrmjIfku6QubWsgW251bWJl"
        "cl3vvJrmtojnga3mjIflrprpqrDlrZDvvIzlkI7m"
        "jqXpqrDlrZDluo/lj7cK5oyH5LukLm1rYe+8mua4"
        "heepuuaJgOacieaJi+WKqOmqsOWtkA==");
}

bool entry_manual_dice::resolve_request(std::string const& message,
                                        event_info& event,
                                        std::string& response) {
    std::string str_nickname = event.nickname;

    std::smatch match_command;
    std::regex_search(message, match_command, filter_command);
    if (match_command.empty())
        return manualdice_create(message, event, response);

    std::string str_match = match_command[1];
    utils::lower_case(str_match);

    auto iter = func_map.find(str_match);
    if (iter != func_map.end()) {
        return iter->second(match_command.suffix().str(), event, response);
    }
    return false;
}