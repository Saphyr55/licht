#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/hash/hasher.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/memory.hpp"

#include <initializer_list>
#include <utility>

namespace licht {

template <typename KeyType>
struct HashSetElement {
    KeyType key;
    HashSetElement<KeyType>* next = nullptr;
    size_t hash = 0;

    HashSetElement() = default;

    HashSetElement(const KeyType& p_key, size_t p_hash)
        : key(p_key)
        , next(nullptr)
        , hash(p_hash) {}

    HashSetElement(KeyType&& p_key, size_t p_hash)
        : key(std::move(p_key))
        , next(nullptr)
        , hash(p_hash) {}
};

template <typename KeyType,
          CTypedAllocator<HashSetElement<KeyType>> AllocatorType = TypedDefaultAllocator<HashSetElement<KeyType>>,
          typename HasherType = Hasher>
class HashSet {
public:
    using ElementType = HashSetElement<KeyType>;
    using value_type = KeyType;

    class Iterator {
    public:
        Iterator(HashSet* set = nullptr, size_t bucket_index = 0, ElementType* elem = nullptr)
            : set_(set), bucket_index_(bucket_index), elem_(elem) {}

        value_type& operator*() const {
            LCHECK_MSG(elem_, "Iterator de-referencing null element");
            return elem_->key;
        }

        value_type* operator->() const { return &elem_->key; }

        Iterator& operator++() {
            if (!set_ || !elem_) {
                bucket_index_ = set_ ? set_->capacity_ : 0;
                elem_ = nullptr;
                return *this;
            }

            if (elem_->next) {
                elem_ = elem_->next;
                return *this;
            }

            bucket_index_++;
            while (bucket_index_ < set_->capacity_) {
                if (set_->buckets_[bucket_index_]) {
                    elem_ = set_->buckets_[bucket_index_];
                    return *this;
                }
                ++bucket_index_;
            }

            // Fin
            elem_ = nullptr;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return set_ == other.set_ && bucket_index_ == other.bucket_index_ && elem_ == other.elem_;
        }

        bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
        HashSet* set_;
        size_t bucket_index_;
        ElementType* elem_;
    };

    class ConstIterator {
    public:
        ConstIterator(const HashSet* set = nullptr, size_t bucket_index = 0, const ElementType* elem = nullptr)
            : set_(set), bucket_index_(bucket_index), elem_(elem) {}

        const value_type& operator*() const { return elem_->key; }

        const value_type* operator->() const { return &elem_->key; }

        ConstIterator& operator++() {
            if (!set_ || !elem_) {
                bucket_index_ = set_ ? set_->capacity_ : 0;
                elem_ = nullptr;
                return *this;
            }

            if (elem_->next) {
                elem_ = elem_->next;
                return *this;
            }

            ++bucket_index_;
            while (bucket_index_ < set_->capacity_) {
                if (set_->buckets_[bucket_index_]) {
                    elem_ = set_->buckets_[bucket_index_];
                    return *this;
                }
                ++bucket_index_;
            }

            elem_ = nullptr;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const {
            return set_ == other.set_ && bucket_index_ == other.bucket_index_ && elem_ == other.elem_;
        }

        bool operator!=(const ConstIterator& other) const { return !(*this == other); }

    private:
        const HashSet* set_;
        size_t bucket_index_;
        const ElementType* elem_;
    };

public:
    HashSet(size_t capacity = 8)
        : size_(0)
        , capacity_(capacity)
        , buckets_(nullptr)
        , allocator_() {
        initialize_buckets();
    }

    HashSet(std::initializer_list<KeyType> init)
        : size_(0)
        , capacity_(8)
        , buckets_(nullptr)
        , allocator_() {
        initialize_buckets();
        for (const auto& key : init) {
            insert(key);
        }
    }

    HashSet(const HashSet& other)
        : size_(0)
        , capacity_(other.capacity_)
        , buckets_(nullptr)
        , allocator_() {
        initialize_buckets();

        for (size_t i = 0; i < other.capacity_; ++i) {
            ElementType* cur = other.buckets_[i];
            ElementType** tail = &buckets_[i];
            while (cur) {
                ElementType* copy = allocator_.allocate(1);
                lplacement_new(copy) ElementType(cur->key, cur->hash);
                *tail = copy;
                tail = &((*tail)->next);
                cur = cur->next;
                ++size_;
            }
        }
    }

    HashSet(HashSet&& other) noexcept
        : size_(other.size_)
        , capacity_(other.capacity_)
        , buckets_(other.buckets_)
        , allocator_(std::move(other.allocator_)) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.buckets_ = nullptr;
    }

    ~HashSet() {
        clear();
        // TODO: use ldelete_array.
        delete[] buckets_;
        buckets_ = nullptr;
    }

    HashSet& operator=(const HashSet& other) {
        if (this == &other) {
            return *this;
        }

        HashSet tmp(other);
        swap(tmp);
        return *this;
    }

    HashSet& operator=(HashSet&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        clear();
        // TODO: use ldelete_array.
        delete[] buckets_;

        allocator_ = std::move(other.allocator_);
        size_ = other.size_;
        capacity_ = other.capacity_;
        buckets_ = other.buckets_;
        other.size_ = 0;
        other.capacity_ = 0;
        other.buckets_ = nullptr;

        return *this;
    }

    Iterator insert(const KeyType& key) {
        if (!buckets_) {
            initialize_buckets();
        }

        if (static_cast<float64>(size_ + 1) > static_cast<float64>(capacity_) * load_factor_) {
            resize(capacity_ * 2);
        }

        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);

        ElementType* cur = buckets_[idx];
        while (cur) {
            if (cur->key == key) {
                return Iterator(this, idx, cur);
            }
            cur = cur->next;
        }

        ElementType* ne = create_element(key, h);
        ne->next = buckets_[idx];
        buckets_[idx] = ne;
        size_++;
        return Iterator(this, idx, ne);
    }

    KeyType& add(const KeyType& key) {
        return *insert(key);
    }

    KeyType& add(KeyType&& key) {
        return *insert(key);
    }

    Iterator insert(KeyType&& key) {
        if (!buckets_) {
            initialize_buckets();
        }

        if (static_cast<float64>(size_ + 1) > static_cast<float64>(capacity_) * load_factor_) {
            resize(capacity_ * 2);
        }

        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);

        ElementType* cur = buckets_[idx];
        while (cur) {
            if (cur->key == key) {
                return Iterator(this, idx, cur);
            }
            cur = cur->next;
        }

        ElementType* ne = create_element(std::move(key), h);
        ne->next = buckets_[idx];
        buckets_[idx] = ne;
        size_++;
        return Iterator(this, idx, ne);
    }

    size_t remove(const KeyType& key) {
        if (!buckets_) {
            return 0;
        }
        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);

        ElementType* cur = buckets_[idx];
        ElementType* prev = nullptr;
        while (cur) {
            if (cur->key == key) {
                if (prev) {
                    prev->next = cur->next;
                } else {
                    buckets_[idx] = cur->next;
                }
                cur->~ElementType();
                allocator_.deallocate(cur, 1);
                --size_;
                return 1;
            }
            prev = cur;
            cur = cur->next;
        }
        return 0;
    }

    Iterator find(const KeyType& key) {
        if (!buckets_) {
            return end();
        }
        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);
        ElementType* cur = buckets_[idx];
        while (cur) {
            if (cur->key == key) {
                return Iterator(this, idx, cur);
            }
            cur = cur->next;
        }
        return end();
    }

    ConstIterator find(const KeyType& key) const {
        if (!buckets_) {
            return end();
        }
        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);
        ElementType* cur = buckets_[idx];
        while (cur) {
            if (cur->key == key) {
                return ConstIterator(this, idx, cur);
            }
            cur = cur->next;
        }
        return end();
    }

    bool contains(const KeyType& key) const {
        return find_element(key) != nullptr;
    }

    void clear() {
        if (!buckets_) {
            return;
        }
        for (size_t i = 0; i < capacity_; ++i) {
            ElementType* cur = buckets_[i];
            while (cur) {
                ElementType* next = cur->next;
                cur->~ElementType();
                allocator_.deallocate(cur, 1);
                cur = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    Iterator begin() {
        if (!buckets_) {
            return end();
        }
        for (size_t i = 0; i < capacity_; ++i) {
            if (buckets_[i]) {
                return Iterator(this, i, buckets_[i]);
            }
        }
        return end();
    }

    Iterator end() {
        return Iterator(this, capacity_, nullptr);
    }

    ConstIterator begin() const {
        if (!buckets_) {
            return end();
        }
        for (size_t i = 0; i < capacity_; ++i) {
            if (buckets_[i]) {
                return ConstIterator(this, i, buckets_[i]);
            }
        }
        return end();
    }

    ConstIterator end() const {
        return ConstIterator(this, capacity_, nullptr);
    }

    ConstIterator cbegin() const {
        return begin();
    }

    ConstIterator cend() const {
        return end();
    }

    void rehash() {
        resize(capacity_);
    }

    void resize(size_t new_capacity) {
        if (new_capacity == 0) {
            return;
        }
        // TODO: lnew_array
        ElementType** new_buckets = new ElementType*[new_capacity];
        Memory::write(new_buckets, 0, sizeof(ElementType*) * new_capacity);

        for (size_t i = 0; i < capacity_; ++i) {
            ElementType* cur = buckets_[i];
            while (cur) {
                ElementType* next = cur->next;
                size_t idx = static_cast<size_t>(cur->hash % new_capacity);

                cur->next = new_buckets[idx];
                new_buckets[idx] = cur;

                cur = next;
            }
        }

        delete[] buckets_;  // TODO: ldelete_array
        buckets_ = new_buckets;
        capacity_ = new_capacity;
    }

private:
    ElementType* find_element(const KeyType& key) const {
        if (!buckets_) {
            return nullptr;
        }
        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);
        ElementType* cur = buckets_[idx];
        while (cur) {
            if (cur->key == key) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    ElementType* create_element(const KeyType& key, size_t h) {
        ElementType* raw = allocator_.allocate(1);
        ElementType* element = lplacement_new(raw) ElementType(key, h);
        return element;
    }

    ElementType* create_element(KeyType&& key, size_t h) {
        ElementType* raw = allocator_.allocate(1);
        ElementType* element = lplacement_new(raw) ElementType(std::move(key), h);
        return element;
    }

    size_t hash(const KeyType& key) const {
        return std::hash<KeyType>{}(key);
    }

    void initialize_buckets() {
        if (capacity_ == 0) {
            capacity_ = 1;
        }
        //TODO: lnew_array
        buckets_ = new ElementType*[capacity_];
        Memory::write(buckets_, 0, sizeof(ElementType*) * capacity_);
    }

    void swap(HashSet& other) noexcept {
        std::swap(buckets_, other.buckets_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(allocator_, other.allocator_);
    }

private:
    ElementType** buckets_;
    AllocatorType allocator_;
    size_t size_ = 0;
    size_t capacity_ = 1;
    static constexpr float64 load_factor_ = 0.75;
};

}  //namespace licht
