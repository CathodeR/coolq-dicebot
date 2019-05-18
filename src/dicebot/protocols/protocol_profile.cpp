#include "./protocol_profile.h"

#include "../../cqsdk/utils/vendor/cpp-base64/base64.h"
#include "../data/nick_manager.h"
#include "../data/profile_manager.h"
#include "../dice_spliter.h"
#include "../parser/dicenalyzer.h"
#include "../parser/parser.h"
#include "../utils/dice_utils.h"

using namespace dicebot;
using namespace dicebot::protocol;

#pragma region set roll
protocol_set_roll::protocol_set_roll() {
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

bool protocol_set_roll::resolve_request(std::string const& message, event_info& event, std::string& response) {
    profile::profile_manager* pfm = profile::profile_manager::get_instance();

    std::string message_cp = message;

    auto set_default = [this, pfm, &event, &response](std::string const& str_roll_command) -> bool {
        pfm->set_value<profile::def_roll_type, std::string>(profile::def_roll_type::def_roll, str_roll_command, event.user_id);

        output_constructor oc(event.nickname);
        oc.append_message(u8"设置默认骰子指令:");
        oc.append_message(str_roll_command);
        response = oc.str();
        return true;
    };

    auto set_named = [this, pfm, &event, &response](std::string const& str_roll_command, std::string const& str_message) -> bool {
        std::smatch m_name;
        std::regex_search(str_message, m_name, this->filter_name);
        if (!m_name[1].matched) return false;

        pfm->set_value<std::string, std::string>(str_message, str_roll_command, event.user_id);

        output_constructor oc(event.nickname);
        oc.append_message(u8"设置指令:");
        oc.append_message(str_roll_command);
        oc.append_message(u8"为");
        oc.append_message(m_name[1]);
        response = oc.str();
        return true;
    };

    diceparser::tokenizer::token_container_t tk_cont;
    diceparser::tokenizer tknz(tk_cont, {true, true}, message_cp, pfm->get_profile(event.user_id)->get_map<std::string, std::string>());
    diceparser::parser parser(tknz);
    auto pcomp = parser.parse(message_cp);
    if (!pcomp) return false;
    if (pcomp->type == diceparser::nterminal_const_expr) {
        auto p_const = diceparser::build_component_from_syntax(pcomp.get());
        if (!p_const) return false;

        diceparser::str_container cont;

        number result = p_const->roll_the_dice(cont);
        cont.clear();
        p_const->print(cont);

        if (parser.tail.size() > 0) {
            return set_named(result.str(), parser.tail);
        } else {
            return set_default(result.str());
        }
    }

    auto p_dice = diceparser::build_component_from_syntax(pcomp.get());
    if (!p_dice) return false;

    auto return_same = [](const std::string& s) -> decltype(s) { return s; };

    auto p_dicelet = std::dynamic_pointer_cast<diceparser::dicelet>(p_dice);
    if (p_dicelet) {
        if (parser.tail.size() > 0) return false;
        diceparser::str_container cont;
        p_dicelet->print(cont);
        return set_default(diceparser::result_builder("(", cont, return_same, "", ")"));
    }

    if (p_dice) {
        diceparser::str_container cont;
        p_dice->print(cont);
        if (parser.tail.size() > 0) {
            return set_named(diceparser::result_builder("(", cont, return_same, "", ")"), parser.tail);
        } else {
            return set_default(diceparser::result_builder("(", cont, return_same, "", ")"));
        }
    }
    return false;
}
#pragma endregion

#pragma region list
protocol_list::gen_defr_t protocol_list::defr_msg = [](profile::user_profile::def_roll_map_t const& map, std::string const& head,
                                                       std::string const& message, output_constructor& out) {
    if (map.size() == 0) return;
    out.append_message("\r\n" + head);
    for (auto const& pair : map) {
        out.append_message(u8"\r\n>");
        out.append_message(pair.second);
    }
};

protocol_list::gen_macro_t protocol_list::macro_msg = [](profile::user_profile::mac_roll_map_t const& map, std::string const& head,
                                                         std::string const& message, output_constructor& out) {
    if (map.size() == 0) return;
    out.append_message("\r\n" + head);
    for (auto const& pair : map) {
        if (message.size() > 0 && pair.first.find(message) == std::string::npos) continue;
        out.append_message(u8"\r\n>");
        out.append_message(pair.first);
        out.append_message(u8":");
        out.append_message(pair.second);
    }
};

protocol_list::protocol_list() {
    this->is_stand_alone = true;
    this->filter_command = std::regex("^(a(?:ll)?|r(?:oll)?)? *");
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

    list_call_t list_all =
        [](protocol_list const& self, std::string const& message, event_info const& event, std::string& response) -> bool {
        profile::sptr_user_profile upf = profile::profile_manager::get_instance()->get_profile(event.user_id);

        auto def_rolls = upf->get_map<profile::def_roll_type, std::string>();
        auto user_vars = upf->get_map<std::string, profile::var_pair>();
        auto macro_rolls = upf->get_map<std::string, std::string>();

        output_constructor oc(event.nickname);
        oc.append_message(u8"的个人信息如下:");
        self.defr_msg(*def_rolls, u8"默认骰子:", "", oc);
        self.macro_msg(*macro_rolls, u8"已设置下列骰子指令:", "", oc);
        response = oc.str();
        return true;
    };
    this->call_map.insert(call_map_value_t("all", list_all));
    this->call_map.insert(call_map_value_t("a", list_all));

    list_call_t list_roll =
        [](protocol_list const& self, std::string const& message, event_info const& event, std::string& response) -> bool {
        profile::sptr_user_profile upf = profile::profile_manager::get_instance()->get_profile(event.user_id);

        auto def_rolls = upf->get_map<profile::def_roll_type, std::string>();
        auto macro_rolls = upf->get_map<std::string, std::string>();

        output_constructor oc(event.nickname);
        oc.append_message(u8"的个人信息如下:");
        self.macro_msg(*macro_rolls, u8"已设置下列骰子指令:", message, oc);
        response = oc.str();
        return true;
    };
    this->call_map.insert(call_map_value_t("roll", list_roll));
    this->call_map.insert(call_map_value_t("r", list_roll));
}

bool protocol_list::resolve_request(std::string const& message, event_info& event, std::string& response) {
    std::smatch m;
    std::regex_search(message, m, this->filter_command);
    if (m.size() > 0) {
        std::string message_cp;
        std::string args = m.suffix();
        if (m[1].matched) {
            message_cp = m[1];
            std::transform(message_cp.begin(), message_cp.end(), message_cp.begin(), tolower);
        } else {
            message_cp = "a";
        }
        auto iter = this->call_map.find(message_cp);
        if (iter != this->call_map.end()) return iter->second(*this, args, event, response);
    }
    return false;
}
#pragma endregion

#pragma region delete
protocol_delete::protocol_delete() {
    this->is_stand_alone = false;
    this->filter_command = std::regex("^(all|v(?:ar)?|r(?:oll)?) *");
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

    delete_call_t delete_all =
        [](protocol_delete const& self, std::string const& message, event_info const& event, std::string& response) -> bool {
        profile::profile_manager* pfm = profile::profile_manager::get_instance();
        profile::sptr_user_profile upf = pfm->get_profile(event.user_id);

        auto user_vars = upf->get_map<std::string, profile::var_pair>();
        auto macro_rolls = upf->get_map<std::string, std::string>();

        user_vars->clear();
        macro_rolls->clear();
        pfm->force_update(event.user_id);

        output_constructor oc(event.nickname);
        oc.append_message(u8"已删除所有骰子指令和变量。");
        response = oc.str();
        return true;
    };
    this->call_map.insert(call_map_value_t("all", delete_all));

    delete_call_t delete_var =
        [](protocol_delete const& self, std::string const& message, event_info const& event, std::string& response) -> bool {
        profile::profile_manager* pfm = profile::profile_manager::get_instance();
        profile::sptr_user_profile upf = pfm->get_profile(event.user_id);

        auto user_vars = upf->get_map<std::string, profile::var_pair>();
        if (message.size() > 0) {
            auto iter = user_vars->find(message);
            if (iter == user_vars->end()) return false;
            user_vars->erase(iter);
            pfm->force_update(event.user_id);

            output_constructor oc(event.nickname);
            oc.append_message(u8"已删除变量:");
            oc.append_message(message);
            response = oc.str();
            return true;
        } else {
            user_vars->clear();
            pfm->force_update(event.user_id);

            output_constructor oc(event.nickname);
            oc.append_message(u8"已删除所有变量。");
            response = oc.str();
            return true;
        }
    };
    this->call_map.insert(call_map_value_t("var", delete_var));
    this->call_map.insert(call_map_value_t("v", delete_var));

    delete_call_t delete_roll =
        [](protocol_delete const& self, std::string const& message, event_info const& event, std::string& response) -> bool {
        profile::profile_manager* pfm = profile::profile_manager::get_instance();
        profile::sptr_user_profile upf = pfm->get_profile(event.user_id);

        auto macro_rolls = upf->get_map<std::string, std::string>();
        if (message.size() > 0) {
            auto iter = macro_rolls->find(message);
            if (iter == macro_rolls->end()) return false;
            macro_rolls->erase(iter);
            pfm->force_update(event.user_id);

            output_constructor oc(event.nickname);
            oc.append_message(u8"已删除骰子指令:");
            oc.append_message(message);
            response = oc.str();
            return true;
        } else {
            macro_rolls->clear();
            pfm->force_update(event.user_id);

            output_constructor oc(event.nickname);
            oc.append_message(u8"已删除所有骰子指令。");
            response = oc.str();
            return true;
        }
    };
    this->call_map.insert(call_map_value_t("roll", delete_roll));
    this->call_map.insert(call_map_value_t("r", delete_roll));
}

bool protocol_delete::resolve_request(std::string const& message, event_info& event, std::string& response) {
    std::smatch m;
    std::regex_search(message, m, this->filter_command);
    if (m.size() > 0) {
        std::string message_cp = m[1];
        std::string args = m.suffix();

        std::transform(message_cp.begin(), message_cp.end(), message_cp.begin(), tolower);

        auto iter = this->call_map.find(message_cp);
        if (iter != this->call_map.end()) return iter->second(*this, args, event, response);
    }
    return false;
}
#pragma endregion