#include "./entry_poker.h"
#include <regex>
#include "../data/poker_manager.h"
using namespace dicebot;
using namespace entry;

entry_poker::entry_poker() {
    this->is_stand_alone = false;
    this->filter_command =
        std::regex("^(init|draw|shuffle) *", std::regex_constants::icase);
    this->identifier_regex = "p(?:oker)?";
    this->identifier_list = {"poker", "p"};

    this->help_message = "";
}

bool entry_poker::resolve_request(std::string const &message, event_info &event,
                                  std::string &response) {
    std::smatch detail_command_match;
    std::regex_search(message, detail_command_match, this->filter_command);
    if (!detail_command_match.empty()) {
        if (detail_command_match[1] == "init") {
            std::string init_sequence = detail_command_match.suffix();
            if (init_sequence.empty()) return false;

            auto p_deck =
                poker::poker_manager::get_instance()->get_deck(event.group_id);

            p_deck->init(init_sequence);

            output_constructor oc(event.nickname);
            oc.append_message(u8"已初始化牌库，总计");
            oc.append_message(p_deck->size());
            oc.append_message(u8"张牌");
            response = oc.str();
            return true;
        }
        if (detail_command_match[1] == "draw") {
            poker::card_item card;

            auto p_deck =
                poker::poker_manager::get_instance()->get_deck(event.group_id);

            if (p_deck->draw(card)) {
                output_constructor oc(event.nickname);
                oc.append_message(u8"抽出了");
                oc.append_message(p_deck->render_name(card));
                response = oc.str();
                return true;
            } else {
                output_constructor oc(event.nickname);
                oc.append_message(u8"无牌可抽");
                oc.append_message(p_deck->render_name(card));
                response = oc.str();
                return true;
            }
            return false;
        }
        if (detail_command_match[1] == "shuffle") {
            poker::poker_type card;
            auto p_deck =
                poker::poker_manager::get_instance()->get_deck(event.group_id);
            p_deck->shuffle();
            output_constructor oc(event.nickname);
            oc.append_message(u8"已将牌堆重新切洗");
            response = oc.str();
            return true;
        }
    }
    return false;
}