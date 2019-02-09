#ifdef _WIN32
    #ifdef _DEBUG
        #pragma comment(lib,"gtestd.lib")
    #else
        #pragma comment(lib,"gtest.lib")
    #endif
#endif

#include "gtest/gtest.h"
#include "dicebot/token.h"

namespace dicebot::test{
    
#pragma region tokenizer test

    TEST(Tokenizer, Base_Streamer){
        using namespace diceparser;
        std::string str_sample = "{ 1 d 20 kl 3} + 3#100.4-(4d6+2)d3";
        struct token_item
        {
            char src[8];
            token_index id;
            keyword kw;
            punctuator pc;
        } token_sample[] = {
            {"{",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_left_brace},
            {"1",       token_index::index_number},
            {"d",       token_index::index_keyword,     keyword::keyword_d},
            {"20",      token_index::index_number},
            {"kl",      token_index::index_keyword,     keyword::keyword_kl},
            {"3",       token_index::index_number},
            {"}",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_right_brace},
            {"+",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_add},
            {"3",       token_index::index_number},
            {"#",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_sharp},
            {"100.4",   token_index::index_number},
            {"-",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_sub},
            {"(",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_left_bracket},
            {"4",       token_index::index_number},
            {"d",       token_index::index_keyword,     keyword::keyword_d},
            {"6",       token_index::index_number},
            {"+",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_add},
            {"2",       token_index::index_number},
            {")",       token_index::index_punctuator,  keyword::keyword_d,     punctuator::punctuator_right_bracket},
            {"d",       token_index::index_keyword,     keyword::keyword_d},
            {"3",       token_index::index_number},
        };
        
        diceparser::token_streamer ts(str_sample);
        size_t pos = 0;
        while(ts.next_token()->id != diceparser::token_index::index_stop){
            ASSERT_EQ(ts.cur_token()->id,token_sample[pos].id);
            if(token_sample[pos].id == token_index::index_keyword){
                ASSERT_EQ(ts.cur_token()->kw,token_sample[pos].kw);
            }
            else if(token_sample[pos].id == token_index::index_punctuator){
                ASSERT_EQ(ts.cur_token()->pc,token_sample[pos].pc);
            }
            pos ++;
        }
    }

    TEST(Tokenizer, Bad_l_brace){
        using namespace diceparser;
        std::string str_sample = "1 d 20 kl 3 { + 3#100.4-(4d6+2)d3";
        struct token_item
        {
            char src[8];
            token_index id;
            keyword kw;
            punctuator pc;
        } token_sample[] = {
            {"1",       token_index::index_number},
            {"d",       token_index::index_keyword,     keyword::keyword_d},
            {"20",      token_index::index_number},
            {"kl",      token_index::index_keyword,     keyword::keyword_kl},
            {"3",       token_index::index_number}
        };
        
        diceparser::token_streamer ts(str_sample);
        size_t pos = 0;
        while(ts.next_token()->id != diceparser::token_index::index_stop){
            ASSERT_LT(pos,5);
            ASSERT_EQ(ts.cur_token()->id,token_sample[pos].id);
            if(token_sample[pos].id == token_index::index_keyword){
                ASSERT_EQ(ts.cur_token()->kw,token_sample[pos].kw);
            }
            else if(token_sample[pos].id == token_index::index_punctuator){
                ASSERT_EQ(ts.cur_token()->pc,token_sample[pos].pc);
            }
            pos ++;
        }
    }

    TEST(Tokenizer, Bad_comma){
        using namespace diceparser;
        std::string str_sample = "1 d 20 kl 3 , + 3#100.4-(4d6+2)d3";
        struct token_item
        {
            char src[8];
            token_index id;
            keyword kw;
            punctuator pc;
        } token_sample[] = {
            {"1",       token_index::index_number},
            {"d",       token_index::index_keyword,     keyword::keyword_d},
            {"20",      token_index::index_number},
            {"kl",      token_index::index_keyword,     keyword::keyword_kl},
            {"3",       token_index::index_number}
        };
        
        diceparser::token_streamer ts(str_sample);
        size_t pos = 0;
        while(ts.next_token()->id != diceparser::token_index::index_stop){
            ASSERT_LT(pos,5);
            ASSERT_EQ(ts.cur_token()->id,token_sample[pos].id);
            if(token_sample[pos].id == token_index::index_keyword){
                ASSERT_EQ(ts.cur_token()->kw,token_sample[pos].kw);
            }
            else if(token_sample[pos].id == token_index::index_punctuator){
                ASSERT_EQ(ts.cur_token()->pc,token_sample[pos].pc);
            }
            pos ++;
        }
    }

#pragma endregion
}