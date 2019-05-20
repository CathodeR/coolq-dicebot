#pragma once

#include "../common.h"

namespace dicebot::manual {
    class manual_dice;

    using p_manual = std::shared_ptr<manual_dice>;
    using manual_kpair = std::pair<int64_t, int64_t>;
    using manual_map = std::map<manual_kpair, p_manual>;
    using manual_pair = std::pair<manual_kpair, p_manual>;

    class manual_dice_control : public std::map<manual_kpair, p_manual> {
    private:
        static std::unique_ptr<manual_dice_control> instance;

    public:
        static manual_dice_control *get_instance();
        static manual_dice_control *create_instance();
        static void destroy_instance();
        p_manual create(const int64_t, const int64_t, const std::string &);
        p_manual roll(const int64_t, const int64_t, const std::string &);
        p_manual kill(const int64_t, const int64_t, const std::string &);
        p_manual add(const int64_t, const int64_t, const std::string &);
        p_manual killall(const int64_t, const int64_t);
        bool update_database(manual_kpair, p_manual) const;
        bool insert_database(manual_kpair, p_manual) const;
        bool read_database(manual_kpair, p_manual);
        bool exist_database(manual_kpair) const;
    };
} // namespace dicebot::manual
