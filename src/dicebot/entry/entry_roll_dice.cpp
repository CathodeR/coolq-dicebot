#include "./entry_roll_dice.h"

#include <regex>

#include "../data/nick_manager.h"
#include "../data/profile_manager.h"
#include "../parser/dicenalyzer.h"
#include "../parser/parser.h"
#include "../utils/utils.h"

// regular dice, with detailed info

using namespace dicebot;
using namespace dicebot::entry;

static const std::regex filter_command("^s(?:ource)? *",
                                       std::regex_constants::icase);
entry_roll_dice::entry_roll_dice() noexcept {
    this->is_stand_alone = false;
    this->identifier_regex = "r(?:oll)?";
    this->identifier_list = {"roll", "r"};
    this->help_message =
        u8"骰子指令(.roll或.r)\n"
        u8"指令.r 1d20：骰一个1d20\n"
        u8"指令.r 1d20+2d6+4：骰1d20+2d6+4，并输出其计算结果\n"
        u8"指令.r (10d6+10)*150%：骰(10d6+10)*150%，并输出其计算结果\n"
        u8"指令.r 4d6k3：骰4d6，取其中较高的三个结果\n"
        u8"指令.r 2d20kl1：骰2d20，取其中较低的一个结果\n"
        u8"指令.r 6#4d6kl3：骰6次4d6kl3，将它们的结果分别显示\n"
        u8"指令.r {1d20,1d12,1d10,1d8,1d6}：分别骰这些骰子，结果分别显示\n"
        u8"指令.r 2#d20-{1,2}：骰两次d20,结果分别减去1和2，结果分别显示\n"
        u8"指令.roll source off：停止显示你的骰子的详细结果\n"
        u8"指令.rs off：上述指令的较短形式\n"
        u8"指令.roll source on：恢复显示你的骰子的详细结果\n"
        u8"指令.rs on：上述指令的较短形式\n"
        u8"指令.rs 3d6：骰3d6，无视rs off状态，仍然显示详细结果\n";
}

bool entry_roll_dice::resolve_request(std::string const& message,
                                      event_info& event,
                                      std::string& response) {
    bool detailed_roll_message = false;
    std::smatch match_list_command_detail;
    std::regex_search(message, match_list_command_detail, filter_command);

    std::string message_cp;
    auto pfm = profile::profile_manager::get_instance();

    profile::var_pair var;
    if (pfm->get_profile(event.user_id)
            ->sys_vars.get(profile::sys_var_type::rs_on, var)
        == profile::profile_status::finished) {
        detailed_roll_message = var.second != 0;
    }

    if (!match_list_command_detail.empty()) {
        message_cp = match_list_command_detail.suffix().str();
        if (message_cp == "on") {
            var.second = true;
            if (pfm->get_profile(event.user_id)
                    ->sys_vars.set(profile::sys_var_type::rs_on, var)
                == profile::profile_status::finished) {
                output_constructor oc(event.nickname);
                oc << u8"启用骰子详细输出";
                response = oc.str();
                return true;
            } else
                return false;
        } else if (message_cp == "off") {
            var.second = false;
            if (pfm->get_profile(event.user_id)
                    ->sys_vars.set(profile::sys_var_type::rs_on, var)
                == profile::profile_status::finished) {
                output_constructor oc(event.nickname);
                oc << u8"关闭骰子详细输出";
                response = oc.str();
                return true;
            } else
                return false;
        } else {
            detailed_roll_message = true;
        }
    } else
        message_cp = message;

    if (message_cp.empty()) {
        if (pfm->get_profile(event.user_id)
                ->def_roll.get(profile::def_roll_type::def_roll, message_cp)
            != profile::profile_status::finished)
            return false;
    }

    diceparser::tokenizer::token_container_t tk_cont;
    diceparser::tokenizer tknz(tk_cont,
                               {true, true},
                               message_cp,
                               &pfm->get_profile(event.user_id)->mac_rolls);
    diceparser::parser parser(tknz);
    auto pcomp = parser.parse(message_cp);
    if (!pcomp) return false;
    auto p_dice = diceparser::build_component_from_syntax(pcomp.get());
    if (!p_dice) return false;

    std::string str_roll_command;
    std::string str_roll_detail;
    std::string str_result;
    diceparser::str_container strs_command;

    p_dice->print(strs_command);
    for (const auto& str : strs_command) str_roll_command.append(str);

    auto p_dicelet = std::dynamic_pointer_cast<diceparser::dicelet>(p_dice);

    if (p_dicelet) {
        diceparser::str_container strs_detail;
        diceparser::result_container results;
        p_dicelet->roll_dicelet(results, strs_detail);

        str_result.assign(
            diceparser::result_builder<diceparser::result_container>(
                "{",
                results,
                [](const number& n) -> std::string { return n.str(); },
                ", ",
                "}"));
        if (detailed_roll_message)
            str_roll_detail.assign(
                diceparser::result_builder<diceparser::str_container>(
                    "",
                    strs_detail,
                    [](const std::string& s) -> decltype(s) { return s; },
                    "",
                    ""));
    } else {
        diceparser::str_container strs_detail;
        number result = p_dice->roll_the_dice(strs_detail);
        str_result.append(result.str());
        if (detailed_roll_message)
            str_roll_detail.assign(
                diceparser::result_builder<diceparser::str_container>(
                    "",
                    strs_detail,
                    [](const std::string& s) -> decltype(s) { return s; },
                    "",
                    ""));
    }

    output_constructor oc(event.nickname, parser.tail);
    if (detailed_roll_message)
        oc.append_roll(str_roll_command, str_roll_detail, str_result);
    else
        oc.append_roll(str_roll_command, "", str_result);
    response = oc.str();
    return true;
}