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

        bool exec_noquery(const char *sql) const noexcept;

        bool exec(const char *sql, void *data, int (*callback)(void *, int, char **, char **)) const noexcept;
    };

    class sqlstmt_binder {
        sqlite3_stmt *stmt;
        sqlstmt_binder(sqlite3_stmt *){};

        template <class... Args>
        void column_recur(int which, int64_t &val, Args &... args) {
            val = sqlite3_column_int64(stmt, which);
            this->column_recur(which + 1, std::forward(args)...);
        }

        template <class... Args>
        void column_recur(int which, std::string &val, Args &... args) {
            val = sqlite3_column_text(stmt, which);
            this->column_recur(which + 1, std::forward(args)...);
        }

        template <class... Args>
        void column_recur(int which, int &val, Args &... args) {
            val = sqlite3_column_int(stmt, which);
            this->column_recur(which + 1, std::forward(args)...);
        }

        template <class... Args>
        void column_recur(int which, bool &val, Args &... args) {
            val = sqlite3_column_int(stmt, which) > 0;
            this->column_recur(which + 1, std::forward(args)...);
        }

    public:
        template <class... Args>
        void column(Args &... args) {
            this->column_recur(0, std::forward(args)...);
        }
        int step() { sqlite3_step(stmt); }

        sqlstmt_binder(const sqlstmt_binder &) = delete;
        sqlstmt_binder &operator=(const sqlstmt_binder &) = delete;

        sqlstmt_binder(sqlstmt_binder &&) = default;
        sqlstmt_binder &operator=(sqlstmt_binder &&) = default;
        ~sqlstmt_binder() { sqlite3_reset(stmt); }
    };

    class sqlstmt_wrapper {
        sqlite3_stmt *stmt;

        template <class... Args>
        void bind_recur(int which, int64_t val, Args... args) {
            sqlite3_bind_int64(stmt, which, val);
            this->column_recur(which + 1, std::forward(args)...);
        }
        template <class... Args>
        void bind_recur(int which, int val, Args... args) {
            sqlite3_bind_int(stmt, which, val);
            this->column_recur(which + 1, std::forward(args)...);
        }
        template <class... Args>
        void bind_recur(int which, const char *val, Args... args) {
            sqlite3_bind_text(stmt, which, val, SQLITE_STATIC);
            this->column_recur(which + 1, std::forward(args)...);
        }
        template <class... Args>
        void bind_recur(int which, bool val, Args... args) {
            sqlite3_bind_int(stmt, which, val ? 1 : 0);
            this->column_recur(which + 1, std::forward(args)...);
        }

    public:
        sqlstmt_wrapper(sqlite3 *db, const char *sql) {
            const char *tail;
            sqlite3_prepare_v2(db, sql, -1, &this->stmt, &tail);
        }

        friend sqlstmt_binder::sqlstmt_binder(sqlite3_stmt *para) { sqlstmt_binder::stmt = para; }

        template <class... Args>
        sqlstmt_binder bind(Args... args) {
            this->bind_recur(std::forward(args)...);
            return sqlstmt_binder(stmt);
        }
    };

} // namespace dicebot::database
