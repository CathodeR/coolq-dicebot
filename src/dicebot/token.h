#include <cstdint>
#include <string>

namespace dicebot::diceparser{

    enum oper_constants : uint8_t{
        oper_default = 0,
        no_named = 1,
        no_brace = 2
    };

    enum class token_index : uint8_t{
        index_stop,
        index_number,
        index_identifier,
        index_punctuator,
        index_keyword
    };

    enum class keyword : uint8_t{
        keyword_d,
        keyword_k,
        keyword_kl
    };

    struct keyword_container{
        char name[3];
        keyword kw;
    } 
    static constexpr keyword_arr[]{
        {"d", keyword::keyword_d},
        {"kl", keyword::keyword_kl},
        {"k", keyword::keyword_k}
    };

    enum class punctuator : uint8_t{
        punctuator_add,
        punctuator_sub,
        punctuator_mul,
        punctuator_dvi,
        punctuator_left_brace,
        punctuator_right_brace,
        punctuator_comma,
        punctuator_left_bracket,
        punctuator_right_bracket,
        punctuator_sharp
    };

    struct punctuator_container{
        char name;
        punctuator pc;
    } 
    static constexpr punctuator_arr[]{
        {'+', punctuator::punctuator_add},
        {'-', punctuator::punctuator_sub},
        {'*', punctuator::punctuator_mul},
        {'/', punctuator::punctuator_dvi},
        {'{', punctuator::punctuator_left_brace},
        {'}', punctuator::punctuator_right_brace},
        {',', punctuator::punctuator_comma},
        {'(', punctuator::punctuator_left_bracket},
        {')', punctuator::punctuator_right_bracket},
        {'#', punctuator::punctuator_sharp}
    };

    constexpr char illegal_identifier[] = " +-*/{},()#<>&|:%.";

    enum class dice_keep:uint8_t{
        none,
        keep_high,
        keep_low
    };

    struct token_t{
        token_index id;
        keyword kw;
        punctuator pc;
    };
    
    class token_streamer{
    private:
        size_t pos_cur;
        size_t pos_next;
        token_t ret_token;
        std::string src;
        size_t src_max;
        bool next_keyword();
        bool next_punctuator();
        bool next_number();
    public:
        token_streamer(std::string const &source);
        token_t * cur_token();
        token_t * next_token();
        std::string cur_token_string();
    };
}