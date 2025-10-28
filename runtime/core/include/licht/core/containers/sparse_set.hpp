#pragma once

#include <limits>
#include "licht/core/containers/array.hpp"

namespace licht {

template <typename ElementType, typename KeyIndexType = size_t>
class SparseSet {
public:
    using size_type = size_t;
    using value_type = ElementType;
    using reference = ElementType&;
    using const_reference = const ElementType&;

    using key_index_type = KeyIndexType;
    using sparse_type = Array<key_index_type>;

    static constexpr const key_index_type invalid_index = std::numeric_limits<key_index_type>::max();

public:
    ElementType* put(size_t i, const_reference element) {
        key_index_type index = get_dense_index(i);
        if (index != invalid_index) {
            dense_value_[index] = element;
            dense_identification_[index] = i;

            return &dense_value_[index];
        }

        key_index_type new_index = dense_value_.size();
        set_dense_index(i, new_index);

        dense_value_.push_back(element);
        dense_identification_.push_back(i);

        return &dense_value_.back();
    }

    reference operator[](size_type i) {
        key_index_type index = get_dense_index(i);
        return dense_value_[index];
    }

    const_reference operator[](size_type i) const {
        key_index_type index = get_dense_index(i);
        return dense_value_[index];
    }

    ElementType* get(size_type i) {
        key_index_type index = get_dense_index(i);
        return (index != invalid_index) ? &dense_value_[index] : nullptr;
    }

    const ElementType* get(size_type i) const {
        key_index_type index = get_dense_index(i);
        return (index != invalid_index) ? &dense_value_[index] : nullptr;
    }

    void remove(size_type i) {
        key_index_type deleted_index = get_dense_index(i);
        LCHECK(deleted_index != invalid_index && !dense_value_.empty());

        key_index_type last_index = dense_value_.size() - 1;
        key_index_type last_id = dense_identification_.back();

        if (deleted_index != last_index) {
            std::swap(dense_value_[deleted_index], dense_value_.back());
            std::swap(dense_identification_[deleted_index], dense_identification_.back());
            set_dense_index(last_id, deleted_index);
        }

        set_dense_index(i, invalid_index);

        dense_value_.pop();
        dense_identification_.pop();
    }

    sparse_type& dense() {
        return dense_identification_;
    }

    key_index_type dense_at(size_type dense_index) const {
        return dense_identification_[dense_index];
    }

    inline const Array<ElementType>& elements() const {
        return dense_value_;
    }

    inline bool contains(size_type i) {
        return get_dense_index(i) != invalid_index;
    }

    inline bool empty() const {
        return dense_value_.empty();
    }

    inline size_type size() const {
        return dense_value_.size();
    }

    void reserve(size_type dense_capacity) {
        if (capacity_page_ < dense_capacity) {
            capacity_page_ = dense_capacity;
        }
        dense_value_.reserve(dense_capacity);
        dense_identification_.reserve(dense_capacity);
    }

    void resize(size_type new_size, const_reference default_value = ElementType{}) {
        if (capacity_page_ < new_size) {
            capacity_page_ = new_size;
        }
        dense_value_.resize(new_size, default_value);
        dense_identification_.resize(new_size, invalid_index);
    }

    void clear() {
        dense_value_.clear();
        dense_identification_.clear();
        sparses_.clear();
    }

public:
    SparseSet(size_t capacity_page = 100)
        : capacity_page_(capacity_page)
        , dense_value_(capacity_page)
        , sparses_(capacity_page) {
    }

    ~SparseSet() = default;

private:
    void set_dense_index(size_type i, key_index_type index) {
        size_type page = i / capacity_page_;
        size_type sparse_index = i % capacity_page_;

        if (page >= sparses_.size()) {
            sparses_.resize(page + 1);
        }

        sparse_type& sparse = sparses_[page];
        if (sparse_index >= sparse.size()) {
            sparse.resize(sparse_index + 1, invalid_index);
        }

        sparse[sparse_index] = index;
    }

    key_index_type get_dense_index(size_type i) const {
        size_type page = i / capacity_page_;
        size_type sparse_index = i % capacity_page_;

        if (page < sparses_.size()) {
            const sparse_type& sparse = sparses_[page];
            if (sparse_index < sparse.size()) {
                return sparse[sparse_index];
            }
        }

        return invalid_index;
    }

private:
    Array<sparse_type> sparses_;
    Array<ElementType> dense_value_;
    sparse_type dense_identification_;
    size_type capacity_page_;
};

}  //namespace licht