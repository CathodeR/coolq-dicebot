#pragma once
class NickNameControl
{
public:
	NickNameControl();
	~NickNameControl();
	static NickNameControl * instance;
	void getNickName(const int i_AuthCode, const int64_t fromGroup, const int64_t fromQQ, std::string &nickname);
	void setNickName(const int i_AuthCode, const int64_t fromGroup, const int64_t fromQQ, const std::string &nickname);
private:
	sqlite3 * database;
	std::map<int64_t, std::string> map_nickname_cache;
	bool is_no_sql_mode;
	bool is_my_table_exist(const std::string & table_name);
	static int sqlite3_callback(void *data, int argc, char **argv, char **azColName);
	static int sqlite3_callback_check_table(void * data, int argc, char ** argv, char ** azColName);
	static int sqlite3_callback_quarry_name(void * data, int argc, char ** argv, char ** azColName);
};

