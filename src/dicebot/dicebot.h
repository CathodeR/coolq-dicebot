#pragma once

#include "./common.h"
#include "./utils/dice_utils.h"
#include "./utils/number.h"

#include <memory>

#include "./data/database_manager.h"
#include "./data/manual_dice_control.h"
#include "./data/nick_manager.h"
#include "./data/profile_manager.h"
#include "./dice_roller.h"
#include "./protocols/protocol_base.h"
#include "./protocols/protocol_manager.h"
#include "./protocols/protocol_manual_dice.h"
#include "./protocols/protocol_nickname.h"
#include "./protocols/protocol_profile.h"
#include "./protocols/protocol_roll_dice.h"
#include "./protocols/protocol_specialized_dice.h"

namespace dicebot {
    std::unique_ptr<protocol_manager> dice_ptcs;
    profile::profile_manager* pf_ctrl;

    void initialize(const char* app_dir) {
        database::database_manager::create_instance(app_dir);
        nickname::nickname_manager::create_instance();
        manual::manual_dice_control::create_instance();
        profile::profile_manager::create_instance();

        roll::random_initialize();
        dice_ptcs = std::make_unique<protocol_manager>();
        dice_ptcs->register_dice(
            std::make_shared<protocol::protocol_roll_dice>());
        dice_ptcs->register_dice(
            std::make_shared<protocol::protocol_coc_dice>());
        dice_ptcs->register_dice(
            std::make_shared<protocol::protocol_manual_dice>());
        dice_ptcs->register_dice(
            std::make_shared<protocol::protocol_nickname>());
        dice_ptcs->register_dice(
            std::make_shared<protocol::protocol_wod_dice>());
        dice_ptcs->register_dice(
            std::make_shared<protocol::protocol_fate_dice>());
        dice_ptcs->register_dice(
            std::make_shared<protocol::protocol_set_roll>());
        dice_ptcs->register_dice(std::make_shared<protocol::protocol_list>());
        dice_ptcs->register_dice(std::make_shared<protocol::protocol_delete>());
        dice_ptcs->finish_initialization();
    }

    void salvage() {
        if (dice_ptcs) dice_ptcs = nullptr;
        profile::profile_manager::destroy_instance();
        nickname::nickname_manager::destroy_instance();
        manual::manual_dice_control::destroy_instance();
        database::database_manager::destroy_instance();
    }

    void set_logger(std::function<void(std::string, std::string)> varlog) {
        logger::_log = varlog;
    }

    bool try_fill_nickname(event_info& event) {
        return nickname::nickname_manager::instance->get_nickname(
            event, event.nickname);
    }

    bool message_pipeline(std::string const& source, event_info& event,
                          std::string& output) {
        std::list<std::string> source_splits;

        utils::split_line(source, source_splits);

        std::list<std::string>::iterator iter_source = source_splits.begin();

        ostrs ot(ostrs::ate);

        bool is_output_available = false;
        bool does_last_line_have_output = false;

        for (; iter_source != source_splits.end(); iter_source++) {
            size_t pos_of_content;

            if (!utils::find_point(*iter_source, pos_of_content)) continue;

            std::string content = (*iter_source).substr(pos_of_content);

            std::smatch match_command;

            std::regex_search(
                content, match_command, *(dice_ptcs->get_regex_command()));

            if (!match_command[1].matched) continue;

            std::string command = match_command[1];
            protocol::p_protocol target_protocol =
                dice_ptcs->get_protocol(command);

            if (target_protocol->is_stand_alone) {
                if (target_protocol->resolve_request(
                        match_command.suffix().str(), event, output)) {
                    return is_output_available = true;
                }
            } else {
                std::string response;
                if (target_protocol->resolve_request(
                        match_command.suffix().str(), event, response)) {
                    if (is_output_available) ot << std::endl;
                    ot << response;
                    is_output_available = true;
                    continue;
                }
            }
        }

        if (is_output_available) {
            output.assign(ot.str());
        }
        return is_output_available;
    }
} // namespace dicebot