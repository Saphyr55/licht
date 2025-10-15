#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/fixed_array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/concepts.hpp"

namespace licht {

template <typename ElementType>
class ArrayView {
public:
    using value_type = ElementType;
    using size_type = size_t;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using iterator = ElementType*;
    using const_iterator = const ElementType*;

public:
    const_reference front() const {
        LCHECK_MSG(!empty(), "Call front with an empty array.");
        return data_[0];
    }

    reference front() {
        LCHECK_MSG(!empty(), "Call front with an empty array.");
        return data_[0];
    }

    const_reference back() const {
        LCHECK_MSG(!empty(), "Call front with an empty array.");
        return data_[size_ - 1];
    }

    reference back() {
        LCHECK_MSG(!empty(), "Call front with an empty array.");
        return data_[size_ - 1];
    }

    inline constexpr ElementType& operator[](size_type index) {
        LCHECK_MSG(index < size_, "Index out of bounds.");
        return data_[index];
    }

    inline constexpr const ElementType& operator[](size_type index) const {
        LCHECK_MSG(index < size_, "Index out of bounds.");
        return data_[index];
    }

    inline constexpr size_type size() const {
        return size_;
    }

    inline constexpr bool empty() const {
        return size() == 0;
    }

    inline const ElementType* data() const {
        return data_;
    }

    inline ElementType* data() {
        return const_cast<ElementType*>(data_);
    }

    bool contains(const ElementType& element) const {
        for (size_type i = 0; i < size_; ++i) {
            if (data_[i] == element) {
                return true;
            }
        }
        return false;
    }

    bool contains(const ElementType& element, auto&& predicate) const {
        for (size_type i = 0; i < size_; ++i) {
            if (predicate(data_[i], element)) {
                return true;
            }
        }
        return false;
    }

    ElementType* get_if(auto&& p_predicate) const {
        for (size_type i = 0; i < size_; ++i) {
            if (p_predicate(data_[i])) {
                return &data_[i];
            }
        }
        return nullptr;
    }

public:
    iterator begin() {
        return const_cast<iterator>(data_);
    }

    const_iterator begin() const {
        return data_;
    }

    const_iterator cbegin() const {
        return data_;
    }

    iterator end() {
        return const_cast<iterator>(data_) + size_;
    }

    const_iterator end() const {
        return data_ + size_;
    }

    const_iterator cend() const {
        return data_ + size_;
    }

public:
    ArrayView() = default;

    ArrayView(ElementType* data, size_type size)
        : data_(data), size_(size) {}

    template <CTypedAllocator<ElementType> AllocatorType = TypedDefaultAllocator<ElementType>>
    ArrayView(const Array<ElementType, AllocatorType>& array)
        : data_(array.data()), size_(array.size()) {}

    template <size_type Capacity>
    ArrayView(FixedArray<ElementType, Capacity> array)
        : data_(array.data()), size_(array.size()) {}

private:
    const ElementType* data_;
    size_type size_;
};

}  //namespace licht