#pragma once
#include <unordered_map>
namespace dicebot::utils {
    using pair_t = std::pair<int64_t, int64_t>;
    struct pair_hash {
        size_t operator()(const pair_t& p) const noexcept {
            return std::_Hash_impl::hash(p);
        }
    };
} // namespace dicebot::utils
