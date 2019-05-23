#pragma once

#include <exception>
#include <memory>
#include <vector>

#include "./constants.h"
namespace dicebot::roll {

#define CHECK_LIMITS(_Num, _Face) ((_Face < MAX_DICE_FACE && _Num < MAX_DICE_NUM) && (_Face > 1 && _Num >= 1))

    using dice_pair = std::pair<int32_t, bool>;

    class dice_roll {
    private:
        dice_roll(roll_status const& status) noexcept;

    public:
        int summary;
        std::vector<int> results;
        std::vector<bool> flags;
        std::string detail();
        dice_roll() noexcept;
        size_t add_result(int const result);
        size_t add_ignored_result(int const result);
        size_t update_result(int const result, size_t const target);
        size_t set_ignore(size_t const target);
        size_t set_good(size_t const target);
        void clear() noexcept;
        void finish_roll(bool) noexcept;
        void finish_coc() noexcept;
        std::string detail_coc() noexcept;
        void finish_wod(int const i_d, bool const failing) noexcept;
        std::string detail_fate() noexcept;
    };

    using p_dice_roll = std::shared_ptr<dice_roll>;

    void roll_base(dice_roll&, int const, int const);

    void roll_rdk(dice_roll&, int const num, int const face, int const keep);

    void roll_coc(dice_roll& dice, int const i_bp);

    void roll_wod(dice_roll& dice, int const i_val, int const i_d, int const i_bonus, bool failing);
    void roll_nwod(dice_roll& dice, std::string const& str_dice_command);
    void roll_owod(dice_roll& dice, std::string const& str_dice_command);

    void roll_fate(dice_roll& dice, int const i_val);
} // namespace dicebot::roll