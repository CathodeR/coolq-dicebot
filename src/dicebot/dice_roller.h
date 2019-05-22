#pragma once

#include <exception>
#include <memory>
#include <vector>

#include "./constants.h"
namespace dicebot {
    class face_exceed : public std::exception {
        const char* what() const noexcept override {
            return "too many faces for a die.";
        }
    };
    class dice_exceed : public std::exception {
        const char* what() const noexcept override {
            return "too many dice for a roll.";
        }
    };
    class unit_exceed : public std::exception {
        const char* what() const noexcept override {
            return "too many times for repeating roll.";
        }
    };
    class invalid_dice : public std::exception {
        const char* what() const noexcept override {
            return "a positive integer is required.";
        }
    };
} // namespace dicebot

namespace dicebot::roll {

#define CHECK_LIMITS(_Num, _Face) \
    ((_Face < MAX_DICE_FACE && _Num < MAX_DICE_NUM) && (_Face > 1 && _Num >= 1))

    using dice_pair = std::pair<int32_t, bool>;

    class dice_roll {
    private:
        dice_roll(roll_status const& status) noexcept;

    public:
        int32_t summary;
        std::vector<dice_pair> results;
        std::string detail();
        dice_roll() noexcept;
        size_t add_result(int32_t const result);
        size_t add_ignored_result(int32_t const result);
        size_t update_result(int32_t const result, size_t const target);
        size_t set_ignore(size_t const target);
        size_t set_good(size_t const target);
        void clear() noexcept;
        void finish_roll() noexcept;
        void finish_coc() noexcept;
        std::string detail_coc() noexcept;
        void finish_wod(int const i_d, bool const failing) noexcept;
        std::string detail_fate() noexcept;
    };

    using p_dice_roll = std::shared_ptr<dice_roll>;

    void roll_base(dice_roll&, int const, int const);

    void roll_rdk(dice_roll&, int const num, int const face, int const keep);
    void roll_rdk(dice_roll& dice, std::string const& str_dice_command);

    void roll_coc(dice_roll& dice, int const i_bp);
    void roll_coc(dice_roll& dice, std::string const& str_dice_command);

    void roll_wod(dice_roll& dice, int const i_val, int const i_d,
                  int const i_bonus, bool failing);
    void roll_nwod(dice_roll& dice, std::string const& str_dice_command);
    void roll_owod(dice_roll& dice, std::string const& str_dice_command);

    void roll_fate(dice_roll& dice);
    void roll_fate(dice_roll& dice, int const i_val);
    void roll_fate(dice_roll& dice, std::string const& str_dice_command);
} // namespace dicebot::roll