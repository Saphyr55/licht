#pragma once

#include "licht/core/defines.hpp"

namespace licht {

template <typename ElementType, size_t Capacity>
class FixedArray {
public:
    using value_type = ElementType;
    using size_type = size_t;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using iterator = ElementType*;
    using const_iterator = const ElementType*;

public:
    inline constexpr iterator begin() {
        return data_;
    }

    inline constexpr const_iterator begin() const {
        return data_;
    }

    inline constexpr const_iterator cbegin() const {
        return data_;
    }

    inline constexpr iterator end() {
        return data_ + Capacity;
    }

    inline constexpr const_iterator end() const {
        return data_ + Capacity;
    }

    inline constexpr const_iterator cend() const {
        return data_ + Capacity;
    }

public:
    inline constexpr size_t size() const {
        return Capacity;
    }

    inline constexpr bool empty() const {
        return size() == 0;
    }

    inline constexpr const_reference operator[](size_t index) const {
        LCHECK_MSG(index < Capacity, "Index out of bounds.");
        return data_[index];
    }

    inline reference operator[](size_t index) {
        LCHECK_MSG(index < Capacity, "Index out of bounds.");
        return data_[index];
    }

    inline const value_type* data() const {
        return data_;
    }

    inline value_type* data()  {
        return data_;
    }

public:
    constexpr FixedArray() = default;

    constexpr FixedArray(std::initializer_list<ElementType> init) {
        LCHECK_MSG(init.size() <= Capacity, "Initializer list size exceeds static array capacity.");
        size_t i = 0;
        for (const ElementType& item : init) {
            new (data_ + i++) ElementType(item);
        }
    }

    constexpr FixedArray(const FixedArray& other) {
        for (size_t i = 0; i < Capacity; i++) {
            new (data_ + i) ElementType(other.data_[i]);
        }
    }

    constexpr FixedArray(FixedArray&& other) noexcept {
        for (size_t i = 0; i < Capacity; i++) {
            new (data_ + i) ElementType(std::move(other.data_[i]));
        }
    }

    constexpr FixedArray& operator=(const FixedArray& other) {
        if (this != &other) {
            for (size_t i = 0; i < Capacity; i++) {
                data_[i].~ElementType();
                new (data_ + i) ElementType(other.data_[i]);
            }
        }
        return *this;
    }

    constexpr FixedArray& operator=(FixedArray&& other) noexcept {
        if (this != &other) {
            for (size_t i = 0; i < Capacity; i++) {
                data_[i].~ElementType();
                new (data_ + i) ElementType(std::move(other.data_[i]));
            }
        }
        return *this;
    }

    ~FixedArray() = default;

private:
    ElementType data_[Capacity];
};

}  //namespace licht