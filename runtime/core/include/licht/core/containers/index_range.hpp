#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

#include <concepts>
#include <cstddef>
#include <iterator>

namespace licht {

template <std::integral IndexType = size_t>
class IndexRange {
public:
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = IndexType;
        using difference_type = IndexType;
        using pointer = IndexType*;
        using reference = IndexType&;

        constexpr explicit Iterator(IndexType index)
            : index_(index) {}

        constexpr value_type operator*() const { return index_; }

        constexpr Iterator operator++() {
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
        IndexType index_;
    };

    constexpr IndexRange(IndexType start, IndexType end)
        : start_(start), end_(end) {
        LCHECK(start <= end)
    }

    constexpr IndexRange(const auto& start, const auto& end)
        : start_(static_cast<IndexType>(start)), end_(static_cast<IndexType>(end)) {
        LCHECK(start <= end)
    }

    constexpr Iterator begin() const noexcept { return Iterator(start_); }

    constexpr Iterator end() const noexcept { return Iterator(end_); }

    constexpr const Iterator cbegin() const noexcept { return begin(); }

    constexpr const Iterator cend() const noexcept { return end(); }

    constexpr IndexType Size() const noexcept { return end_ - start_; }

private:
    IndexType start_;
    IndexType end_;
};


}  // namespace licht