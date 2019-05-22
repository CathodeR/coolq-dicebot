#pragma once

#include <list>
#include <string>

namespace dicebot::utils {
    void split_line(std::string const& source,
                    std::list<std::string>& source_splits);
    void remove_space_and_tab(std::string& source);
    void quick_sort(int* origin, int* pilot, int first, int last);
    bool find_point(std::string const& source, size_t& content_begin);
    bool find_space(std::string const& source, size_t& space);
    void lower_case(std::string& target);
    std::string lower_case_copy(const std::string& target);

    template <class _Function>
    void repeat(size_t times, _Function func) {
        while (times--) func();
    }
} // namespace dicebot::utils