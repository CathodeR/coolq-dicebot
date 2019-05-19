#include <sqlite3.h>
#include <memory>
#include <string>

namespace dicebot::database {
    static int sqlite3_exec_noquery(sqlite3 *database, const char *sql) {
        char *pchar_err_message = nullptr;
        sqlite3_exec(database,
                     sql,
                     [](void *data, int argc, char **argv, char **azColName) -> int { return SQLITE_OK; },
                     nullptr,
                     &pchar_err_message);
    }

    extern bool is_no_sql_mode;

    class database_manager {
    private:
        static std::unique_ptr<database_manager> instance;
        bool is_ready;
        sqlite3 *database;

    public:
        sqlite3 *get_database();
        static database_manager *create_instance(const char *str_app_dir) noexcept;
        static void destroy_instance() noexcept;
        static database_manager *get_instance() noexcept;
        database_manager(const char *str_app_dir);
        int register_table(const char *str_table_name, const char *str_table_sql);
        int is_table_exist(const char *table_name, bool &is_exist);

        bool exec_noquery(const char *sql) {
            char *pchar_err_message;
            int ret_code = sqlite3_exec(this->database,
                                        sql,
                                        [](void *data, int argc, char **argv, char **azColName) -> int { return SQLITE_OK; },
                                        nullptr,
                                        &pchar_err_message);
            if (ret_code != SQLITE_OK) {
#ifdef _DEBUG
                std::string err_msg = "sqlite3 errmsg: ";
                err_msg.append(sqlite3_errmsg(database));
                err_msg.append("\nreturned err msg from exec: ");
                err_msg.append(pchar_err_message);
                logger::log("exec_noquery", std::string());
#endif
                is_no_sql_mode = true;
                return false;
            } else
                return true;
        }

        bool exec(const char *sql, int (*callback)(void *, int, char **, char **), void *data) {
            char *pchar_err_message;
            int ret_code = sqlite3_exec(this->database, sql, callback, data, &pchar_err_message);
            if (ret_code != SQLITE_OK) {
#ifdef _DEBUG
                std::string err_msg = "sqlite3 errmsg: ";
                err_msg.append(sqlite3_errmsg(database));
                err_msg.append("\nreturned err msg from exec: ");
                err_msg.append(pchar_err_message);
                logger::log("exec_noquery", std::string());
#endif
                is_no_sql_mode = true;
                return false;
            } else
                return true;
        }
    };

} // namespace dicebot::database
