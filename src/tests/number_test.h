#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "gtestd.lib")
#else
#pragma comment(lib, "gtest.lib")
#endif
#endif

#include <climits>

#include "../dicebot/utils/number.h"
#include "gtest/gtest.h"

namespace dicebot::test {

#pragma region number test
    template <class T>
    struct add {
        inline T operator()(T a, T b) { return a + b; }
    };

    template <class T>
    struct sub {
        inline T operator()(T a, T b) { return a - b; }
    };

    template <class T>
    struct mul {
        inline T operator()(T a, T b) { return a * b; }
    };

    template <class T>
    struct dvi {
        inline T operator()(T a, T b) { return a / b; }
    };

    template <template <class> class oper, class v1_t, class v2_t>
    void ceil_check(v1_t val1, v2_t val2) {
        dicebot::number ret = oper<dicebot::number>()(val1, val2);

        oper<double> double_oper;

        std::string compare = std::to_string(double_oper((double)val1, (double)val2));
        int pos = compare.find_last_not_of('0');
        if (pos != std::string::npos)
            compare.assign(compare.substr(0, pos + 1));
        else
            compare = std::to_string(double_oper((double)val1, (double)val2));
        ASSERT_EQ(ret.str(), compare);
        ASSERT_EQ(ret.is_using_int, false);
    }

    template <template <class T> class oper, class v1_t, class v2_t>
    void case_check(v1_t val1, v2_t val2, bool is_int) {
        dicebot::number num1 = val1;
        dicebot::number num2 = val2;
        auto ret = oper<dicebot::number>()(num1, num2);
        std::string compare;

        oper<double> double_oper;

        if (is_int)
            compare = std::to_string((int32_t)double_oper((double)val1, (double)val2));
        else {
            compare = std::to_string(double_oper((double)val1, (double)val2));
            int pos = compare.find_last_not_of('0');
            if (pos != std::string::npos) compare.assign(compare.substr(0, pos + 1));
        }
        ASSERT_EQ(ret.str(), compare);
        ASSERT_EQ(ret.is_using_int, is_int);
    }

} // namespace dicebot::test

TEST(NumberTest, Add_01_Int_Int) { dicebot::test::case_check<dicebot::test::add>(12, 1234, true); }
TEST(NumberTest, Add_02_Int_Float) { dicebot::test::case_check<dicebot::test::add>(12, 123.4f, false); }
TEST(NumberTest, Add_03_Float_Int) { dicebot::test::case_check<dicebot::test::add>(123.4f, 12, false); }
TEST(NumberTest, Add_04_Float_Float) { dicebot::test::case_check<dicebot::test::add>(123.4f, 12.6f, true); }
TEST(NumberTest, Add_05_MAX_UP) { dicebot::test::ceil_check<dicebot::test::add>(INT_MAX, 1); }
TEST(NumberTest, Add_05_MAX_DOWN) { dicebot::test::ceil_check<dicebot::test::add>(-INT_MAX, -11); }

TEST(NumberTest, Minus_01_Int_Int) { dicebot::test::case_check<dicebot::test::sub>(1234, 12, true); }
TEST(NumberTest, Minus_02_Int_Float) { dicebot::test::case_check<dicebot::test::sub>(1234, 12.3f, false); }
TEST(NumberTest, Minus_03_Float_Int) { dicebot::test::case_check<dicebot::test::sub>(123.4f, 12, false); }
TEST(NumberTest, Minus_04_Float_Float) { dicebot::test::case_check<dicebot::test::sub>(123.4f, 12.4f, true); }
TEST(NumberTest, Minus_05_MAX_UP) { dicebot::test::ceil_check<dicebot::test::sub>(INT_MAX, -5); }
TEST(NumberTest, Minus_06_MAX_DOWN) { dicebot::test::ceil_check<dicebot::test::sub>(-INT_MAX, 11); }

TEST(NumberTest, Product_01_Int_Int) { dicebot::test::case_check<dicebot::test::mul>(12, 12, true); }
TEST(NumberTest, Product_02_Int_Float) { dicebot::test::case_check<dicebot::test::mul>(12, 12.3f, false); }
TEST(NumberTest, Product_03_Float_Int) { dicebot::test::case_check<dicebot::test::mul>(25.4f, 12, false); }
TEST(NumberTest, Product_04_Float_Float) { dicebot::test::case_check<dicebot::test::mul>(2.5f, 0.4f, true); }
TEST(NumberTest, Product_05_MAX_A_UP) { dicebot::test::ceil_check<dicebot::test::mul>(INT_MAX, 5); }
TEST(NumberTest, Product_06_MAX_A_DOWN) { dicebot::test::ceil_check<dicebot::test::mul>(INT_MAX, -3); }
TEST(NumberTest, Product_05_MAX_B_UP) { dicebot::test::ceil_check<dicebot::test::mul>(-INT_MAX, -5); }
TEST(NumberTest, Product_06_MAX_B_DOWN) { dicebot::test::ceil_check<dicebot::test::mul>(-INT_MAX, 3); }

TEST(NumberTest, Divide_01_Int_Int) { dicebot::test::case_check<dicebot::test::dvi>(48, 12, true); }
TEST(NumberTest, Divide_02_Int_Float) { dicebot::test::case_check<dicebot::test::dvi>(12, 12.3f, false); }
TEST(NumberTest, Divide_03_Float_Int) { dicebot::test::case_check<dicebot::test::dvi>(25.4f, 12, false); }
TEST(NumberTest, Divide_04_Float_Float) { dicebot::test::case_check<dicebot::test::dvi>(3.6f, 1.2f, true); }
TEST(NumberTest, Divide_05_MAX_A_UP) { dicebot::test::ceil_check<dicebot::test::dvi>(INT_MAX, 5); }
TEST(NumberTest, Divide_06_MAX_A_DOWN) { dicebot::test::ceil_check<dicebot::test::dvi>(INT_MAX, -3); }
TEST(NumberTest, Divide_05_MAX_B_UP) { dicebot::test::ceil_check<dicebot::test::dvi>(-INT_MAX, -5); }
TEST(NumberTest, Divide_06_MAX_B_DOWN) { dicebot::test::ceil_check<dicebot::test::dvi>(-INT_MAX, 3); }
TEST(NumberTest, Divide_07_Int_Int) { dicebot::test::case_check<dicebot::test::dvi>(32, 12, false); }
#pragma endregion
