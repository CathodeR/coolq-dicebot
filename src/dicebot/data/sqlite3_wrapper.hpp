#include <sqlite3.h>

static int sqlite3_exec_noquery(sqlite3 *database, const char *sql) {
    char *pchar_err_message = nullptr;
    sqlite3_exec(
        database, sql, [](void *data, int argc, char **argv, char **azColName) -> int { return SQLITE_OK; }, nullptr, &pchar_err_message);
}