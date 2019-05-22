#pragma once

#include <unordered_map>
#include "../common.h"
#include "./profile.h"

namespace dicebot::profile {
    using sptr_user_profile = std::shared_ptr<user_profile>;
    using profile_map = std::unordered_map<int64_t, sptr_user_profile>;
    using profile_pair = std::pair<int64_t, sptr_user_profile>;

    class profile_manager {
        static std::unique_ptr<profile_manager> instance;
        profile_map profiles;

    public:
        static profile_manager* create_instance() noexcept;
        static profile_manager* get_instance() noexcept;
        static void destroy_instance() noexcept;

        bool force_update(int64_t const user_id) const;

        sptr_user_profile get_profile(int64_t const user_id);
    };
} // namespace dicebot::profile