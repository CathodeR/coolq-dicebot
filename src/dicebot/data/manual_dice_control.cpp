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

p_manual manual_dice_control::create(const int64_t user_id,
                                     const int64_t group_id,
                                     const std::string &command) {
    p_manual pmd = std::make_shared<manual_dice>(command);
    if (!(*pmd)) return nullptr;

    manual_kpair pair = manual_kpair(user_id, group_id);

    manual_map::iterator iter = find(pair);
    if (iter != this->end()) {
        erase(iter);
    }
    this->insert(manual_pair(pair, pmd));

    if (!is_no_sql_mode) {
        if (exist_database(pair)) {
            update_database(pair, pmd);
        } else
            insert_database(pair, pmd);
    }

    return pmd;
}

p_manual manual_dice_control::roll(const int64_t user_id,
                                   const int64_t group_id,
                                   const std::string &command) {
    manual_kpair pair = manual_kpair(user_id, group_id);
    auto find_ret = this->find(pair);
    if (find_ret != this->end()) {
        p_manual pmd_manualdice = find_ret->second;
        pmd_manualdice->roll(command);
        if (!is_no_sql_mode) {
            this->update_database(pair, pmd_manualdice);
        }
        return pmd_manualdice;
    } else {
        p_manual pmd_manualdice = std::make_shared<manual_dice>();
        if (!is_no_sql_mode && this->read_database(pair, pmd_manualdice)) {
            pmd_manualdice->roll(command);
            this->update_database(pair, pmd_manualdice);
        }
        this->insert(manual_pair(pair, pmd_manualdice));
        return pmd_manualdice;
    }
}

p_manual manual_dice_control::kill(const int64_t user_id,
                                   const int64_t group_id,
                                   const std::string &command) {
    manual_kpair pair = manual_kpair(user_id, group_id);
    auto find_ret = this->find(pair);

    if (find_ret != this->end()) {
        p_manual pmd_manualdice = find_ret->second;
        pmd_manualdice->kill(command);
        if (!is_no_sql_mode) {
            this->update_database(pair, pmd_manualdice);
        }
        return pmd_manualdice;
    } else {
        p_manual pmd_manualdice = std::make_shared<manual_dice>();
        if (!is_no_sql_mode && this->read_database(pair, pmd_manualdice)) {
            pmd_manualdice->kill(command);
            this->update_database(pair, pmd_manualdice);
        }
        this->insert(manual_pair(pair, pmd_manualdice));
        return pmd_manualdice;
    }
}

p_manual manual_dice_control::add(const int64_t user_id, const int64_t group_id,
                                  const std::string &command) {
    manual_kpair pair = manual_kpair(user_id, group_id);
    auto find_ret = this->find(pair);

    if (find_ret != this->end()) {
        p_manual pmd_manualdice = find_ret->second;
        pmd_manualdice->add(command);
        if (!is_no_sql_mode) {
            this->update_database(pair, pmd_manualdice);
        }
        return pmd_manualdice;
    } else {
        p_manual pmd_manualdice = std::make_shared<manual_dice>();
        if (!is_no_sql_mode && this->read_database(pair, pmd_manualdice)) {
            pmd_manualdice->add(command);
            this->update_database(pair, pmd_manualdice);
        }
        this->insert(manual_pair(pair, pmd_manualdice));
        return pmd_manualdice;
    }
}

p_manual manual_dice_control::killall(const int64_t user_id,
                                      const int64_t group_id) {
    manual_kpair pair = manual_kpair(user_id, group_id);
    auto find_ret = this->find(pair);

    if (find_ret != this->end()) {
        p_manual pmd_manualdice = find_ret->second;
        pmd_manualdice->killall();
        if (!is_no_sql_mode) {
            this->update_database(pair, pmd_manualdice);
        }
        return pmd_manualdice;
    } else {
        p_manual pmd_manualdice = std::make_shared<manual_dice>();
        if (!is_no_sql_mode && this->read_database(pair, pmd_manualdice)) {
            pmd_manualdice->killall();
            this->update_database(pair, pmd_manualdice);
        }
        this->insert(manual_pair(pair, pmd_manualdice));
        return pmd_manualdice;
    }
}

bool manual_dice_control::insert_database(manual_kpair manual_dice_key,
                                          p_manual manual_dice_target) const {
    std::string str_encoded_manualdice(manual_dice_target->encode());
    ostrs ostrs_sql_command(ostrs::ate);
    ostrs_sql_command.str("insert into " MANUALDICE_TABLE_NAME
                          " (qqid, groupid, source) values ( ");
    ostrs_sql_command << manual_dice_key.first << ", " << manual_dice_key.second
                      << ", '" << str_encoded_manualdice << "'"
                      << ");";
    return db_manager::get_instance()->exec_noquery(
        ostrs_sql_command.str().c_str());
}

bool manual_dice_control::update_database(manual_kpair manual_dice_key,
                                          p_manual manual_dice_target) const {
    std::string str_encoded_manualdice(manual_dice_target->encode());
    ostrs ostrs_sql_command(ostrs::ate);
    ostrs_sql_command.str("update " MANUALDICE_TABLE_NAME " set ");
    ostrs_sql_command << " source ='" << str_encoded_manualdice << "'"
                      << " where qqid =" << manual_dice_key.first
                      << " and groupid =" << manual_dice_key.second;
    return db_manager::get_instance()->exec_noquery(
        ostrs_sql_command.str().c_str());
}

bool manual_dice_control::read_database(manual_kpair manual_dice_key,
                                        p_manual manual_dice_target) {
    std::string str_encoded_manualdice(manual_dice_target->encode());
    ostrs ostrs_sql_command(ostrs::ate);
    ostrs_sql_command << "SELECT source FROM " MANUALDICE_TABLE_NAME
                         " where qqid ="
                      << manual_dice_key.first
                      << " and groupid =" << manual_dice_key.second;
    std::string str_manualdice_read;

    return db_manager::get_instance()->exec(
        ostrs_sql_command.str().c_str(),
        manual_dice_target.get(),
        [](void *data, int argc, char **argv, char **azColName) -> int {
            if (argc == 1) {
                manual_dice *pstr_ret = reinterpret_cast<manual_dice *>(data);
                pstr_ret->decode(argv[0]);
                return SQLITE_OK;
            }
            return SQLITE_ABORT;
        });
}

bool manual_dice_control::exist_database(manual_kpair manual_dice_key) const {
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