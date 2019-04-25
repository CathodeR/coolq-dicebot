#pragma once

#include "../common.h"
#include "../utils/number.h"
#include "./syntax.h"
#include "./token.h"

namespace dicebot::diceparser {

    class parser {
        using macro_map_t = std::map<std::string, std::string>;
        macro_map_t const &macro_map;
        std::vector<uint8_t> vec_status;
        std::vector<p_syntax_item> vec_symbols;
        std::list<token_t> tokens;

    public:
        std::string tail;
        parser(macro_map_t const &);
        p_syntax_item parse(std::string const &source, int mode);
        p_syntax_item resolve(uint8_t) const;
        p_syntax_item stack(token_t, std::string &&) const;
    };
} // namespace dicebot::diceparser