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

        static profile_manager* get_instance() {
            if (!profile_manager::instance) {
                return profile_manager::create_instance();
            }
            return profile_manager::instance.get();
        }

        bool force_update(int64_t const user_id) {
            if (is_no_sql_mode) return false;

            auto iter = this->find(user_id);
            if (iter == this->end())
                return false;
            else {
                return profile_db::update_database(*(iter->second), user_id);
            }
            return false;
        }

        sptr_user_profile get_profile(int64_t const user_id) {
            auto iter = this->find(user_id);
            if (iter != this->end()) {
                return iter->second;
            } else {
                if (is_no_sql_mode) return nullptr;

                sptr_user_profile upf = std::make_shared<user_profile>();
                profile_db::read_database(*upf, user_id);

                auto t = this->insert(profile_pair(user_id, upf));
                if (!t.second) return nullptr;
                profile_db::insert_database(*(t.first->second), user_id);
                return upf;
            }
        }
    };
} // namespace dicebot::profile