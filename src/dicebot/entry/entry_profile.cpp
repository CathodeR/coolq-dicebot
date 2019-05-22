#include "./entry_profile.h"

#include "../../cqsdk/utils/vendor/cpp-base64/base64.h"
#include "../data/nick_manager.h"
#include "../data/profile_manager.h"
#include "../parser/dicenalyzer.h"
#include "../parser/parser.h"
#include "../utils/utils.h"

using namespace dicebot;
using namespace dicebot::entry;

#pragma region set roll
entry_set_roll::entry_set_roll() {
    this->is_stand_alone = false;
    this->identifier_regex = "s(?:et)?";
    this->identifier_list = {"set", "s"};
    this->filter_name = "^([^\\+\\-\\*/\\(\\)\\s]+)";

    this->help_message = base64_decode(
        "6K6+572u6aqw5a2QKC5zZXTmiJbogIUucykK5oyH"
        "5LukLnNldCAxZDEwMO+8muiuvuWumum7mOiupOmq"
        "sOWtkOS4ujFkMTAwCuaMh+S7pC5y77ya5aaC5p6c"
        "LnLmsqHmnInmjqXku7vkvZXpqrDlrZDml7bvvIzk"
        "vJrpqrDpu5jorqTpqrAK5oyH5LukLnNldCA0ZDZr"
        "MyDlsZ7mgKfvvJrorr7lrprkuIDkuKrlkI3np7Dk"
        "uLrigJzlsZ7mgKfigJ3nmoTpqrDlrZDkuLo0ZDZr"
        "MwrmjIfku6QucuWxnuaAp++8muWmguaenC5y5ZCO"
        "6Z2i5piv6aqw5a2Q5ZCN56ew77yM5YiZ6aqw6K+l"
        "5ZCN56ew55qE6aqw5a2QCuazqOaEj++8mumqsOWt"
        "kOWQjeensOS4reS4jeiDveWMheWQqystKi/lkozn"
        "qbrmoLw=");
}

bool entry_set_roll::resolve_request(std::string const& message,
                                     event_info& event, std::string& response) {
    auto pfm = profile::profile_manager::get_instance();

    std::string message_cp = message;

    auto set_default = [this, pfm, &event, &response](
                           std::string const& str_roll_command) -> bool {
        pfm->get_profile(event.user_id)
            ->def_roll.set(profile::def_roll_type::def_roll, str_roll_command);
        pfm->force_update(event.user_id);

        output_constructor oc(event.nickname);
        oc.append_message(u8"设置默认骰子指令:");
        oc.append_message(str_roll_command);
        response = oc.str();
        return true;
    };

    auto set_named = [this, pfm, &event, &response](
                         std::string const& str_roll_command,
                         std::string const& str_message) -> bool {
        std::smatch m_name;
        std::regex_search(str_message, m_name, this->filter_name);
        if (!m_name[1].matched) return false;
        pfm->get_profile(event.user_id)
            ->mac_rolls.set(m_name[1], str_roll_command);
        pfm->force_update(event.user_id);

        output_constructor oc(event.nickname);
        oc.append_message(u8"设置指令:");
        oc.append_message(str_roll_command);
        oc.append_message(u8"为");
        oc.append_message(m_name[1]);
        response = oc.str();
        return true;
    };

    diceparser::tokenizer::token_container_t tk_cont;
    diceparser::tokenizer tknz(tk_cont,
                               diceparser::tokenizer_flag{true, true},
                               message_cp,
                               &(pfm->get_profile(event.user_id)->mac_rolls));
    diceparser::parser parser(tknz);
    auto p_syntax = parser.parse(message_cp);
    if (!p_syntax) return false;

    auto p_comp = diceparser::build_component_from_syntax(p_syntax.get());
    if (!p_comp) return false;

    auto p_number = std::dynamic_pointer_cast<diceparser::comp_number>(p_comp);
    if (p_number) {
        diceparser::str_container cont;

        number result = p_number->roll_the_dice(cont);
        cont.clear();
        p_number->print(cont);

        if (!parser.tail.empty()) {
            return set_named(result.str(), parser.tail);
        } else {
            return set_default(result.str());
        }
    }

    diceparser::str_container cont;

    auto return_same = [](const std::string& s) -> std::string { return s; };

    auto p_dicelet = std::dynamic_pointer_cast<diceparser::dicelet>(p_comp);
    if (p_dicelet) {
        if (!parser.tail.empty()) return false;
        p_dicelet->print(cont);
        return set_default(
            diceparser::result_builder("(", cont, return_same, "", ")"));
    }

    auto p_holder = std::dynamic_pointer_cast<diceparser::base_holder>(p_comp);
    if (p_holder) {
        p_comp->print(cont);
        if (!parser.tail.empty())
            return set_named(
                diceparser::result_builder("", cont, return_same, "", ""),
                parser.tail);
        else
            return set_default(
                diceparser::result_builder("", cont, return_same, "", ""));
    }

    p_comp->print(cont);
    if (!parser.tail.empty())
        return set_named(
            diceparser::result_builder("(", cont, return_same, "", ")"),
            parser.tail);
    else
        return set_default(
            diceparser::result_builder("(", cont, return_same, "", ")"));
}
#pragma endregion

#pragma region list
static auto defr_msg = [](profile::user_profile::def_roll_map_t const& map,
                          std::string const& head, output_constructor& out) {
    if (map.empty()) return;
    out.append_message("\r\n" + head);
    for (auto const& pair : map) {
        out.append_message(pair.second);
    }
};

static auto macro_msg = [](profile::user_profile::mac_roll_map_t const& map,
                           std::string const& message,
                           output_constructor& out) {
    if (map.empty()) return;
    for (auto const& pair : map) {
        if (!message.empty() && pair.first.find(message) == std::string::npos)
            continue;
        out.append_message(u8"\r\n>");
        out.append_message(pair.first);
        out.append_message(u8":");
        out.append_message(pair.second);
    }
};

entry_list::entry_list() {
    this->is_stand_alone = true;
    this->identifier_regex = "l(?:ist)?";
    this->identifier_list = {"list", "l"};

    this->help_message = base64_decode(
        "5pi+56S65bey5L+d5a2Y55qE6aqw5a2Q5ZKM5Y+Y"
        "6YePKC5saXN05oiW6ICFLmwpCuaMh+S7pC5saXN0"
        "IHJvbGzvvJrmmL7npLrmiYDmnInkv53lrZjnmoTp"
        "qrDlrZAK5oyH5LukLmxy77ya5LiK6L+w5oyH5Luk"
        "55qE566A5YaZ5b2i5byPCuaMh+S7pC5saXN0IHZh"
        "cu+8muaYvuekuuaJgOacieS/neWtmOeahOWPmOmH"
        "jwrmjIfku6QubHbvvJrkuIrov7DmjIfku6TnmoTn"
        "roDlhpnlvaLlvI8K5oyH5LukLmxpc3Qgcm9sbCDl"
        "ipvvvJrmmL7npLrmiYDmnInkv53lrZjnmoTpqrDl"
        "rZDkuK3vvIzlkI3np7DluKbmnInigJzlipvigJ3n"
        "moQK5oyH5LukLmxpc3QgdmFyIOazleacr++8muaY"
        "vuekuuaJgOacieS/neWtmOeahOmqsOWtkOS4re+8"
        "jOWQjeensOW4puacieKAnOazleacr+KAneeahArm"
        "jIfku6QubGlzdCBhbGzvvJrmmL7npLrmiYDmnInk"
        "v53lrZjnmoTpqrDlrZDlkozlj5jph48K5rOo5oSP"
        "77yaLmxpc3QgYWxs5Lit55qEYWxs5LiN6IO9566A"
        "5YaZ");
}

bool entry_list::resolve_request(std::string const& message, event_info& event,
                                 std::string& response) {
    if (message.empty()) {
        auto p_profile = profile::profile_manager::get_instance()->get_profile(
            event.user_id);
        output_constructor oc(event.nickname);
        oc.append_message(u8"已设置下列骰子指令:");
        defr_msg(p_profile->def_roll, u8"* 默认 :", oc);
        macro_msg(p_profile->mac_rolls, "", oc);
        response = oc.str();
        return true;
    } else {
        const auto& macros = profile::profile_manager::get_instance()
                                 ->get_profile(event.user_id)
                                 ->mac_rolls;

        output_constructor oc(event.nickname);
        oc.append_message(u8"已设置如下包含\"");
        oc.append_message(message);
        oc.append_message("\"的骰子指令:");
        macro_msg(macros, message, oc);
        response = oc.str();
        return true;
    }
    return false;
}
#pragma endregion

#pragma region delete
entry_delete::entry_delete() {
    this->is_stand_alone = false;
    this->identifier_regex = "d(?:elete)?";
    this->identifier_list = {"d", "delete"};

    this->help_message = base64_decode(
        "5Yig6Zmk5bey5L+d5a2Y55qE6aqw5a2Q5ZKM5Y+Y"
        "6YePKC5kZWxldGXmiJbogIUuZCkK5oyH5LukLmRl"
        "bGV0ZSByb2xs77ya5Yig6Zmk5omA5pyJ5L+d5a2Y"
        "55qE6aqw5a2QCuaMh+S7pC5kcu+8muS4iui/sOaM"
        "h+S7pOeahOeugOWGmeW9ouW8jwrmjIfku6QuZGVs"
        "ZXRlIHZhcu+8muWIoOmZpOaJgOacieS/neWtmOea"
        "hOWPmOmHjwrmjIfku6QuZHbvvJrkuIrov7DmjIfk"
        "u6TnmoTnroDlhpnlvaLlvI8K5oyH5LukLmRlbGV0"
        "ZSByb2xsIOWKm+mHj++8muWIoOmZpOWQjeensOS4"
        "uuKAnOWKm+mHj+KAneeahOmqsOWtkArmjIfku6Qu"
        "ZGVsZXRlIHZhciDms5XmnK/vvJrliKDpmaTlkI3n"
        "p7DkuLrigJzms5XmnK/igJ3nmoTlj5jph48K5oyH"
        "5LukLmRlbGV0ZSBhbGzvvJrmmL7npLrmiYDmnInk"
        "v53lrZjnmoTpqrDlrZDlkozlj5jph48K5rOo5oSP"
        "77yaLmRlbGV0ZSBhbGzkuK3nmoRhbGzkuI3og73n"
        "roDlhpkK5rOo5oSP77ya6buY6K6k6aqw5a2Q5piv"
        "5peg5rOV5Yig6Zmk55qE");
}

bool entry_delete::resolve_request(std::string const& message,
                                   event_info& event, std::string& response) {
    auto pfm = profile::profile_manager::get_instance();
    if (message.empty()) {
        pfm->get_profile(event.user_id)->mac_rolls.clear();
        pfm->force_update(event.user_id);

        output_constructor oc(event.nickname);
        oc.append_message(u8"已删除所有骰子指令");
        response = oc.str();
        return true;
    } else {
        const auto& p_macro = pfm->get_profile(event.user_id)->mac_rolls;
        auto iter = p_macro.find(message);
        if (iter == p_macro.end()) return false;
        pfm->get_profile(event.user_id)->mac_rolls.erase(iter);
        pfm->force_update(event.user_id);

        output_constructor oc(event.nickname);
        oc.append_message(u8"已删除骰子指令:");
        oc.append_message(message);
        response = oc.str();
        return true;
    }
    return false;
}
#pragma endregion