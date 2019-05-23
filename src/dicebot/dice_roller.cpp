#include "./dice_roller.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <regex>
#include <sstream>

#include "./dice_excepts.h"
#include "./random/random_provider.h"
#include "./utils/utils.h"

using namespace dicebot;
using namespace dicebot::roll;

static auto check_limits = [](int num, int face) {
    if (num > MAX_DICE_NUM) throw dice_exceed();
    if (face > MAX_DICE_FACE) throw face_exceed();
    if (face < 1 || num < 1) throw invalid_dice();
};

size_t dice_roll::add_result(int const result) {
    this->results.push_back(result);
    return this->results.size();
}

size_t dice_roll::add_ignored_result(int const result) {
    this->results.push_back(result);
    return this->results.size();
}

size_t dice_roll::update_result(int const result, size_t const target) {
    this->results[target] = result;
    return results.size();
}

size_t dice_roll::set_ignore(size_t const target) {
    this->flags[target] = false;
    return this->results.size();
}

size_t dice_roll::set_good(size_t target) {
    this->flags[target] = true;
    return this->results.size();
}

void dice_roll::clear() noexcept {
    this->results.clear();
    this->summary = 0;
}

dice_roll::dice_roll() noexcept { this->summary = 0; }

void dice_roll::finish_roll(bool ignore_flag = false) noexcept {
    std::accumulate(this->results.begin(), this->results.end(), 0);
    for (size_t i = 0; i < this->results.size(); i++) {
        if (ignore_flag || this->flags[i]) this->summary += this->results[i];
    }
}

std::string dice_roll::detail() {
    std::ostringstream ost;
    ost << "[";
    if (this->flags.empty()) {
        std::for_each(this->results.begin(), this->results.end() - 1, [&ost](int a) { ost << a << " + "; });
        ost << this->results.back();
    } else {
        utils::repeat(this->results.size() - 1, [&ost, this](size_t pos) {
            ost << this->results[pos];
            if (!this->flags[pos]) ost << "*";
            ost << " + ";
        });
        ost << this->results.back();
        if (!this->flags.back()) ost << "*";
    }
    ost << "]";
    return ost.str();
}

void dice_roll::finish_coc() noexcept {
    if (this->results.size() == 1) {
        this->summary = this->results[0];
    } else {
        uint16_t units = this->results[0];
        uint16_t tens = 0;
        for (uint16_t i = 1; i < this->results.size(); i++) {
            if (this->flags[i]) {
                tens = this->results[i];
                break;
            }
        }
        this->summary = units + tens * 10;
    }
}

std::string dice_roll::detail_coc() noexcept {
    std::ostringstream ost;
    if (this->results.size() > 1) {
        ost << "[";
        for (uint16_t i = 1; i < this->results.size(); i++) {
            if (this->flags[i])
                ost << this->results[i];
            else
                ost << this->results[i] << "*";
            if (i != results.size() - 1) ost << " + ";
        }
        ost << "] [";
        ost << this->results[0];
        ost << "]";
    }
    return ost.str();
}

void dice_roll::finish_wod(int const i_d, bool const failing) noexcept {
    int penalty = 0;
    for (uint16_t i = 0; i < this->results.size(); i++) {
        if (this->results[i] >= i_d) {
            this->flags[i] = true;
            this->summary++;
        } else if (failing && this->results[i] == 1) {
            penalty++;
        }
    }
    if (this->summary > penalty)
        this->summary -= penalty;
    else
        this->summary = 0;
}

std::string dice_roll::detail_fate() noexcept {
    std::ostringstream ost;
    ost << "[";
    for (uint16_t i = 0; i < 4; i++) {
        switch (this->results[i]) {
        case 1:
            ost << "+";
            break;
        case -1:
            ost << "-";
            break;
        case 0:
            ost << "o";
            break;
        default:
            break;
        }
        if (i != 3) ost << " ";
    }
    if (this->results.size() > 4) {
        if (this->results[4] < 0)
            ost << "] - " << -this->results[4];
        else
            ost << "] + " << this->results[4];
    } else
        ost << "]";
    return ost.str();
}

void roll::roll_base(dice_roll& dice, int const i_dice, int const i_face) {
    dice.clear();
    check_limits(i_dice, i_face);
    auto distr = random::create_distribution(1, i_face);
    uint16_t single_result = 0;
    bool first = true;
    dice.results.resize(i_dice);

    std::generate(dice.results.begin(), dice.results.end(), [&distr]() -> int { return random::rand_int(distr); });

    dice.finish_roll(true);
}

void roll::roll_rdk(dice_roll& dice, int const i_dice, int const i_face, int const i_keep) {
    int i_num_of_keep = i_keep > 0 ? i_keep : (-i_keep);
    bool is_keep_high = i_keep > 0;
    if (i_num_of_keep >= i_dice) {
        roll::roll_base(dice, i_dice, i_face);
        return;
    }

    check_limits(i_dice, i_face);

    auto distr = random::create_distribution(1, i_face);

    dice.results.resize(i_dice);
    dice.flags.resize(i_dice, false);

    std::generate(dice.results.begin(), dice.results.end(), [&distr]() -> int { return random::rand_int(distr); });

    auto pilot = utils::pilot_sort(dice.results);

    if (is_keep_high) {
        std::for_each(pilot.rbegin(), pilot.rbegin() + i_num_of_keep, [&dice](size_t a) { dice.flags[a] = true; });
    } else {
        std::for_each(pilot.begin(), pilot.begin() + i_num_of_keep, [&dice](size_t a) { dice.flags[a] = true; });
    }
    dice.finish_roll();
}

void roll::roll_coc(dice_roll& dice, int const i_bp) {
    if (i_bp == 0) {
        roll::roll_base(dice, 1, 100);
        return;
    }

    size_t dice_count = i_bp > 0 ? (1 + i_bp) : (1 - i_bp);
    bool is_bonus = i_bp > 0;
    check_limits(dice_count, 100);

    auto units_distr = random::create_distribution(0, 9);
    auto tens_distr = random::create_distribution(1, 10);

    size_t i_units = random::rand_int(units_distr);

    dice.results.resize(dice_count);
    dice.flags.resize(dice_count, false);

    dice.results.front() = i_units;
    dice.add_result(i_units);

    auto work_point = dice.results.begin() + 1;
    auto end_point = dice.results.end();

    if (i_units == 0) {
        std::generate(work_point, end_point, [&tens_distr]() -> int { return random::rand_int(tens_distr); });
    } else {
        std::generate(work_point, end_point, [&units_distr]() -> int { return random::rand_int(units_distr); });
    }

    decltype(dice.results)::const_iterator target;
    if (is_bonus)
        target = std::min_element(work_point, end_point);
    else
        target = std::max_element(work_point, end_point);

    dice.flags[target - dice.results.begin()] = true;

    dice.finish_coc();
}

void roll::roll_wod(dice_roll& dice, int const i_val, int const i_d, int const i_bonus, bool failing) {
    dice.clear();

    auto distr = random::create_distribution(1, 10);

    check_limits(i_val, 10);

    uint16_t single_result = 0;
    bool first = true;
    int i_dice = i_val;

    dice.results.reserve(i_val);
    dice.flags.reserve(i_val);

    while (i_dice--) {
        single_result = random::rand_int(distr);
        if (single_result >= i_bonus) i_dice++;
        if (dice.results.size() > MAX_DICE_NUM) break;
        dice.add_ignored_result(single_result);
    }
    dice.finish_wod(i_d, failing);
}

void roll::roll_nwod(dice_roll& dice, std::string const& str_dice_command) {
    dice.clear();
    try {
        std::string source(str_dice_command);
        std::regex regex_pb("^(\\d+)(?:[dD](\\d+))?(?:[bB](\\d+))?");
        std::smatch smatch_coc;
        std::regex_search(source, smatch_coc, regex_pb);
        if (smatch_coc.empty()) return;

        uint16_t i_dice = std::stoi(smatch_coc[1]);
        uint16_t i_diff = 8;
        if (smatch_coc[2].matched) i_diff = std::stoi(smatch_coc[2]);
        uint16_t i_bonus = 10;
        if (smatch_coc[3].matched) i_bonus = std::stoi(smatch_coc[3]);
        if (i_bonus < 6) i_bonus = 10;
        return roll::roll_wod(dice, i_dice, i_diff, i_bonus, false);
    } catch (const std::invalid_argument& ia) {
#ifdef _DEBUG
        logger::log("roll_nwod", ia.what());
#endif
    }
}

void roll::roll_owod(dice_roll& dice, std::string const& str_dice_command) {
    dice.clear();
    try {
        std::string source(str_dice_command);
        std::regex regex_pb("^(\\d+)(?:[dD](\\d+))?(?:[bB](\\d+))?");
        std::smatch smatch_coc;
        std::regex_search(source, smatch_coc, regex_pb);
        if (smatch_coc.empty()) return;

        uint16_t i_dice = std::stoi(smatch_coc[1].str());
        uint16_t i_diff = 6;
        if (smatch_coc[2].matched) i_diff = std::stoi(smatch_coc[2].str());
        uint16_t i_bonus = 11;
        if (smatch_coc[3].matched) i_bonus = std::stoi(smatch_coc[3].str());
        if (i_bonus < 6) i_bonus = 10;
        return roll::roll_wod(dice, i_dice, i_diff, i_bonus, true);
    } catch (const std::invalid_argument& ia) {
#ifdef _DEBUG
        logger::log("roll_owod", ia.what());
#endif
    }
}

void roll::roll_fate(dice_roll& dice, int const i_val) {
    dice.clear();

    auto distr = random::create_distribution(-1, 1);
    int16_t single_result = 0;
    bool first = true;
    int i_dice = 4;
    while (i_dice-- > 0) {
        single_result = random::rand_int(distr);
        dice.add_result(single_result);
    }
    if (i_val != 0) dice.add_result(i_val);
    dice.finish_roll();
}