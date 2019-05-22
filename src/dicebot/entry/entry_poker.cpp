#include "./entry_poker.h"
#include <regex>
#include "../data/poker_manager.h"
using namespace dicebot;
using namespace entry;

using func_type =
    std::function<bool(const std::string &, const event_info &, std::string &)>;

using func_map_t = std::unordered_map<std::string, func_type>;

static auto init_poker = [](const std::string &suffix, const event_info &event,
                            std::string &response) noexcept -> bool {
    if (suffix.empty()) return false;

    auto p_deck =
        poker::poker_manager::get_instance()->get_deck(event.group_id);

    p_deck->init(suffix);

    output_constructor oc(event.nickname);
    oc << u8"已初始化牌库，总计" << p_deck->size() << u8"张牌";
    response = oc.str();
    return true;
};

static auto draw_poker = [](const std::string &suffix, const event_info &event,
                            std::string &response) noexcept -> bool {
    poker::card_item card;
    auto p_deck =
        poker::poker_manager::get_instance()->get_deck(event.group_id);

    if (p_deck->draw(card)) {
        output_constructor oc(event.nickname);
        oc << u8"抽出了" << p_deck->render_name(card);
        response = oc.str();
        return true;
    } else {
        output_constructor oc(event.nickname);
        oc.append_message(u8"无牌可抽");
        response = oc.str();
        return true;
    }
    return false;
};

static auto shuffle_poker = [](const std::string &suffix,
                               const event_info &event,
                               std::string &response) noexcept -> bool {
    poker::poker_type card;
    auto p_deck =
        poker::poker_manager::get_instance()->get_deck(event.group_id);
    p_deck->shuffle();

    output_constructor oc(event.nickname);
    oc << u8"已将牌堆重新切洗";
    response = oc.str();
    return true;
};

static const func_map_t func_map = {{"init", init_poker},
                                    {"draw", draw_poker},
                                    {"d", draw_poker},
                                    {"shuffle", shuffle_poker}

};

static const std::regex filter_command("^(init|draw|d|shuffle) *",
                                       std::regex_constants::icase);
entry_poker::entry_poker() noexcept {
    this->is_stand_alone = false;
    this->identifier_regex = "p(?:oker)?";
    this->identifier_list = {"poker", "p"};
    this->help_message = "";
}

bool entry_poker::resolve_request(std::string const &message, event_info &event,
                                  std::string &response) {
    std::smatch detail_command_match;
    std::regex_search(message, detail_command_match, filter_command);
    if (!detail_command_match.empty()) {
        auto iter = func_map.find(detail_command_match[1]);
        if (iter == func_map.end()) return false;
        iter->second(detail_command_match.suffix().str(), event, response);
    }
    return false;
}