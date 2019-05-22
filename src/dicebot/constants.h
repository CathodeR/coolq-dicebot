#pragma once
namespace dicebot {
    namespace roll {
        enum class roll_status {
            FINISHED = 0,
            GENERAL_ERR = 1,
            DICE_NOT_AVAILABLE = 2,
            UNINITIALIZED = -1,
            TOO_MANY_DICE = 3
        };
        enum class roll_mode { DND_DK = 1, COC_PB = 2, WOD_WA = 3 };
    } // namespace roll

    namespace profile {
        enum class profile_status { finished, not_exist, exceed };
        enum class sys_var_type { rs_on };
        enum class def_roll_type { def_roll };
    } // namespace profile

    constexpr unsigned MAX_DICE_UNIT_COUNT = 10;
    constexpr unsigned MAX_DICE_NUM = 50;
    constexpr unsigned MAX_DICE_FACE = 1000;
} // namespace dicebot