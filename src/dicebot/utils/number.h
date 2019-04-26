#pragma once
#include <cmath>
#include <cstdint>
#include <exception>
#include <string>
/*
** 2018-12
** by dynilath
** an number class that auto-adjust between float_type and integer
*/
namespace dicebot {

    using integer_type = int32_t;
    using float_type = double;

    constexpr integer_type P_INT32_MIN = -(INT32_MAX);
    constexpr float_type ZERO_THRESHOLD = 1e-4;

    class zero_divider_exception : public std::exception {
    public:
        zero_divider_exception() : exception() {}
    };

    union number_val {
        integer_type i_value;
        float_type f_value;
    };

    class number {
    private:
        number_val value;

        void initialize(const std::string &source) {
            this->is_using_int = false;
            if (source.back() == '%') {
                this->value.f_value = std::stof(source.substr(0, source.size() - 1)) / 100;
            } else
                this->value.f_value = std::stof(source);

            this->try_to_be_integer();
        }

        void int_2_float() {
            if (!this->is_using_int) return;
            this->value.f_value = static_cast<float_type>(this->value.i_value);
            this->is_using_int = false;
        }

        void float_2_int() {
            if (this->is_using_int) return;
            this->value.i_value = (integer_type)this->value.f_value;
            this->is_using_int = true;
        }

        inline void try_to_be_integer() {
            if (!this->is_using_int) {
                if (this->value.f_value < INT32_MAX && this->value.f_value > P_INT32_MIN) {
                    integer_type new_val = std::lround(this->value.f_value);
                    float_type diff = this->value.f_value - new_val;
                    this->is_using_int = (diff < ZERO_THRESHOLD && diff > -ZERO_THRESHOLD);
                    if (this->is_using_int) this->value.i_value = new_val;
                }
            }
        }

    public:
        bool is_using_int;

        number() {
            this->value.i_value = 0;
            this->is_using_int = true;
        }

        number(const integer_type source) {
            this->value.i_value = source;
            this->is_using_int = true;
        }

        number(const float_type source) {
            this->value.f_value = source;
            this->is_using_int = false;
        }

        number(const std::string &source) { initialize(source); }

        number(const char *source) {
            std::string str_source(source);
            initialize(str_source);
        }

        inline number operator+(const number &val1) const {
            if (val1.is_using_int) {
                return *this + val1.value.i_value;
            } else {
                return *this + val1.value.f_value;
            }
        }

        inline number operator+(const integer_type &val1) const {
            if (this->is_using_int
                && (val1 == 0 || (val1 > 0 && INT32_MAX - val1 > this->value.i_value)
                    || (val1 < 0 && P_INT32_MIN - val1 < this->value.i_value))) {
                return number(this->value.i_value + val1);
            } else {
                return *this + static_cast<float_type>(val1);
            }
        }

        inline number operator+(const float_type &val1) const {
            float_type value_this =
                this->is_using_int ? static_cast<float_type>(this->value.i_value) : this->value.f_value;
            number ret(value_this + val1);
            ret.try_to_be_integer();
            return ret;
        }

        inline number operator-(const number &val1) const {
            if (val1.is_using_int) {
                return *this - val1.value.i_value;
            } else {
                return *this - val1.value.f_value;
            }
        }

        inline number operator-(const integer_type val1) const {
            if (this->is_using_int
                && (val1 == 0 || (val1 > 0 && P_INT32_MIN + val1 < this->value.i_value)
                    || (val1 < 0 && INT32_MAX + val1 > this->value.i_value))) {
                return number(this->value.i_value - val1);
            } else {
                return *this - static_cast<float_type>(val1);
            }
        }

        inline number operator-(const float_type val1) const {
            float_type value_this =
                this->is_using_int ? static_cast<float_type>(this->value.i_value) : this->value.f_value;
            number ret(value_this - val1);
            ret.try_to_be_integer();
            return ret;
        }

        inline number operator*(const number &val1) const {
            if (val1.is_using_int) {
                return (*this) * (val1.value.i_value);
            } else {
                return (*this) * (val1.value.f_value);
            }
        }

        inline number operator*(const integer_type val1) const {
            if (this->is_using_int
                && (val1 == 0 || this->value.i_value == 0
                    || (val1 > 0 && this->value.i_value > 0 && INT32_MAX / val1 > this->value.i_value)
                    || (val1 < 0 && this->value.i_value < 0 && INT32_MAX / val1 < this->value.i_value)
                    || (val1 < 0 && this->value.i_value > 0 && P_INT32_MIN / val1 > this->value.i_value)
                    || (val1 > 0 && this->value.i_value < 0 && P_INT32_MIN / val1 < this->value.i_value))) {
                return number(this->value.i_value * val1);
            } else {
                return (*this) * static_cast<float_type>(val1);
            }
        }

        inline number operator*(const float_type val1) const {
            float_type value_this =
                this->is_using_int ? static_cast<float_type>(this->value.i_value) : this->value.f_value;
            number ret(value_this * val1);
            ret.try_to_be_integer();
            return ret;
        }

        inline number operator/(const number &val1) const {
            if ((val1.is_using_int && val1.value.i_value == 0)
                || (!val1.is_using_int && val1.value.f_value < ZERO_THRESHOLD
                    && val1.value.f_value > -ZERO_THRESHOLD)) {
                throw zero_divider_exception();
            }

            if (val1.is_using_int) {
                return *this / val1.value.i_value;
            } else {
                return *this / val1.value.f_value;
            }
        }

        inline number operator/(const integer_type val1) const {
            if (val1 == 0) {
                throw zero_divider_exception();
            }

            if (this->is_using_int && this->value.i_value % val1 == 0) {
                return number(this->value.i_value / val1);
            } else {
                return (*this) / static_cast<float_type>(val1);
            }
        }

        inline number operator/(const float_type val1) const {
            if (val1 < ZERO_THRESHOLD && val1 > -ZERO_THRESHOLD) {
                throw zero_divider_exception();
            }

            float_type value_this =
                this->is_using_int ? static_cast<float_type>(this->value.i_value) : this->value.f_value;
            number ret(value_this / val1);
            ret.try_to_be_integer();
            return ret;
        }

        inline bool operator==(const number &val1) const {
            if (val1.is_using_int)
                return *this == val1.value.i_value;
            else
                return *this == val1.value.f_value;
        }

        inline bool operator==(const integer_type val1) const {
            if (this->is_using_int)
                return this->value.i_value == val1;
            else
                return this->value.i_value == val1;
        }

        inline bool operator==(const float_type val1) const {
            if (this->is_using_int)
                return this->value.i_value == val1;
            else
                return this->value.i_value == val1;
        }

        std::string str() const {
            if (this->is_using_int)
                return std::to_string(this->value.i_value);
            else {
                std::string ret = std::to_string(this->value.f_value);
                int pos = ret.find_last_not_of('0');
                if (pos != std::string::npos) ret.assign(ret.substr(0, pos + 1));
                return ret;
            }
        }

        inline integer_type force_positive_int() {
            if (this->is_using_int) {
                return this->value.i_value > 0 ? this->value.i_value : 0;
            } else {
                return this->value.f_value > 0 ? static_cast<integer_type>(this->value.f_value) : 0;
            }
        }
    };

    const number zero = 0;
} // namespace dicebot
