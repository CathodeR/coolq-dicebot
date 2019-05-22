#include "./manual_dice_control.h"

#include "./database_manager.h"
#include "./manual_dice.h"

using namespace dicebot;
using namespace dicebot::manual;
using namespace dicebot::database;
using db_manager = dicebot::database::database_manager;

#define MANUALDICE_TABLE_NAME "manualdice"
#define MANUALDICE_TABLE_DEFINE           \
    "create table " MANUALDICE_TABLE_NAME \
    "(qqid      int     NOT NULL,"        \
    "groupid   int     NOT NULL,"         \
    "source     text    NOT NULL,"        \
    "primary    key    (QQID,GROUPID));"

static bool insert_database(const utils::pair_t &manual_dice_key,
                            const manual_dice &manual_dice_target) {
    std::string str_encoded_manualdice(manual_dice_target.encode());
    ostrs ostrs_sql_command(ostrs::ate);
    ostrs_sql_command.str("insert into " MANUALDICE_TABLE_NAME
                          " (qqid, groupid, source) values ( ");
    ostrs_sql_command << manual_dice_key.first << ", " << manual_dice_key.second
                      << ", '" << str_encoded_manualdice << "'"
                      << ");";
    return db_manager::get_instance()->exec_noquery(
        ostrs_sql_command.str().c_str());
}

static bool update_database(const utils::pair_t &manual_dice_key,
                            const manual_dice &manual_dice_target) {
    std::string str_encoded_manualdice(manual_dice_target.encode());
    ostrs ostrs_sql_command(ostrs::ate);
    ostrs_sql_command.str("update " MANUALDICE_TABLE_NAME " set ");
    ostrs_sql_command << " source ='" << str_encoded_manualdice << "'"
                      << " where qqid =" << manual_dice_key.first
                      << " and groupid =" << manual_dice_key.second;
    return db_manager::get_instance()->exec_noquery(
        ostrs_sql_command.str().c_str());
}

static bool read_database(const utils::pair_t &manual_dice_key,
                          manual_dice &manual_dice_target) {
    std::string str_encoded_manualdice(manual_dice_target.encode());
    ostrs ostrs_sql_command(ostrs::ate);
    ostrs_sql_command << "SELECT source FROM " MANUALDICE_TABLE_NAME
                         " where qqid ="
                      << manual_dice_key.first
                      << " and groupid =" << manual_dice_key.second;
    std::string str_manualdice_read;

    return db_manager::get_instance()->exec(
        ostrs_sql_command.str().c_str(),
        &manual_dice_target,
        [](void *data, int argc, char **argv, char **azColName) -> int {
            if (argc == 1) {
                manual_dice *pstr_ret = reinterpret_cast<manual_dice *>(data);
                pstr_ret->decode(argv[0]);
                return SQLITE_OK;
            }
            return SQLITE_ABORT;
        });
}

static bool exist_database(const utils::pair_t &manual_dice_key) {
    bool ret = false;
    ostrs ostrs_sql_command(ostrs::ate);
    ostrs_sql_command << "SELECT count(*) FROM " MANUALDICE_TABLE_NAME
                         " where qqid ="
                      << manual_dice_key.first
                      << " and groupid =" << manual_dice_key.second;
    int ret_code = database::database_manager::get_instance()->exec(
        MANUALDICE_TABLE_NAME,
        &ret,
        [](void *data, int argc, char **argv, char **azColName) -> int {
            *(reinterpret_cast<bool *>(data)) = true;
            return SQLITE_OK;
        });
    return ret;
}

std::unique_ptr<manual_dice_control> manual_dice_control::instance = nullptr;

manual_dice_control *manual_dice_control::create_instance() {
    db_manager::get_instance()->register_table(MANUALDICE_TABLE_NAME,
                                               MANUALDICE_TABLE_DEFINE);
    instance = std::make_unique<manual_dice_control>();
}

manual_dice_control *manual_dice_control::get_instance() {
    return instance.get();
}

void manual_dice_control::destroy_instance() { instance = nullptr; }

manual_dice_control::md_map_t::iterator manual_dice_control::find_manual_dice(
    const event_info &event) {
    auto iter = this->manual_dice_map.find(event.pair());
    if (iter == this->manual_dice_map.end()) {
        auto insert_result =
            this->manual_dice_map.insert({event.pair(), manual_dice()});
        if (!insert_result.second) return this->manual_dice_map.end();
        if (read_database(insert_result.first->first,
                          insert_result.first->second))
            return insert_result.first;
        else {
            insert_database(insert_result.first->first,
                            insert_result.first->second);
            return insert_result.first;
        }
    } else
        return iter;
}

void manual_dice_control::sync_database(const md_map_t::iterator iter) const {
    if (iter != this->manual_dice_map.end()) {
        update_database(iter->first, iter->second);
    }
}