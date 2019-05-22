#include "./dice_roller.h"

#include <chrono>
#include <random>
#include <regex>
#include <sstream>

#include "./utils/utils.h"

#include "./random/random_provider.h"

using namespace dicebot;
using namespace dicebot::roll;

size_t dice_roll::add_result(int32_t const result) {
    this->results.push_back(dice_pair(result, true));
    return this->results.size();
}

size_t dice_roll::add_ignored_result(int32_t const result) {
    this->results.push_back(dice_pair(result, false));
    return this->results.size();
}

size_t dice_roll::update_result(int32_t const result, size_t const target) {
    if (target < this->results.size()) {
        this->results[target].first = result;
    }
    return results.size();
}

size_t dice_roll::set_ignore(size_t const target) {
    if (this->results[target].second) {
        this->results[target].second = false;
    }
    return this->results.size();
}

size_t dice_roll::set_good(size_t target) {
    if (!this->results[target].second) {
        this->results[target].second = true;
    }
    return this->results.size();
}

size_t dice_roll::clear() {
    this->status = roll_status::UNINITIALIZED;
    this->results.clear();
    this->summary = 0;
    return this->results.size();
}

dice_roll::dice_roll() noexcept {
    this->status = roll_status::UNINITIALIZED;
    this->summary = 0;
}

roll_status dice_roll::finish_roll() noexcept {
    for (uint16_t i = 0; i < this->results.size(); i++) {
        if (this->results[i].second) this->summary += this->results[i].first;
    }
    return this->status = roll_status::FINISHED;
}

std::string dice_roll::detail() {
    std::ostringstream ost;
    ost << "[";
    for (uint16_t i = 0; i < this->results.size(); i++) {
        dice_pair dp = results[i];
        if (!dp.second)
            ost << dp.first << "*";
        else
            ost << dp.first;
        if (i != results.size() - 1) ost << " + ";
    }
    ost << "]";
    return ost.str();
}

roll_status dice_roll::finish_coc() noexcept {
    if (this->results.empty())
        return this->status = roll_status::UNINITIALIZED;
    else if (this->results.size() == 1) {
        this->summary = this->results[0].first;
    } else {
        uint16_t units = this->results[0].first;
        uint16_t tens = 0;
        for (uint16_t i = 1; i < this->results.size(); i++) {
            if (this->results[i].second) {
                tens = this->results[i].first;
                break;
            }
        }
        this->summary = units + tens * 10;
    }
    return this->status = roll_status::FINISHED;
}

std::string dice_roll::detail_coc() noexcept {
    std::ostringstream ost;
    if (this->results.size() > 1) {
        ost << "[";
        for (uint16_t i = 1; i < this->results.size(); i++) {
            dice_pair dp = results[i];
            if (!dp.second)
                ost << dp.first << "*";
            else
                ost << dp.first;
            if (i != results.size() - 1) ost << " + ";
        }
        ost << "] [";
        ost << this->results[0].first;
        ost << "]";
    }
    return ost.str();
}

roll_status dice_roll::finish_wod(int const i_d, bool const failing) noexcept {
    int penalty = 0;
    for (uint16_t i = 0; i < this->results.size(); i++) {
        if (this->results[i].first >= i_d) {
            this->results[i].second = true;
            this->summary++;
        } else if (failing && this->results[i].first == 1) {
            penalty++;
        }
    }
    if (this->summary > penalty)
        this->summary -= penalty;
    else
        this->summary = 0;
    return this->status = roll_status::FINISHED;
}

std::string dice_roll::detail_fate() noexcept {
    std::ostringstream ost;
    ost << "[";
    for (uint16_t i = 0; i < 4; i++) {
        dice_pair dp = results[i];
        switch (dp.first) {
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
        int val = this->results[4].first;
        if (val < 0)
            ost << "] - ";
        else
            ost << "] + ";
        val = val > 0 ? val : -val;
        ost << val;
    } else
        ost << "]";
    return ost.str();
}

roll_status dice_roll::dice_exceed() noexcept {
    return this->status = roll_status::TOO_MANY_DICE;
}

roll_status dice_roll::general_err() noexcept {
    return this->status = roll_status::GENERAL_ERR;
}

dice_roll::dice_roll(roll_status const& stat) noexcept { this->status = stat; }

dice_roll::operator bool() const noexcept {
    return this->status == roll_status::FINISHED;
}

roll_status roll::roll_base(dice_roll& dice, int const i_num_of_dice,
                            int const i_num_of_face) noexcept {
    dice.clear();

    auto distr = random::create_distribution(1, i_num_of_face);
    if (CHECK_LIMITS(i_num_of_dice, i_num_of_face)) {
        uint16_t single_result = 0;
        bool first = true;
        int i_dice = i_num_of_dice;
        while (i_dice--) {
            dice.add_result(random::rand_int(distr));
        }
        return dice.finish_roll();
    } else {
        return dice.dice_exceed();
    }
}

roll_status roll::roll_rdk(dice_roll& dice, int const i_num_of_dice,
                           int const i_num_of_face, int const i_keep) noexcept {
    dice.clear();

    if (i_keep == 0) {
        return dice.general_err();
    }
    int i_num_of_keep = i_keep > 0 ? i_keep : (-i_keep);
    if (i_num_of_keep >= i_num_of_dice)
        return roll::roll_base(dice, i_num_of_dice, i_num_of_face);

    auto distr = random::create_distribution(1, i_num_of_face);

    if (!CHECK_LIMITS(i_num_of_dice, i_num_of_face)) {
        return dice.dice_exceed();
    }

    int i_result = 0;

    std::vector<int> resultList;
    std::vector<int> sortList;
    std::vector<int> pilotList;
    std::vector<bool> flagList;

    for (uint16_t i_count = 0; i_count < i_num_of_dice; i_count++) {
        uint16_t i_temp_result = random::rand_int(distr);
        dice.add_ignored_result(i_temp_result);
        pilotList.push_back(i_count);
        sortList.push_back(i_temp_result);
    }

    flagList.assign(sortList.size(), 0);
    utils::quick_sort(
        sortList.data(), pilotList.data(), 0, sortList.size() - 1);

    if (i_keep < 0) {
        for (int i_iter = 0; i_iter < i_num_of_keep; i_iter++) {
            dice.set_good(pilotList[i_iter]);
        }
    } else {
        for (int i_iter = 1; i_iter <= i_num_of_keep; i_iter++) {
            dice.set_good(pilotList[pilotList.size() - i_iter]);
        }
    }
    return dice.finish_roll();
}

roll_status roll::roll_rdk(dice_roll& dice,
                           std::string const& str_dice_command) noexcept {
    dice.clear();

    std::string source(str_dice_command);

    std::regex regex_rd("(\\d+)?[d](\\d+)(?:k(l)?(\\d+))?",
                        std::regex_constants::icase);

    std::smatch smatch_rd;
    std::regex_match(str_dice_command, smatch_rd, regex_rd);

    if (smatch_rd.empty()) return dice.general_err();

    int i_num_of_die = 1;
    int i_num_of_face = 0;
    int i_num_of_keep = 0;
    bool is_keep = false;
    bool is_keeping_high = true;
    try {
        if (smatch_rd[1].matched) i_num_of_die = std::stoi(smatch_rd[1].str());
        if (smatch_rd[2].matched)
            i_num_of_face = std::stoi(smatch_rd[2].str());
        else
            return dice.general_err();
        if (smatch_rd[3].matched) is_keeping_high = false;
        if (smatch_rd[4].matched) {
            is_keep = true;
            i_num_of_keep = std::stoi(smatch_rd[4].str());
            if (!is_keeping_high) i_num_of_keep = -i_num_of_keep;
        }

        if (!CHECK_LIMITS(i_num_of_die, i_num_of_face))
            return dice.dice_exceed();

        if (is_keep) {
            return roll::roll_rdk(
                dice, i_num_of_die, i_num_of_face, i_num_of_keep);
        }
        return roll::roll_base(dice, i_num_of_die, i_num_of_face);
    } catch (const std::invalid_argument& ia) {
#ifdef _DEBUG
        logger::log("dice_roller", ia.what());
#endif
        return dice.general_err();
    }
}

roll_status roll::roll_coc(dice_roll& dice, int const i_bp) noexcept {
    dice.clear();
    if (i_bp == 0) {
        return roll::roll_base(dice, 1, 100);
    } else {
        uint16_t i_dice_count = i_bp > 0 ? (1 + i_bp) : (1 - i_bp);

        if (!CHECK_LIMITS(i_dice_count, 100)) return dice.dice_exceed();

        auto units_distr = random::create_distribution(0, 9);
        auto tens_distr = random::create_distribution(1, 10);

        uint16_t i_units = random::rand_int(units_distr);

        dice.add_result(i_units);

        if (i_units == 0) {
            for (uint16_t i_count = 0; i_count < i_dice_count; i_count++) {
                uint16_t i_temp_result = random::rand_int(tens_distr);
                dice.add_ignored_result(i_temp_result);
            }

        } else {
            for (uint16_t i_count = 0; i_count < i_dice_count; i_count++) {
                uint16_t i_temp_result = random::rand_int(units_distr);
                dice.add_ignored_result(i_temp_result);
            }
        }
        uint16_t i_target = 1;
        if (i_bp > 0) {
            for (uint16_t i = 1; i < dice.results.size(); i++) {
                if (dice.results[i].first < dice.results[i_target].first) {
                    i_target = i;
                }
            }
        } else if (i_bp < 0) {
            for (uint16_t i = 1; i < dice.results.size(); i++) {
                if (dice.results[i].first > dice.results[i_target].first) {
                    i_target = i;
                }
            }
        }
        dice.results[i_target].second = true;

        return dice.finish_coc();
    }
}

roll_status roll::roll_coc(dice_roll& dice,
                           std::string const& str_dice_command) noexcept {
    dice.clear();
    try {
        std::string source(str_dice_command);
        std::regex regex_pb("^([bp])(\\d+)", std::regex_constants::icase);
        std::smatch smatch_coc;
        int i_bp_count = 0;
        while (true) {
            std::regex_search(source, smatch_coc, regex_pb);
            if (smatch_coc.empty()) break;
            int this_bp = std::stoi(smatch_coc[2].str());

            if (smatch_coc[1].str()[0] == 'p'
                || smatch_coc[1].str()[0] == 'P') {
                i_bp_count -= this_bp;
            } else {
                i_bp_count += this_bp;
            }
            source.assign(smatch_coc.suffix().str());
            if (source.length() < 2) break;
        }
        return roll::roll_coc(dice, i_bp_count);
    } catch (const std::invalid_argument& ia) {
#ifdef _DEBUG
        logger::log("dice_roller", ia.what());
#endif
        return dice.general_err();
    }
}

roll_status roll::roll_wod(dice_roll& dice, int const i_val, int const i_d,
                           int const i_bonus, bool failing) noexcept {
    dice.clear();

    auto distr = random::create_distribution(1, 10);
    if (CHECK_LIMITS(i_val, 10)) {
        uint16_t single_result = 0;
        bool first = true;
        int i_dice = i_val;
        while (i_dice-- > 0) {
            single_result = random::rand_int(distr);
            if (single_result >= i_bonus) i_dice++;
            if (dice.results.size() > MAX_DICE_NUM) break;
            dice.add_ignored_result(single_result);
        }
        return dice.finish_wod(i_d, failing);
    } else {
        return dice.dice_exceed();
    }
}

roll_status roll::roll_nwod(dice_roll& dice,
                            std::string const& str_dice_command) noexcept {
    dice.clear();
    try {
        std::string source(str_dice_command);
        std::regex regex_pb("^(\\d+)(?:[dD](\\d+))?(?:[bB](\\d+))?");
        std::smatch smatch_coc;
        std::regex_search(source, smatch_coc, regex_pb);
        if (smatch_coc.empty()) return dice.general_err();

        uint16_t i_dice = std::stoi(smatch_coc[1].str());
        uint16_t i_diff = 8;
        if (smatch_coc[2].matched) i_diff = std::stoi(smatch_coc[2].str());
        uint16_t i_bonus = 10;
        if (smatch_coc[3].matched) i_bonus = std::stoi(smatch_coc[3].str());
        if (i_bonus < 6) i_bonus = 10;
        return roll::roll_wod(dice, i_dice, i_diff, i_bonus, false);
    } catch (const std::invalid_argument& ia) {
#ifdef _DEBUG
        logger::log("roll_nwod", ia.what());
#endif
        return dice.general_err();
    }
}

roll_status roll::roll_owod(dice_roll& dice,
                            std::string const& str_dice_command) noexcept {
    dice.clear();
    try {
        std::string source(str_dice_command);
        std::regex regex_pb("^(\\d+)(?:[dD](\\d+))?(?:[bB](\\d+))?");
        std::smatch smatch_coc;
        std::regex_search(source, smatch_coc, regex_pb);
        if (smatch_coc.empty()) return dice.general_err();

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
        return dice.general_err();
    }
}

roll_status roll::roll_fate(dice_roll& dice) noexcept {
    dice.clear();
    auto distr = random::create_distribution(-1, 1);
    int16_t single_result = 0;
    bool first = true;
    int i_dice = 4;
    while (i_dice-- > 0) {
        single_result = random::rand_int(distr);
        dice.add_result(single_result);
    }
    return dice.finish_roll();
}

roll_status roll::roll_fate(dice_roll& dice, int const i_val) noexcept {
    dice.clear();

    auto distr = random::create_distribution(-1, 1);
    int16_t single_result = 0;
    bool first = true;
    int i_dice = 4;
    while (i_dice-- > 0) {
        single_result = random::rand_int(distr);
        dice.add_result(single_result);
    }
    dice.add_result(i_val);
    return dice.finish_roll();
}

roll_status roll::roll_fate(dice_roll& dice,
                            std::string const& str_dice_command) noexcept {
    dice.clear();
    try {
        std::string source(str_dice_command);
        std::regex regex_d("^([\\+\\-]\\d+)");
        std::smatch smatch_fate;
        std::regex_search(source, smatch_fate, regex_d);
        if (smatch_fate.empty()) return dice.general_err();

        int16_t i_val = std::stoi(smatch_fate[1].str());
        return roll::roll_fate(dice, i_val);
    } catch (const std::invalid_argument& ia) {
#ifdef _DEBUG
        logger::log("roll_fate", ia.what());
#endif
        return dice.general_err();
    }
}