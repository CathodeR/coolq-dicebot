#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include "./token.h"

namespace dicebot::diceparser {

    struct macro_marker {
        size_t macro_begin;
        size_t macro_end;
    };

    class tokenizer {
       private:
        using macro_map_t = std::map<std::string, std::string>;
        using macro_map_pair_t = typename macro_map_t::node_type;
        std::deque<token_t> &token_container;
        macro_map_t const &macro_map;

        std::vector<std::string> sources;
        std::vector<macro_marker> sources_sites;
        std::string rtail;

        uint8_t ambiguity_flags;

        static const int ambigui_d = 0b00000001;
        static const int ambigui_k = 0b00000010;
        static const int ambigui_kl = 0b00000100;

        size_t regulate_brackets(std::string const &) const;

        bool get_punctuator(token_t &) const;
        bool get_keyword(token_t &);
        bool get_number(token_t &) const;
        bool resolve_identifier(token_t &) const;

        void move_next_cursor(token_t &, size_t length) const;

        token_t peek_next(token_t const &) const;
        bool peek_identifier_d(token_t &) const;

       public:
        tokenizer(std::deque<token_t> &, std::map<std::string, std::string> const &, std::string const &);

        token_t *cur_token();
        token_t *next_token();
        std::string cur_token_string();

        std::string token_string(token_t const &) const;
        std::string tail(token_t const &) const;
    };
}  // namespace dicebot::diceparser