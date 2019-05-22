#include "./entry_specialized_dice.h"
#include "../data/nick_manager.h"
#include "../dice_roller.h"
#include "../utils/utils.h"
using namespace dicebot;
using namespace dicebot::entry;

#pragma region wod

static const std::regex wod_full_dice("^(\\d+)(?:d(\\d+))?(?:b(\\d+))? *",
                                      std::regex_constants::icase);

static const std::regex wod_filter_command("^(n|o) *",
                                           std::regex_constants::icase);

static const auto roll_owod = [](std::string const& message,
                                 const event_info& event,
                                 std::string& response) noexcept -> bool {
    std::smatch command_match;
    std::regex_search(message, command_match, wod_full_dice);
    if (!command_match.empty()) {
        std::string str_roll_source = command_match.str();
        utils::remove_space_and_tab(str_roll_source);

        roll::dice_roll dr;
        roll::roll_owod(dr, str_roll_source);
        if (dr) {
            output_constructor oc(event.nickname);
            if (command_match.suffix().length())
                oc << command_match.suffix().str();
            oc.append_roll("oWoD", dr.detail(), dr.summary);
            response = oc.str();
            return true;
        }
    }
    return false;
};

static const auto roll_nwod = [](std::string const& message,
                                 const event_info& event,
                                 std::string& response) noexcept -> bool {
    std::smatch command_match;
    std::regex_search(message, command_match, wod_full_dice);
    if (!command_match.empty()) {
        std::string str_roll_source = command_match.str();
        utils::remove_space_and_tab(str_roll_source);

        roll::dice_roll dr;
        roll::roll_nwod(dr, str_roll_source);
        if (dr) {
            output_constructor oc(event.nickname);
            if (command_match.suffix().length())
                oc << command_match.suffix().str();
            oc.append_roll("nWoD", dr.detail(), dr.summary);
            response = oc.str();

            return true;
        }
    }
    return false;
};

using wod_call =
    std::function<bool(std::string const&, const event_info&, std::string&)>;
using wod_map_t = std::map<std::string, wod_call>;

static const wod_map_t wod_map = {{"n", roll_nwod}, {"o", roll_owod}};

entry_wod_dice::entry_wod_dice() noexcept {
    this->is_stand_alone = false;
    this->identifier_regex = "w(?:od)?";
    this->identifier_list = {"wod", "w"};

    this->help_message =
        u8"WoD定制骰子(.wod或.w)\n"
        u8"指令.wo4：使用owod规则进行骰池为4的检定\n"
        u8"指令.wn4：使用nwod规则进行骰池为4的检定\n"
        u8"指令.wo4d8：指定难度为8\n"
        u8"指令.wn4b8：指定在8或以上获得奖励骰";
}

bool entry_wod_dice::resolve_request(std::string const& message,
                                     event_info& event, std::string& response) {
    std::smatch command_match;
    std::regex_search(message, command_match, wod_filter_command);
    if (command_match.empty()) return false;

    std::string str_match = command_match[1];
    utils::lower_case(str_match);

    auto iter = wod_map.find(str_match);
    if (iter != wod_map.end()) {
        return iter->second(command_match.suffix().str(), event, response);
    }

    return false;
}
#pragma endregion

#pragma region coc

entry_coc_dice::entry_coc_dice() {
    this->is_stand_alone = false;
    this->identifier_regex = "c(?:oc)?";
    this->identifier_list = {"coc", "c"};

    this->help_message =
        u8"CoC定制骰子(.coc或.c)\n"
        u8"指令.coc：coc定制骰子\n"
        u8"指令.c：上述命令的简写形式\n"
        u8"指令.coc p1：惩罚骰1（penalty 1）\n"
        u8"指令.coc b1：奖励骰1（bonus 1）";
}

static const std::regex coc_full_dice("^([pb]\\d+ *)* *",
                                      std::regex_constants::icase);
bool entry_coc_dice::resolve_request(std::string const& message,
                                     event_info& event, std::string& response) {
    std::smatch roll_match;
    std::regex_search(message, roll_match, coc_full_dice);
    if (!roll_match.empty()) {
        std::string str_roll_message = roll_match.suffix().str();
        std::string str_roll_source = roll_match.str();
        dicebot::utils::remove_space_and_tab(str_roll_source);

        // roll::dice_roller diceRoll(str_roll_source, roll::roll_mode::COC_PB);
        roll::dice_roll dr;
        roll::roll_coc(dr, str_roll_source);
        if (dr) {
            output_constructor oc(event.nickname);
            if (!str_roll_message.empty()) oc.append_message(str_roll_message);
            oc.append_roll(
                "CoC " + str_roll_source, dr.detail_coc(), dr.summary);
            response = oc.str();

            return true;
        }
    }
    return false;
}
#pragma endregion

#pragma region fate
entry_fate_dice::entry_fate_dice() {
    this->is_stand_alone = false;
    this->identifier_regex = "f(?:ate)?";
    this->identifier_list = {"fate", "f"};

    this->help_message =
        u8"FATE定制骰子(.fate或.f)\n"
        u8"指令.f：常规FATE骰\n"
        u8"指令.f+4：指定+4修正";
}

static const std::regex fate_full_dice("^([\\+|\\-]\\d+)? *");

bool entry_fate_dice::resolve_request(std::string const& message,
                                      event_info& event,
                                      std::string& response) {
    std::smatch roll_match;
    std::regex_search(message, roll_match, fate_full_dice);

    std::string str_roll_message;

    roll::dice_roll dr;

    if (roll_match[1].matched) {
        std::string str_command = roll_match[1];
        str_roll_message = roll_match.suffix();
        roll::roll_fate(dr, str_command);
    } else {
        str_roll_message = message;
        roll::roll_fate(dr);
    }

    if (dr) {
        output_constructor oc(event.nickname);
        if (!str_roll_message.empty()) oc.append_message(str_roll_message);
        oc.append_roll("FATE", dr.detail_fate(), dr.summary);
        response = oc.str();
        return true;
    }
    return false;
}
#pragma endregion