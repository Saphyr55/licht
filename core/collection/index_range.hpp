#pragma once

#include <concepts>
#include <cstddef>
#include <iterator>

#include "core/core_exports.hpp"
#include "core/defines.hpp"

namespace licht {

template <std::integral IntegerType = size_t>
class LICHT_CORE_API IndexRange {
public:
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = IntegerType;
        using difference_type = IntegerType;
        using pointer = IntegerType*;
        using reference = IntegerType&;

        constexpr explicit Iterator(IntegerType index)
            : index_(index) {}

        constexpr IntegerType operator*() const { return index_; }

        constexpr Iterator& operator++() {
            ++index_;
            return *this;
        }

        constexpr Iterator operator++(int) {
            Iterator temp = *this;
            ++index_;
            return temp;
        }

        constexpr bool operator==(const Iterator& other) const {
            return index_ == other.index_;
        }

        constexpr bool operator!=(const Iterator& other) const {
            return index_ != other.index_;
        }

    private:
        IntegerType index_;
    };

    constexpr IndexRange(IntegerType start, IntegerType end)
        : start_(start), end_(end) {
        LCHECK(start <= end)
    }

    constexpr Iterator begin() const noexcept { return Iterator(start_); }

    constexpr Iterator end() const noexcept { return Iterator(end_); }

    constexpr const Iterator cbegin() const noexcept { return begin(); }

    constexpr const Iterator cend() const noexcept { return end(); }

    constexpr size_t Size() const noexcept { return end_ - start_; }

private:
    IntegerType start_;
    IntegerType end_;
};

}  // namespace licht