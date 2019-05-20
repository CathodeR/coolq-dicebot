#include "./parser.h"
#include <iostream>

using namespace dicebot;
using namespace diceparser;

#define _s action_t::stack
#define _r action_t::resolve
#define _e action_t::error
#define _ac action_t::accept

enum class action_t : uint8_t { stack, resolve, error, accept };

struct action_item {
    action_t operation;
    uint8_t status;
} constexpr ACTION[][14] = {
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_s, 9}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_ac, 0}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 16}, {_s, 17}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 1}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 18}, {_s, 19}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 2}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 20}, {_s, 21}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 3}},
    {{_e, 0}, {_e, 0}, {_r, 6}, {_r, 6}, {_r, 6}, {_r, 6}, {_s, 22}, {_e, 0}, {_e, 0}, {_s, 23}, {_e, 0}, {_r, 6}, {_r, 6}, {_r, 6}},
    {{_e, 0}, {_e, 0}, {_r, 4}, {_r, 4}, {_r, 4}, {_r, 4}, {_r, 4}, {_r, 4}, {_r, 4}, {_r, 4}, {_e, 0}, {_r, 4}, {_r, 4}, {_r, 4}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_s, 9}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 8}, {_s, 27}, {_r, 8}, {_r, 8}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 8}, {_r, 8}, {_r, 8}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 17}, {_r, 17}, {_r, 17}, {_r, 17}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 17}, {_r, 17}, {_r, 17}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 21}, {_s, 33}, {_r, 21}, {_r, 21}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 21}, {_r, 21}, {_r, 21}},
    {{_e, 0}, {_e, 0}, {_r, 37}, {_r, 37}, {_r, 37}, {_r, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 37}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_s, 39}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 43}, {_s, 42}, {_r, 43}, {_r, 43}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 43}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_s, 64}, {_e, 0}, {_s, 16}, {_s, 17}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_s, 65}, {_e, 0}, {_s, 18}, {_s, 19}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_s, 66}, {_e, 0}, {_s, 20}, {_s, 21}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 9}, {_s, 27}, {_r, 9}, {_r, 9}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 9}, {_r, 9}, {_r, 9}},
    {{_e, 0}, {_e, 0}, {_r, 22}, {_s, 33}, {_r, 22}, {_r, 22}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 22}, {_r, 22}, {_r, 22}},
    {{_e, 0}, {_e, 0}, {_r, 44}, {_s, 42}, {_r, 44}, {_r, 44}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 44}},
    {{_e, 0}, {_e, 0}, {_r, 12}, {_r, 12}, {_r, 12}, {_r, 12}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 12}, {_r, 12}, {_r, 12}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_s, 73}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 77}, {_s, 78}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 33}, {_r, 33}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 79}, {_s, 80}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 34}, {_r, 34}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 6}, {_r, 6}, {_r, 6}, {_r, 6}, {_s, 22}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 6}, {_r, 6}, {_r, 6}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_s, 39}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 8}, {_s, 83}, {_r, 8}, {_r, 8}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 8}, {_r, 8}, {_r, 8}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 21}, {_s, 86}, {_r, 21}, {_r, 21}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 21}, {_r, 21}, {_r, 21}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 87}, {_s, 88}, {_e, 0}},
    {{_s, 6}, {_s, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 14}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 11}, {_s, 27}, {_r, 11}, {_r, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 11}, {_r, 11}, {_r, 11}},
    {{_e, 0}, {_e, 0}, {_r, 24}, {_s, 33}, {_r, 24}, {_r, 24}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 24}, {_r, 24}, {_r, 24}},
    {{_e, 0}, {_e, 0}, {_r, 46}, {_s, 42}, {_r, 46}, {_r, 46}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 46}},
    {{_e, 0}, {_e, 0}, {_r, 10}, {_s, 27}, {_r, 10}, {_r, 10}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 10}, {_r, 10}, {_r, 10}},
    {{_e, 0}, {_e, 0}, {_r, 23}, {_s, 33}, {_r, 23}, {_r, 23}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 23}, {_r, 23}, {_r, 23}},
    {{_e, 0}, {_e, 0}, {_r, 45}, {_s, 42}, {_r, 45}, {_r, 45}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 45}},
    {{_e, 0}, {_e, 0}, {_r, 26}, {_s, 27}, {_r, 26}, {_r, 26}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 26}, {_r, 26}, {_r, 26}},
    {{_e, 0}, {_e, 0}, {_r, 28}, {_s, 33}, {_r, 28}, {_r, 28}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 28}, {_r, 28}, {_r, 28}},
    {{_e, 0}, {_e, 0}, {_r, 48}, {_s, 42}, {_r, 48}, {_r, 48}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 48}},
    {{_e, 0}, {_e, 0}, {_r, 25}, {_s, 27}, {_r, 25}, {_r, 25}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 25}, {_r, 25}, {_r, 25}},
    {{_e, 0}, {_e, 0}, {_r, 27}, {_s, 33}, {_r, 27}, {_r, 27}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 27}, {_r, 27}, {_r, 27}},
    {{_e, 0}, {_e, 0}, {_r, 47}, {_s, 42}, {_r, 47}, {_r, 47}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 47}},
    {{_e, 0}, {_e, 0}, {_r, 50}, {_s, 27}, {_r, 50}, {_r, 50}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 50}},
    {{_e, 0}, {_e, 0}, {_r, 52}, {_s, 33}, {_r, 52}, {_r, 52}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 52}},
    {{_e, 0}, {_e, 0}, {_r, 54}, {_s, 42}, {_r, 54}, {_r, 54}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 54}},
    {{_e, 0}, {_e, 0}, {_r, 49}, {_s, 27}, {_r, 49}, {_r, 49}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 49}},
    {{_e, 0}, {_e, 0}, {_r, 51}, {_s, 33}, {_r, 51}, {_r, 51}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 51}},
    {{_e, 0}, {_e, 0}, {_r, 53}, {_s, 42}, {_r, 53}, {_r, 53}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 53}},
    {{_e, 0}, {_e, 0}, {_r, 13}, {_r, 13}, {_r, 13}, {_r, 13}, {_e, 0}, {_s, 92}, {_s, 93}, {_e, 0}, {_e, 0}, {_r, 13}, {_r, 13}, {_r, 13}},
    {{_e, 0}, {_e, 0}, {_r, 29}, {_r, 29}, {_r, 29}, {_r, 29}, {_s, 22}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 29}},
    {{_e, 0}, {_e, 0}, {_r, 30}, {_r, 30}, {_r, 30}, {_r, 30}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 30}},
    {{_e, 0}, {_e, 0}, {_r, 5}, {_r, 5}, {_r, 5}, {_r, 5}, {_r, 5}, {_r, 5}, {_r, 5}, {_r, 5}, {_e, 0}, {_r, 5}, {_r, 5}, {_r, 5}},
    {{_e, 0}, {_e, 0}, {_r, 16}, {_r, 16}, {_r, 16}, {_r, 16}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 16}, {_r, 16}, {_r, 16}},
    {{_e, 0}, {_e, 0}, {_r, 32}, {_r, 32}, {_r, 32}, {_r, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 32}},
    {{_e, 0}, {_e, 0}, {_r, 7}, {_r, 7}, {_r, 7}, {_r, 7}, {_s, 22}, {_e, 0}, {_e, 0}, {_s, 23}, {_e, 0}, {_r, 7}, {_r, 7}, {_r, 7}},
    {{_e, 0}, {_e, 0}, {_r, 18}, {_r, 18}, {_r, 18}, {_r, 18}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 18}, {_r, 18}, {_r, 18}},
    {{_e, 0}, {_e, 0}, {_r, 38}, {_r, 38}, {_r, 38}, {_r, 38}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 38}},
    {{_e, 0}, {_e, 0}, {_s, 64}, {_e, 0}, {_s, 94}, {_s, 95}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 6}, {_r, 6}, {_r, 6}, {_r, 6}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 6}, {_r, 6}, {_r, 6}},
    {{_e, 0}, {_e, 0}, {_r, 8}, {_s, 96}, {_r, 8}, {_r, 8}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 8}, {_r, 8}, {_r, 8}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 19}, {_r, 19}, {_r, 19}, {_r, 19}, {_s, 22}, {_e, 0}, {_e, 0}, {_s, 23}, {_e, 0}, {_r, 19}, {_r, 19}, {_r, 19}},
    {{_e, 0}, {_e, 0}, {_r, 20}, {_r, 20}, {_r, 20}, {_r, 20}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 20}, {_r, 20}, {_r, 20}},
    {{_e, 0}, {_e, 0}, {_r, 39}, {_r, 39}, {_r, 39}, {_r, 39}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 39}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_s, 64}, {_e, 0}, {_s, 77}, {_s, 78}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_s, 65}, {_e, 0}, {_s, 79}, {_s, 80}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 9}, {_s, 83}, {_r, 9}, {_r, 9}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 9}, {_r, 9}, {_r, 9}},
    {{_e, 0}, {_e, 0}, {_r, 22}, {_s, 86}, {_r, 22}, {_r, 22}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 22}, {_r, 22}, {_r, 22}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 31}, {_r, 31}, {_r, 31}, {_r, 31}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 31}},
    {{_s, 6}, {_s, 37}, {_e, 0}, {_e, 0}, {_s, 39}, {_e, 0}, {_s, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 40}, {_r, 40}, {_r, 40}, {_r, 40}, {_s, 22}, {_e, 0}, {_e, 0}, {_s, 23}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 40}},
    {{_e, 0}, {_e, 0}, {_r, 41}, {_r, 41}, {_r, 41}, {_r, 41}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 41}},
    {{_e, 0}, {_e, 0}, {_r, 42}, {_r, 42}, {_r, 42}, {_r, 42}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 42}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_s, 6}, {_s, 32}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 9}, {_s, 96}, {_r, 9}, {_r, 9}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 9}, {_r, 9}, {_r, 9}},
    {{_e, 0}, {_e, 0}, {_r, 11}, {_s, 83}, {_r, 11}, {_r, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 11}, {_r, 11}, {_r, 11}},
    {{_e, 0}, {_e, 0}, {_r, 24}, {_s, 86}, {_r, 24}, {_r, 24}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 24}, {_r, 24}, {_r, 24}},
    {{_e, 0}, {_e, 0}, {_r, 10}, {_s, 83}, {_r, 10}, {_r, 10}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 10}, {_r, 10}, {_r, 10}},
    {{_e, 0}, {_e, 0}, {_r, 23}, {_s, 86}, {_r, 23}, {_r, 23}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 23}, {_r, 23}, {_r, 23}},
    {{_e, 0}, {_e, 0}, {_r, 26}, {_s, 83}, {_r, 26}, {_r, 26}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 26}, {_r, 26}, {_r, 26}},
    {{_e, 0}, {_e, 0}, {_r, 28}, {_s, 86}, {_r, 28}, {_r, 28}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 28}, {_r, 28}, {_r, 28}},
    {{_e, 0}, {_e, 0}, {_r, 25}, {_s, 83}, {_r, 25}, {_r, 25}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 25}, {_r, 25}, {_r, 25}},
    {{_e, 0}, {_e, 0}, {_r, 27}, {_s, 86}, {_r, 27}, {_r, 27}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 27}, {_r, 27}, {_r, 27}},
    {{_e, 0}, {_e, 0}, {_r, 7}, {_r, 7}, {_r, 7}, {_r, 7}, {_s, 22}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 7}, {_r, 7}, {_r, 7}},
    {{_e, 0}, {_e, 0}, {_r, 19}, {_r, 19}, {_r, 19}, {_r, 19}, {_s, 22}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 19}, {_r, 19}, {_r, 19}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 77}, {_s, 78}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 35}, {_r, 35}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_s, 79}, {_s, 80}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 36}, {_r, 36}, {_e, 0}},
    {{_e, 0}, {_e, 0}, {_r, 14}, {_r, 14}, {_r, 14}, {_r, 14}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 14}, {_r, 14}, {_r, 14}},
    {{_e, 0}, {_e, 0}, {_r, 15}, {_r, 15}, {_r, 15}, {_r, 15}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 15}, {_r, 15}, {_r, 15}},
    {{_e, 0}, {_e, 0}, {_r, 11}, {_s, 96}, {_r, 11}, {_r, 11}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 11}, {_r, 11}, {_r, 11}},
    {{_e, 0}, {_e, 0}, {_r, 10}, {_s, 96}, {_r, 10}, {_r, 10}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 10}, {_r, 10}, {_r, 10}},
    {{_e, 0}, {_e, 0}, {_r, 7}, {_r, 7}, {_r, 7}, {_r, 7}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_e, 0}, {_r, 7}, {_r, 7}, {_r, 7}}};

struct goto_item {
    uint8_t status;
} constexpr GOTO[][12] = {{{0}, {1}, {2}, {3}, {4}, {5}, {8}, {10}, {12}, {13}, {0}, {15}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {24}, {25}, {26}, {5}, {8}, {10}, {12}, {13}, {0}, {15}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {5}, {28}, {10}, {29}, {13}, {0}, {30}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {31}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {34}, {35}, {0}, {36}, {38}, {10}, {40}, {0}, {41}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {5}, {43}, {10}, {44}, {13}, {0}, {45}},
                          {{0}, {0}, {0}, {0}, {0}, {5}, {46}, {10}, {47}, {13}, {0}, {48}},
                          {{0}, {0}, {0}, {0}, {0}, {5}, {49}, {10}, {50}, {13}, {0}, {51}},
                          {{0}, {0}, {0}, {0}, {0}, {5}, {52}, {10}, {53}, {13}, {0}, {54}},
                          {{0}, {0}, {0}, {0}, {0}, {5}, {55}, {10}, {56}, {13}, {0}, {57}},
                          {{0}, {0}, {0}, {0}, {0}, {5}, {58}, {10}, {59}, {13}, {0}, {60}},
                          {{0}, {0}, {0}, {0}, {0}, {61}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {62}, {0}, {63}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {67}, {0}, {68}, {0}, {69}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {70}, {0}, {0}, {71}, {72}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {74}, {0}, {75}, {0}, {76}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {81}, {82}, {0}, {36}, {38}, {10}, {40}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {36}, {84}, {10}, {85}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {89}, {0}, {90}, {0}, {91}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {71}, {97}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {36}, {98}, {10}, {99}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {36}, {100}, {10}, {101}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {36}, {102}, {10}, {103}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {36}, {104}, {10}, {105}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {106}, {0}, {68}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {107}, {0}, {75}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {108}, {109}, {0}, {36}, {38}, {10}, {40}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {110}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {111}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {71}, {112}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {71}, {113}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {114}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                          {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}}};

struct production_item {
    production_type ptype;
    nterminals ntype;
    uint8_t len;
} constexpr PRODUCTION[] = {{production_type::S_acc, nterminals::nterminal_S, 1},
                            {production_type::acc_const_expr, nterminals::nterminal_acc, 1},
                            {production_type::acc_rand_expr, nterminals::nterminal_acc, 1},
                            {production_type::acc_dicelet_expr, nterminals::nterminal_acc, 1},
                            {production_type::const_unit_number, nterminals::nterminal_const_unit, 1},
                            {production_type::const_unit_lparenthesis_const_expr_rparenthesis, nterminals::nterminal_const_unit, 3},
                            {production_type::const_mul_const_unit, nterminals::nterminal_const_mul, 1},
                            {production_type::const_mul_const_mul_muldvi_const_unit, nterminals::nterminal_const_mul, 3},
                            {production_type::const_expr_const_mul, nterminals::nterminal_const_expr, 1},
                            {production_type::const_expr_minus_const_mul, nterminals::nterminal_const_expr, 2},
                            {production_type::const_expr_const_expr_plus_const_mul, nterminals::nterminal_const_expr, 3},
                            {production_type::const_expr_const_expr_minus_const_mul, nterminals::nterminal_const_expr, 3},
                            {production_type::rand_unit_d_const_unit, nterminals::nterminal_rand_unit, 2},
                            {production_type::rand_unit_const_unit_d_const_unit, nterminals::nterminal_rand_unit, 3},
                            {production_type::rand_unit_const_unit_d_const_unit_k_const_unit, nterminals::nterminal_rand_unit, 5},
                            {production_type::rand_unit_const_unit_d_const_unit_kl_const_unit, nterminals::nterminal_rand_unit, 5},
                            {production_type::rand_unit_lparenthesis_rand_expr_rparenthesis, nterminals::nterminal_rand_unit, 3},
                            {production_type::rand_mul_rand_unit, nterminals::nterminal_rand_mul, 1},
                            {production_type::rand_mul_const_mul_muldvi_rand_unit, nterminals::nterminal_rand_mul, 3},
                            {production_type::rand_mul_rand_mul_muldvi_const_unit, nterminals::nterminal_rand_mul, 3},
                            {production_type::rand_mul_rand_mul_muldvi_rand_unit, nterminals::nterminal_rand_mul, 3},
                            {production_type::rand_expr_rand_mul, nterminals::nterminal_rand_expr, 1},
                            {production_type::rand_expr_minus_rand_mul, nterminals::nterminal_rand_expr, 2},
                            {production_type::rand_expr_const_expr_plus_rand_mul, nterminals::nterminal_rand_expr, 3},
                            {production_type::rand_expr_const_expr_minus_rand_mul, nterminals::nterminal_rand_expr, 3},
                            {production_type::rand_expr_rand_expr_plus_const_mul, nterminals::nterminal_rand_expr, 3},
                            {production_type::rand_expr_rand_expr_minus_const_mul, nterminals::nterminal_rand_expr, 3},
                            {production_type::rand_expr_rand_expr_plus_rand_mul, nterminals::nterminal_rand_expr, 3},
                            {production_type::rand_expr_rand_expr_minus_rand_mul, nterminals::nterminal_rand_expr, 3},
                            {production_type::dicelet_u_const_unit_sharp_const_unit, nterminals::nterminal_dicelet_u, 3},
                            {production_type::dicelet_u_const_unit_sharp_rand_unit, nterminals::nterminal_dicelet_u, 3},
                            {production_type::dicelet_u_lbrace_dicelet_ct_rbrace, nterminals::nterminal_dicelet_u, 3},
                            {production_type::dicelet_u_lparenthesis_dicelet_expr_rparenthesis, nterminals::nterminal_dicelet_u, 3},
                            {production_type::dicelet_ct_const_expr, nterminals::nterminal_dicelet_ct, 1},
                            {production_type::dicelet_ct_rand_expr, nterminals::nterminal_dicelet_ct, 1},
                            {production_type::dicelet_ct_dicelet_ct_comma_const_expr, nterminals::nterminal_dicelet_ct, 3},
                            {production_type::dicelet_ct_dicelet_ct_comma_rand_expr, nterminals::nterminal_dicelet_ct, 3},
                            {production_type::dicelet_mul_dicelet_u, nterminals::nterminal_dicelet_mul, 1},
                            {production_type::dicelet_mul_const_mul_muldvi_dicelet_u, nterminals::nterminal_dicelet_mul, 3},
                            {production_type::dicelet_mul_rand_mul_muldvi_dicelet_u, nterminals::nterminal_dicelet_mul, 3},
                            {production_type::dicelet_mul_dicelet_mul_muldvi_const_unit, nterminals::nterminal_dicelet_mul, 3},
                            {production_type::dicelet_mul_dicelet_mul_muldvi_rand_unit, nterminals::nterminal_dicelet_mul, 3},
                            {production_type::dicelet_mul_dicelet_mul_muldvi_dicelet_u, nterminals::nterminal_dicelet_mul, 3},
                            {production_type::dicelet_expr_dicelet_mul, nterminals::nterminal_dicelet_expr, 1},
                            {production_type::dicelet_expr_minus_dicelet_mul, nterminals::nterminal_dicelet_expr, 2},
                            {production_type::dicelet_expr_const_expr_plus_dicelet_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_const_expr_minus_dicelet_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_rand_expr_plus_dicelet_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_rand_expr_minus_dicelet_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_dicelet_expr_plus_const_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_dicelet_expr_minus_const_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_dicelet_expr_plus_rand_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_dicelet_expr_minus_rand_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_dicelet_expr_plus_dicelet_mul, nterminals::nterminal_dicelet_expr, 3},
                            {production_type::dicelet_expr_dicelet_expr_minus_dicelet_mul, nterminals::nterminal_dicelet_expr, 3}};

terminals translate(token_index what) {
    switch (what) {
    case token_index::index_stop:
        return terminals::terminal_end;
    case token_index::index_number:
        return terminals::terminal_number;
    case token_index::keyword_d:
        return terminals::terminal_roll_d;
    case token_index::keyword_k:
        return terminals::terminal_roll_k;
    case token_index::keyword_kl:
        return terminals::terminal_roll_kl;
    case token_index::punct_add:
        return terminals::terminal_plus;
    case token_index::punct_sub:
        return terminals::terminal_minus;
    case token_index::punct_mul:
    case token_index::punct_dvi:
        return terminals::terminal_mul_dvi;
    case token_index::punct_lbrace:
        return terminals::terminal_lbrace;
    case token_index::punct_rbrace:
        return terminals::terminal_rbrace;
    case token_index::punct_comma:
        return terminals::terminal_comma;
    case token_index::punct_lparenthesis:
        return terminals::terminal_lparenthesis;
    case token_index::punct_rparenthesis:
        return terminals::terminal_rparenthesis;
    case token_index::punct_sharp:
        return terminals::terminal_sharp;
    default:
        return terminals::terminal_end;
    }
}

parser::parser(tokenizer const& tknz) noexcept : tknzer(tknz){};
p_syntax_item parser::parse(std::string const& source) {
    std::deque<uint8_t> _status = {0};
    this->vec_symbols = std::make_unique<std::vector<p_syntax_item>>();

    while (tknzer.next_token()->id != token_index::index_stop)
        ;

    tokenizer::token_container_t& deque_input_tokens = tknzer.token_container;

    bool not_finished = true;
    bool err = false;
    while (not_finished) {
        token_t next = deque_input_tokens.front();
        uint8_t status = *(_status.rbegin());
        terminals next_terminal = translate(next.id);
        action_item const& ai = ACTION[status][next_terminal];
        switch (ai.operation) {
        case action_t::stack: {
            _status.push_back(ai.status);
            this->vec_symbols->emplace_back(stack(next, tknzer.token_string(next)));
            deque_input_tokens.pop_front();
            break;
        }
        case action_t::resolve: {
            production_item pi = PRODUCTION[ai.status];
            p_syntax_item p = resolve(ai.status);
            int lbeta = pi.len;
            while (lbeta--) {
                this->vec_symbols->pop_back();
                _status.pop_back();
            }
            uint8_t status_new = *(_status.rbegin());
            this->vec_symbols->emplace_back(std::move(p));
            _status.push_back(GOTO[status_new][this->vec_symbols->back()->type].status);
            break;
        }
        case action_t::accept:
            not_finished = false;
            break;
        default: {
            if (err && !this->vec_symbols->empty()) {
                this->vec_symbols->pop_back();
                _status.pop_back();
            } else if (!err) {
                deque_input_tokens.push_front({token_index::index_stop, 0, npos, npos});
                err = true;
            } else
                not_finished = false;
            break;
        }
        }
        if (!not_finished) break;
    }
    if (this->vec_symbols->empty()) return nullptr;
    this->tail = tknzer.tail(this->vec_symbols->front()->source_token);
    return this->vec_symbols->front();
}

p_syntax_item parser::resolve(uint8_t prod_index) const {
    token_t last_pos = this->vec_symbols->back()->source_token;

    auto item = std::make_shared<syntax_nterminal>(last_pos, PRODUCTION[prod_index].ntype, PRODUCTION[prod_index].ptype);

    int prodl = PRODUCTION[prod_index].len;
    auto iter = this->vec_symbols->rbegin();

    while (--prodl >= 0) {
        item->items[prodl] = *iter;
        iter++;
    }
    return item;
}

p_syntax_item parser::stack(token_t target, std::string&& src) const {
    auto item = std::make_shared<syntax_terminal>(target, translate(target.id), std::move(src));
    return item;
}