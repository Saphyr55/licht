#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/allocator.hpp"

#include <algorithm>

namespace licht {

template <typename ElementType,
          typename AllocatorType = DefaultAllocator<ElementType>>
class Array {
public:
    using SizeType = size_t;
    using IteratorType = ElementType*;

public:
    void push_back(ElementType element) {
        append(std::move(element));
    }

    void append(const ElementType& element) {
        if (size_ >= capacity_) {
            reverse(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        new (data_ + size_) ElementType(element);
        ++size_;
    }

    void append(ElementType&& element) {
        if (size_ >= capacity_) {
            reverse(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        new (data_ + size_) ElementType(std::move(element));
        ++size_;
    }

    template<typename OtherAllocatorType>
    void append_all(const Array<ElementType, OtherAllocatorType>& array) {
        for (const ElementType& element: array) {
            append(element);
        }
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        if (size_ >= capacity_) {
            reverse(capacity_ == 0 ? 1 : capacity_ * 2);
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

    inline constexpr ElementType& operator[](SizeType index) {
        LCHECK(index < size_);
        return data_[index];
    }

    inline constexpr const ElementType& operator[](SizeType index) const {
        LCHECK(index < size_);
        return data_[index];
    }

    inline constexpr SizeType size() const {
        return size_;
    }

    inline constexpr SizeType capacity() const {
        return capacity_;
    }

    inline constexpr bool empty() const {
        return size() == 0;
    }

    void clear() {
        for (SizeType i = 0; i < size_; i++) {
            data_[i].~ElementType();
        }

        size_ = 0;
    }

    void remove(const ElementType& value) {
        SizeType newSize = 0;
        for (SizeType i = 0; i < size_; ++i) {
            if (data_[i] != value) {
                if (newSize != i) {
                    new (data_ + newSize) ElementType(data_[i]);
                }
                ++newSize;
            } else {
                data_[i].~ElementType();
            }
        }
        size_ = newSize;
    }

    void reverse(SizeType capacity) {
        if (capacity > capacity_) {
            allocate_resize(capacity);
        }
    }

    void shrink() {
        if (size_ < capacity_) {
            reverse(size_);
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

public:
    constexpr explicit Array(AllocatorType&& allocator = AllocatorType()) noexcept
        : data_(nullptr)
        , size_(0)
        , capacity_(1)
        , allocator_(allocator) {
        data_ = allocator_allocate(capacity_);
    }

    constexpr Array(size_t capacity,
                    AllocatorType&& allocator = AllocatorType()) noexcept
        : data_(nullptr)
        , size_(capacity)
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
        if (capacity_ > 0) {
            data_ = allocator_allocate(capacity_);
            SizeType i = 0;
            for (const ElementType& item : init) {
                new (data_ + i) ElementType(item);
                ++i;
            }
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
        clear();
        if (data_) {
            allocator_deallocate(data_, capacity_);
            data_ = nullptr;
        }
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            Array temp(other);
            swap(temp);
        }
        return *this;
    }

    Array& operator=(Array&& other) noexcept {
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
        for (SizeType i = 0; i < size_; ++i) {
            new (data_ + i) ElementType(source[i]);
        }
    }

    void allocate_resize(SizeType new_capacity) {
        if (new_capacity == capacity_) {
            return;
        }

        ElementType* new_data = allocator_allocate(new_capacity);

        SizeType elements_to_move = (size_ < new_capacity) ? size_ : new_capacity;

        for (SizeType i = 0; i < elements_to_move; ++i) {
            new (new_data + i) ElementType(std::move(data_[i]));
        }

        for (SizeType i = 0; i < size_; ++i) {
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
    
    inline ElementType* allocator_allocate(SizeType n) {
        return allocator_.allocate(n);
    }

    void allocator_deallocate(ElementType* p, SizeType n) {
        if (p) {
            allocator_.deallocate(p, n);
        }
    }

private:
    ElementType* data_;
    SizeType size_;
    SizeType capacity_;
    AllocatorType allocator_;
};

template <typename ElementType, typename AllocatorType>
constexpr bool operator==(const Array<ElementType, AllocatorType>& lhs,
                          const Array<ElementType, AllocatorType>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.size(); ++i) {
        if (!(lhs[i] == rhs[i])) {
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
