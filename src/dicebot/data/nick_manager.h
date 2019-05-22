#pragma once

#include <unordered_map>
#include "../common.h"
namespace dicebot::nickname {

    using pair_t = std::pair<int64_t, int64_t>;
    struct my_hash_pair {
        size_t operator()(pair_t p) const noexcept { return std::_Hash_impl::hash(p); }
    };

    class nickname_manager {
        using nick_map_t = std::unordered_map<pair_t, std::string, my_hash_pair>;
        using nick_pair_t = nick_map_t::value_type;
        using nick_key_t = nick_map_t::key_type;

        nick_map_t nick_map;

    public:
        static std::unique_ptr<nickname_manager> instance;
        static nickname_manager* create_instance();
        static void destroy_instance();

        bool get_nickname(event_info const& event, std::string& nickname);
        void set_nickname(event_info const& event);
    };

} // namespace dicebot::nickname
