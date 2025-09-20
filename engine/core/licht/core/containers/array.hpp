#pragma once

#include "licht/core/algorithm/comparator.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/concepts.hpp"
#include "licht/core/memory/default_allocator.hpp"

#include <algorithm>
#include <cstddef>

namespace licht {

template <typename ElementType, CAllocator<ElementType> AllocatorType = TypedDefaultAllocator<ElementType>>
class Array {
public:
    using IteratorType = ElementType*;

    template <typename NewElement>
    using ReboundAllocator = typename AllocatorType::template rebind<NewElement>::other;

    using value_type = ElementType;
    using size_type = size_t;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using iterator = ElementType*;
    using const_iterator = const ElementType*;

public:

    template <typename OtherElementType = ElementType,
              typename OtherAllocatorType = ReboundAllocator<OtherElementType>>
    Array<OtherElementType, OtherAllocatorType> map(auto&& mapper,
                                                    const OtherAllocatorType& other_allocator = OtherAllocatorType()) const {
        Array<OtherElementType, OtherAllocatorType> other(size_, other_allocator);

        for (size_type i = 0; i < size_; i++) {
            other.append(mapper(data_[i]));
        }

        return other;
    }

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

    void append(const ElementType& element) {
        if (!data_) {
            reallocate(capacity_);
        }

        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        new (data_ + size_) ElementType(element);

        ++size_;
    }

    template <typename OtherAllocatorType>
    void append_all(const Array<ElementType, OtherAllocatorType>& array) {
        for (const ElementType& element : array) {
            append(element);
        }
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        new (data_ + size_) ElementType(std::forward<Args>(args)...);
        ++size_;
    }

    void pop() {
        if (size_ > 0) {
            --size_;
            data_[size_].~ElementType();
        }
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

    inline constexpr size_type capacity() const {
        return capacity_;
    }

    inline constexpr bool empty() const {
        return size() == 0;
    }

    void clear() {
        for (size_type i = 0; i < size_; i++) {
            data_[i].~ElementType();
        }

        size_ = 0;
    }

    void remove(const ElementType& value) {
        remove(value, [](auto& p_1, auto& p_2) -> int32 {
            if (p_1 == p_2) {
                return 0;
            } else if (p_1 > p_2) {
                return 1;
            } else {
                return -1;
            }
        });
    }

    void remove(const ElementType& value, auto&& predicate) {
        size_type new_size = 0;
        for (size_type i = 0; i < size_; ++i) {
            if (predicate(data_[i], value)) {
                if (new_size != i) {
                    new (data_ + new_size) ElementType(data_[i]);
                }
                ++new_size;
            } else {
                data_[i].~ElementType();
            }
        }
        size_ = new_size;
    }

    void resize(size_type size, const ElementType& default_element = ElementType()) {
        if (size > capacity_) {
            reserve(size);
        }

        if (size > size_) {
            for (size_type i = size_; i < size; ++i) {
                new (data_ + i) ElementType(default_element);
            }
        } else {
            for (size_type i = size; i < size_; ++i) {
                data_[i].~ElementType();
            }
        }

        size_ = size;
    }

    void reserve(size_type capacity) {
        if (capacity > capacity_) {
            reallocate(capacity);
        }
    }

    void shrink() {
        if (size_ < capacity_) {
            reallocate(size_);
        }
    }

    void swap(Array& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(allocator_, other.allocator_);
    }

    inline const ElementType* data() const {
        return data_;
    }

    inline ElementType* data() {
        return data_;
    }

    void push_back(ElementType element) {
        append(std::move(element));
    }

    template <typename Predicate>
    ElementType* get_if(Predicate&& p_predicate) const {
        for (size_type i = 0; i < size_; ++i) {
            if (p_predicate(data_[i])) {
                return &data_[i];
            }
        }
        return nullptr;
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

public:
    constexpr Array(const AllocatorType& allocator = AllocatorType()) noexcept
        : data_(nullptr)
        , size_(0)
        , capacity_(2)
        , allocator_(allocator) {
        data_ = allocator_allocate(capacity_);
    }

    constexpr Array(size_t capacity,
                    const AllocatorType& allocator = AllocatorType()) noexcept
        : data_(nullptr)
        , size_(0)
        , capacity_(capacity)
        , allocator_(allocator) {
        data_ = allocator_allocate(capacity_);
    }

    constexpr Array(std::initializer_list<ElementType> init,
                    const AllocatorType& allocator = AllocatorType())
        : allocator_(allocator)
        , data_(nullptr)
        , size_(init.size())
        , capacity_(init.size()) {
        if (capacity_ == 0) {
            capacity_ = 2;
        }
        data_ = allocator_allocate(capacity_);
        size_type i = 0;
        for (const ElementType& item : init) {
            new (data_ + i++) ElementType(item);
        }
    }

    constexpr Array(const Array& other) noexcept
        : allocator_(other.allocator_)
        , data_(nullptr)
        , size_(other.size_)
        , capacity_(other.capacity_) {
        move_data(other.data_);
    }

    constexpr Array(Array&& other) noexcept
        : data_(other.data_)
        , size_(other.size_)
        , capacity_(other.capacity_)
        , allocator_(std::move(other.allocator_)) {
        other.data_ = nullptr;
    }

    ~Array() {
        if (data_) {
            clear();
            allocator_deallocate(data_, capacity_);
            data_ = nullptr;
        }
    }

    constexpr Array& operator=(const Array& other) {
        if (this != &other) {
            Array temp(other);
            swap(temp);
        }
        return *this;
    }

    constexpr Array& operator=(Array&& other) noexcept {
        if (this != &other) {
            clear();

            if (data_) {
                allocator_deallocate(data_, capacity_);
            }

            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            allocator_ = std::move(other.allocator_);
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }

        return *this;
    }

public:
    IteratorType begin() {
        return data_;
    }

    const IteratorType begin() const {
        return data_;
    }

    const IteratorType cbegin() const {
        return data_;
    }

    IteratorType end() {
        return data_ + size_;
    }

    const IteratorType end() const {
        return data_ + size_;
    }

    const IteratorType cend() const {
        return data_ + size_;
    }

private:
    void move_data(ElementType* source) {
        if (size_ <= 0) {
            return;
        }

        data_ = allocator_allocate(capacity_);
        for (size_type i = 0; i < size_; i++) {
            new (data_ + i) ElementType(source[i]);
        }
    }

    void reallocate(size_type new_capacity) {
        ElementType* new_data = allocator_allocate(new_capacity);

        size_type elements_to_move = (size_ < new_capacity) ? size_ : new_capacity;

        for (size_type i = 0; i < elements_to_move; i++) {
            new (new_data + i) ElementType(std::move(data_[i]));
        }

        for (size_type i = 0; i < size_; i++) {
            data_[i].~ElementType();
        }

        if (data_) {
            allocator_deallocate(data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_capacity;

        if (new_capacity < size_) {
            size_ = new_capacity;
        }
    }

    inline ElementType* allocator_allocate(size_type n) {
        ElementType* elements = allocator_.allocate(n);
        LCHECK(elements);
        return elements;
    }

    void allocator_deallocate(ElementType* p, size_type n) {
        if (p) {
            allocator_.deallocate(p, n);
        }
    }

private:
    ElementType* data_;
    size_type size_;
    size_type capacity_;
    AllocatorType allocator_;
};

template <typename ElementType, typename AllocatorType>
inline constexpr bool operator==(const Array<ElementType, AllocatorType>& lhs,
                                 const Array<ElementType, AllocatorType>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.size(); i++) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

template <typename ElementType, typename AllocatorType>
inline constexpr bool operator!=(const Array<ElementType, AllocatorType>& lhs,
                                 const Array<ElementType, AllocatorType>& rhs) {
    return !(lhs == rhs);
}

template <typename ElementType, typename AllocatorType>
inline constexpr void swap(
    Array<ElementType, AllocatorType>& lhs,
    Array<ElementType, AllocatorType>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

}  // namespace licht
