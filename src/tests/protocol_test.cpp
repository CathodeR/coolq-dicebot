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

class Protocol_Level_test : public ::testing::Test {
protected:
    Protocol_Level_test() { dicebot::initialize("./build/test_db/"); }
    ~Protocol_Level_test() { dicebot::salvage(); }
};

TEST_F(Protocol_Level_test, roll_2d20plus4) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";
    std::string output;

    std::string source = ".r2d20+4";

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".ndice", ei, output);
    std::regex result_reg_d(u8"^ \\* .* 的新名字是 dice$");
    ASSERT_EQ(std::regex_match(output, result_reg_d), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rson", ei, output);
    std::regex result_reg_on(u8"^ \\* dice 启用骰子详细输出$");
    ASSERT_EQ(std::regex_match(output, result_reg_on), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(source, ei, output);
    std::regex result_reg_1(u8"^ \\* dice 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = \\d{1,2}$");
    ASSERT_EQ(std::regex_match(output, result_reg_1), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rs2d20+4", ei, output);
    ASSERT_EQ(std::regex_match(output, result_reg_1), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rsoff", ei, output);
    std::regex result_reg_off(u8"^ \\* dice 关闭骰子详细输出$");
    ASSERT_EQ(std::regex_match(output, result_reg_off), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(source, ei, output);
    std::regex result_reg_2(u8"^ \\* dice 掷骰: 2d20\\+4 = \\d{1,2}$");
    ASSERT_EQ(std::regex_match(output, result_reg_2), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rs2d20+4", ei, output);
    ASSERT_EQ(std::regex_match(output, result_reg_1), true);
}

TEST_F(Protocol_Level_test, roll_6sharp4d6kl3) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";
    std::string output;

    std::string source = ".r6#4d6kl3";

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".ndice", ei, output);
    std::regex result_reg_d(u8"^ \\* .* 的新名字是 dice$");
    ASSERT_EQ(std::regex_match(output, result_reg_d), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rson", ei, output);
    std::regex result_reg_on(u8"^ \\* dice 启用骰子详细输出$");
    ASSERT_EQ(std::regex_match(output, result_reg_on), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(source, ei, output);
    std::regex result_reg_1(
        u8"^ \\* dice 掷骰: \\{(4d6kl3, ){5}4d6kl3\\} = \\{(\\[\\d\\*? \\+ \\d\\*? \\+ \\d\\*? \\+ \\d\\*?\\],){5}(\\[\\d\\*? \\+ "
        u8"\\d\\*? \\+ \\d\\*? \\+ \\d\\*?\\])\\} = \\{(\\d{1,2}, ){5}\\d{1,2}\\}$");
    ASSERT_EQ(std::regex_match(output, result_reg_1), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rsoff", ei, output);
    std::regex result_reg_off(u8"^ \\* dice 关闭骰子详细输出$");
    ASSERT_EQ(std::regex_match(output, result_reg_off), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(source, ei, output);
    std::regex result_reg_2(u8"^ \\* dice 掷骰: \\{(4d6kl3, ){5}4d6kl3\\} = \\{(\\d{1,2}, ){5}\\d{1,2}\\}$");
    ASSERT_EQ(std::regex_match(output, result_reg_2), true);

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rs6#4d6kl3", ei, output);
    ASSERT_EQ(std::regex_match(output, result_reg_1), true);
}

TEST_F(Protocol_Level_test, name_dice) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";
    std::string output;

    ei.group_id = 10001;
    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".ndice1", ei, output);
    std::regex result_reg_1(u8"^ \\* .* 的新名字是 dice1$");
    ASSERT_EQ(std::regex_match(output, result_reg_1), true);

    ei.group_id = 10002;
    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".ndice2", ei, output);
    std::regex result_reg_2(u8"^ \\* .* 的新名字是 dice2$");
    ASSERT_EQ(std::regex_match(output, result_reg_2), true);

    ei.group_id = 10001;
    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rs2d20+4", ei, output);
    std::regex result_reg_r1(u8"^ \\* dice1 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = \\d{1,2}$");
    ASSERT_EQ(std::regex_match(output, result_reg_r1), true);

    ei.group_id = 10002;
    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rs2d20+4", ei, output);
    std::regex result_reg_r2(u8"^ \\* dice2 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = \\d{1,2}$");
    ASSERT_EQ(std::regex_match(output, result_reg_r2), true);

    ei.group_id = 10001;
    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".ndice", ei, output);
    std::regex result_reg_d1(u8"^ \\* dice1 的新名字是 dice$");
    ASSERT_EQ(std::regex_match(output, result_reg_d1), true);

    ei.group_id = 10002;
    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".ndice", ei, output);
    std::regex result_reg_d2(u8"^ \\* dice2 的新名字是 dice$");
    ASSERT_EQ(std::regex_match(output, result_reg_d2), true);
}

TEST_F(Protocol_Level_test, macro_recall) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";
    std::string output;

    dicebot::try_fill_nickname(ei);
    dicebot::message_pipeline(".rs2d20+4", ei, output);
    std::regex result_reg_r2(u8"^ \\* dice2 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = \\d{1,2}$");
    ASSERT_EQ(std::regex_match(output, result_reg_r2), true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}