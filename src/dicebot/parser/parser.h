#pragma once

#include "../common.h"
#include "../utils/number.h"
#include "./syntax.h"
#include "./token.h"
#include "./tokenizer.h"

namespace dicebot::diceparser {

    class parser {
        using macro_map_t = std::map<std::string, std::string>;
        // macro_map_t const *p_macro_map;
        tokenizer const& tknzer;
        // std::unique_ptr<std::vector<uint8_t>> vec_status;
        std::unique_ptr<std::vector<p_syntax_item>> vec_symbols;
        // std::unique_ptr<std::list<token_t>> tokens;

    public:
        std::string tail;
        std::string source;
        parser(tokenizer const&) noexcept;
        p_syntax_item parse(std::string const& source);
        p_syntax_item resolve(uint8_t) const;
        p_syntax_item stack(token_t, std::string&&) const;
    };

    template <class container_t, class excutable_t, class item_t = typename container_t::value_type>
    inline std::string result_builder(const char* prefix, const container_t& src, excutable_t strconv, const char* separater,
                                      const char* suffix) {
        std::string ret;
        ret.assign(prefix);
        bool is_first = true;
        for (const auto& item : src) {
            if (is_first)
                is_first = false;
            else
                ret.append(separater);
            ret.append(strconv(item));
        }
        ret.append(suffix);
        return ret;
    };
} // namespace dicebot::diceparser