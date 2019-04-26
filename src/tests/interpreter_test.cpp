#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "gtestd.lib")
#else
#pragma comment(lib, "gtest.lib")
#endif
#endif

#include "dicebot/parser/token.h"
#include "gtest/gtest.h"

namespace dicebot::test {
#pragma region tokenizer test
#pragma endregion
} // namespace dicebot::test

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}