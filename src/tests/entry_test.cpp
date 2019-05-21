#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "gtestd.lib")
#else
#pragma comment(lib, "gtest.lib")
#endif
#endif

#define NO_COOLQ

#include <climits>
#include <cmath>
#include <list>
#include <regex>

#include "gtest/gtest.h"

#include "dicebot/dicebot.h"

class entry_test : public ::testing::Test {
protected:
    entry_test() { dicebot::initialize("./build/test_db/"); }
    ~entry_test() { dicebot::salvage(); }

public:
    void test_call(dicebot::event_info &ei, const std::string &source, const std::regex &reg_test) {
        std::string output;
        dicebot::try_fill_nickname(ei);
        dicebot::message_pipeline(source, ei, output);
        ASSERT_EQ(std::regex_match(output, reg_test), true);
    }

    void base_call(dicebot::event_info &ei, const std::string &source) {
        std::string output;
        dicebot::try_fill_nickname(ei);
        dicebot::message_pipeline(source, ei, output);
    }
};

TEST_F(entry_test, roll_2d20plus4) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    std::string source_1 = ".r2d20+4";
    std::string source_2 = ".r 2d20+4";
    std::string source_3 = ".roll2d20+4";
    std::string source_4 = ".roll 2d20+4";

    this->base_call(ei, ".ndice");

    std::regex result_reg(u8"^ \\* dice 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = \\d{1,2}$");
    std::regex result_reg_s(u8"^ \\* dice 掷骰: 2d20\\+4 = \\d{1,2}$");

    std::regex roll_source_on(u8"^ \\* dice 启用骰子详细输出$");
    this->test_call(ei, ".rson", roll_source_on);
    this->test_call(ei, ".r s on", roll_source_on);
    this->test_call(ei, ".r s on", roll_source_on);
    this->test_call(ei, ".roll source on", roll_source_on);
    this->test_call(ei, source_1, result_reg);
    this->test_call(ei, source_2, result_reg);
    this->test_call(ei, source_3, result_reg);
    this->test_call(ei, source_4, result_reg);

    std::regex roll_source_off(u8"^ \\* dice 关闭骰子详细输出$");
    this->test_call(ei, ".rsoff", roll_source_off);
    this->test_call(ei, ".r s off", roll_source_off);
    this->test_call(ei, ".r s off", roll_source_off);
    this->test_call(ei, ".roll source off", roll_source_off);
    this->test_call(ei, source_1, result_reg_s);
    this->test_call(ei, source_2, result_reg_s);
    this->test_call(ei, source_3, result_reg_s);
    this->test_call(ei, source_4, result_reg_s);
    this->test_call(ei, ".rs2d20+4", result_reg);
    this->test_call(ei, ".r s 2d20+4", result_reg);
    this->test_call(ei, ".roll s 2d20+4", result_reg);
    this->test_call(ei, ".roll source 2d20+4", result_reg);
}

TEST_F(entry_test, roll_6sharp4d6kl3) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    std::string source = ".r6#4d6kl3";

    this->base_call(ei, ".ndice");
    this->base_call(ei, ".rson");

    std::regex result_reg(
        u8"^ \\* dice 掷骰: \\{(4d6kl3, ){5}4d6kl3\\} = \\{(\\[\\d\\*? \\+ \\d\\*? \\+ \\d\\*? \\+ \\d\\*?\\],){5}(\\[\\d\\*? \\+ "
        u8"\\d\\*? \\+ \\d\\*? \\+ \\d\\*?\\])\\} = \\{(\\d{1,2}, ){5}\\d{1,2}\\}$");
    this->test_call(ei, source, result_reg);

    this->test_call(ei, ".rsoff", std::regex(u8"^ \\* dice 关闭骰子详细输出$"));
    std::regex result_reg_s(u8"^ \\* dice 掷骰: \\{(4d6kl3, ){5}4d6kl3\\} = \\{(\\d{1,2}, ){5}\\d{1,2}\\}$");
    this->test_call(ei, source, result_reg_s);
    this->test_call(ei, ".rs6#4d6kl3", result_reg);
}

TEST_F(entry_test, name_dice) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    ei.group_id = 10001;
    this->test_call(ei, ".ndice1", std::regex(u8"^ \\* .* 的新名字是 dice1$"));
    this->test_call(ei, ".namedice1", std::regex(u8"^ \\* dice1 的新名字是 dice1$"));
    this->test_call(ei, ".n   dice1", std::regex(u8"^ \\* dice1 的新名字是 dice1$"));
    this->test_call(ei, ".name   dice1", std::regex(u8"^ \\* dice1 的新名字是 dice1$"));

    ei.group_id = 10002;
    this->test_call(ei, ".ndice2", std::regex(u8"^ \\* .* 的新名字是 dice2$"));
    this->test_call(ei, ".namedice2", std::regex(u8"^ \\* dice2 的新名字是 dice2$"));
    this->test_call(ei, ".n   dice2", std::regex(u8"^ \\* dice2 的新名字是 dice2$"));
    this->test_call(ei, ".name   dice2", std::regex(u8"^ \\* dice2 的新名字是 dice2$"));

    std::regex result_reg_r1(u8"^ \\* dice1 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = \\d{1,2}$");
    std::regex result_reg_r2(u8"^ \\* dice2 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = \\d{1,2}$");

    ei.group_id = 10001;
    this->test_call(ei, ".rs2d20+4", result_reg_r1);

    ei.group_id = 10002;
    this->test_call(ei, ".rs2d20+4", result_reg_r2);

    ei.group_id = 10001;
    this->test_call(ei, ".ndice", std::regex(u8"^ \\* dice1 的新名字是 dice$"));

    ei.group_id = 10002;
    this->test_call(ei, ".ndice", std::regex(u8"^ \\* dice2 的新名字是 dice$"));
}

TEST_F(entry_test, macro_recall) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    this->base_call(ei, ".ndice");
    this->test_call(ei, ".s 4d6", std::regex(u8"^ \\* dice 设置默认骰子指令: \\(4d6\\)"));
    this->test_call(ei, ".set 4d6", std::regex(u8"^ \\* dice 设置默认骰子指令: \\(4d6\\)"));

    std::regex result_reg(u8"^ \\* dice 掷骰: \\(4d6\\) = \\(\\[\\d \\+ \\d \\+ \\d \\+ \\d\\]\\) = \\d{1,2}$");
    std::regex result_reg_s(u8"^ \\* dice 掷骰: \\(4d6\\) = \\d{1,2}$");

    this->base_call(ei, ".rsoff");
    this->test_call(ei, ".rs", result_reg);
    this->test_call(ei, ".r", result_reg_s);

    this->test_call(ei, ".s 4d6 test", std::regex(u8"^ \\* dice 设置指令: \\(4d6\\) 为 test"));
    this->test_call(ei, ".set 4d6 test", std::regex(u8"^ \\* dice 设置指令: \\(4d6\\) 为 test"));

    this->base_call(ei, ".rsoff");
    this->test_call(ei, ".rs test", result_reg);
    this->test_call(ei, ".r test", result_reg_s);

    this->test_call(ei, ".s 4 test", std::regex(u8"^ \\* dice 设置指令: 4 为 test"));

    std::regex result2_reg(u8"^ \\* dice 掷骰: 4d6 = \\[\\d \\+ \\d \\+ \\d \\+ \\d\\] = \\d{1,2}$");
    std::regex result2_reg_s(u8"^ \\* dice 掷骰: 4d6 = \\d{1,2}$");

    this->base_call(ei, ".rsoff");
    this->test_call(ei, ".rs testd6", result2_reg);
    this->test_call(ei, ".rs test d 6 ", result2_reg);
    this->test_call(ei, ".r testd6", result2_reg_s);
    this->test_call(ei, ".r test d 6", result2_reg_s);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}