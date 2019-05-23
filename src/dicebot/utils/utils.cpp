#include "./utils.h"
#include <algorithm>
#include <vector>
using namespace dicebot;

void utils::split_line(std::string const& source,
                       std::list<std::string>& source_splits) {
    if (source.length() > 2) {
        std::string::size_type iter_source = 0;
        std::string spliter = "\r\n";
        while (iter_source < source.length()) {
            std::string::size_type i_target = source.find(spliter, iter_source);
            if (i_target == std::string::npos) {
                if (i_target > iter_source)
                    source_splits.push_back(source.substr(
                        iter_source, source.length() - iter_source));
                break;
            } else {
                if (i_target > iter_source)
                    source_splits.push_back(
                        source.substr(iter_source, i_target - iter_source));
                iter_source = i_target + spliter.length();
            }
        }
    } else if (source.length() > 0) {
        source_splits.push_back(source);
    }
}

constexpr char new_line[] = "\r\n";
constexpr size_t new_line_len = 2;
void utils::split_line_part(
    std::string const& source,
    std::list<std::pair<size_t, size_t>>& source_splits) {
    size_t o_pos = 0;
    size_t f_pos;
    while (o_pos < source.size()) {
        f_pos = source.find(new_line, o_pos);
        if (f_pos == std::string::npos) {
            source_splits.push_back({o_pos, source.size() - 1});
            return;
        }
        if (f_pos > o_pos) source_splits.push_back({o_pos, f_pos});
        o_pos = f_pos + new_line_len;
    }
}

constexpr char trim_chars[] = " \t";
bool utils::trim_part(std::string const& source,
                      std::pair<size_t, size_t>& part) {
    part.first = source.find_first_not_of(trim_chars, part.first);
    if (part.first > part.second) return false;
    part.second = source.find_last_not_of(trim_chars, part.second);
}

void utils::remove_space_and_tab(std::string& source) {
    std::vector<char> container;
    container.reserve(source.size() + 1);
    for (auto& item : source) {
        if (item == ' ' || item == '\t') continue;
        container.push_back(item);
    }
    container.push_back('\0');
    source.assign(container.data());
}

void utils::quick_sort(int* origin, int* pilot, int first, int last) {
    if (last <= first) return;

    int pivot = origin[first];
    int pivot_pilot = pilot[first];
    int left = first + 1;
    int right = last;
    int temp;
    while (left <= right) {
        while (origin[right] > pivot && right >= left) right--;

        while (origin[left] < pivot && left <= right) left++;

        if (left >= right) break;

        temp = origin[left];
        origin[left] = origin[right];
        origin[right] = temp;
        temp = pilot[left];
        pilot[left] = pilot[right];
        pilot[right] = temp;
        left++, right--;
    }
    origin[first] = origin[right];
    origin[right] = pivot;

    pilot[first] = pilot[right];
    pilot[right] = pivot_pilot;

    dicebot::utils::quick_sort(origin, pilot, first, right - 1);
    dicebot::utils::quick_sort(origin, pilot, left, last);
}

bool utils::find_point(std::string const& source, size_t& content_begin) {
    size_t point = 0;
    for (;; point++) {
        if (point == source.size())
            return false;
        else if (source[point] == ' ')
            continue;
        else if (source[point] == '.')
            break;
        else
            return false;
    }
    content_begin = point + 1;
    for (; content_begin < source.size(); content_begin++) {
        if (source[content_begin] == ' ')
            continue;
        else
            return true;
    }
    return false;
}

bool utils::find_space(std::string const& source, size_t& pos) {
    pos = 0;
    do {
        if (pos == source.length())
            return false;
        else if (source[pos] == ' ')
            return true;
    } while (++pos);
    return false;
}

void utils::lower_case(std::string& target) {
    std::transform(target.cbegin(), target.cend(), target.begin(), tolower);
}

std::string utils::lower_case_copy(const std::string& target) {
    std::vector<char> container;
    container.reserve(target.size() + 1);
    std::transform(
        target.cbegin(), target.cend(), std::back_inserter(container), tolower);
    container.push_back('\0');
    return std::string(container.data());
}