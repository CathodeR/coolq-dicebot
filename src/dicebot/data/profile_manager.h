#pragma once

#include <unordered_map>
#include "../common.h"
#include "./profile.h"

namespace dicebot::profile {
    using sptr_user_profile = std::shared_ptr<user_profile>;
    using profile_map = std::unordered_map<int64_t, sptr_user_profile>;
    using profile_pair = std::pair<int64_t, sptr_user_profile>;
    namespace profile_db {
        bool read_database(user_profile& profile, int64_t const user_id);
        bool write_database(user_profile const& profile, int64_t const user_id);
        bool insert_database(user_profile const& profile,
                             int64_t const user_id);
        bool update_database(user_profile const& profile,
                             int64_t const user_id);
        bool exist_database(int64_t const user_id);
    } // namespace profile_db

    class profile_manager : profile_map {
        static std::unique_ptr<profile_manager> instance;

    public:
        static profile_manager* create_instance() noexcept;
        static profile_manager* get_instance() noexcept;
        static void destroy_instance() noexcept;

        bool force_update(int64_t const user_id);

        sptr_user_profile get_profile(int64_t const user_id);
    };
} // namespace dicebot::profile