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

#include "gtest/gtest.h"

#include "dicebot/dicebot.h"

#include "./interpreter_test.h"
#include "./number_test.h"
#include "./roll_test.h"

class Protocol_Level_test : public ::testing::Test {
protected:
    Protocol_Level_test() { dicebot::initialize("./build/test_db/"); }
    ~Protocol_Level_test() { dicebot::salvage(); }
};

TEST_F(Protocol_Level_test, test1) {
    dicebot::event_info ei(123456);
    ei.nickname = "dynilath";
    std::string output;
    dicebot::message_pipeline(".r2d20+4", ei, output);
    ASSERT_GT(output.size(), (unsigned)0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}