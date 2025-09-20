#pragma once

#include "licht/core/defines.hpp"

namespace licht {

template <typename ElementType, size_t Capacity>
class StaticArray {
public:
    using IteratorType = ElementType*;
    using value_type = ElementType;
    using size_type = size_t;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using iterator = ElementType*;
    using const_iterator = const ElementType*;

public:
    inline constexpr IteratorType begin() {
        return data_;
    }

    inline constexpr const IteratorType begin() const {
        return data_;
    }

    inline constexpr const IteratorType cbegin() const {
        return data_;
    }

    inline constexpr IteratorType end() {
        return data_ + Capacity;
    }

    inline constexpr const IteratorType end() const {
        return data_ + Capacity;
    }

    inline constexpr const IteratorType cend() const {
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

    inline value_type data() const {
        return data_;
    }

public:
    constexpr StaticArray() = default;

    constexpr StaticArray(std::initializer_list<ElementType> init) {
        LCHECK_MSG(init.size() <= Capacity, "Initializer list size exceeds static array capacity.");
        size_t i = 0;
        for (const ElementType& item : init) {
            new (data_ + i++) ElementType(item);
        }
    }

    constexpr StaticArray(const StaticArray& other) {
        for (size_t i = 0; i < Capacity; i++) {
            new (data_ + i) ElementType(other.data_[i]);
        }
    }

    constexpr StaticArray(StaticArray&& other) noexcept {
        for (size_t i = 0; i < Capacity; i++) {
            new (data_ + i) ElementType(std::move(other.data_[i]));
        }
    }

    constexpr StaticArray& operator=(const StaticArray& other) {
        if (this != &other) {
            for (size_t i = 0; i < Capacity; i++) {
                data_[i].~ElementType();
                new (data_ + i) ElementType(other.data_[i]);
            }
        }
        return *this;
    }

    constexpr StaticArray& operator=(StaticArray&& other) noexcept {
        if (this != &other) {
            for (size_t i = 0; i < Capacity; i++) {
                data_[i].~ElementType();
                new (data_ + i) ElementType(std::move(other.data_[i]));
            }
        }
        return *this;
    }

    ~StaticArray() = default;

private:
    ElementType data_[Capacity];
};

}  //namespace licht