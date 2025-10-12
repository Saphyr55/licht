#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/hash/hasher.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/memory.hpp"

#include <initializer_list>
#include <utility>

namespace licht {

template <typename KeyType, typename ValueType>
struct HashMapEntry {
    KeyType key;
    ValueType value;

    HashMapEntry() = default;

    HashMapEntry(const KeyType& p_key, const ValueType& p_value)
        : key(p_key)
        , value(p_value) {}

    HashMapEntry(KeyType&& p_key, ValueType&& p_value)
        : key(std::move(p_key))
        , value(std::move(p_value)) {}
};

template <typename KeyType, typename ValueType>
struct HashMapElement {
    HashMapEntry<KeyType, ValueType> entry;
    HashMapElement<KeyType, ValueType>* next = nullptr;
    size_t hash = 0;

    HashMapElement() = default;

    HashMapElement(const KeyType& p_key, const ValueType& p_value, size_t p_hash)
        : entry(p_key, p_value)
        , next(nullptr)
        , hash(p_hash) {}

    HashMapElement(KeyType&& p_key, ValueType&& p_value, size_t p_hash)
        : entry(std::move(p_key), std::move(p_value))
        , next(nullptr)
        , hash(p_hash) {}
};

template <typename KeyType,
          typename ValueType,
          CTypedAllocator<HashMapElement<KeyType, ValueType>> AllocatorType = TypedDefaultAllocator<HashMapElement<KeyType, ValueType>>,
          typename HasherType = Hasher>
class HashMap {
public:
    using ElementType = HashMapElement<KeyType, ValueType>;
    using EntryType = HashMapEntry<KeyType, ValueType>;

    class Iterator {
    public:
        Iterator(HashMap* map = nullptr, size_t bucket_index = 0, ElementType* elem = nullptr)
            : map_(map), bucket_index_(bucket_index), elem_(elem) {}

        EntryType& operator*() const { return elem_->entry; }

        EntryType* operator->() const { return &elem_->entry; }

        Iterator& operator++() {
            if (!map_ || !elem_) {
                // become end
                bucket_index_ = map_ ? map_->capacity_ : 0;
                elem_ = nullptr;
                return *this;
            }

            if (elem_->next) {
                elem_ = elem_->next;
                return *this;
            }

            // find next non-empty bucket
            ++bucket_index_;
            while (bucket_index_ < map_->capacity_) {
                if (map_->buckets_[bucket_index_]) {
                    elem_ = map_->buckets_[bucket_index_];
                    return *this;
                }
                ++bucket_index_;
            }

            // end
            elem_ = nullptr;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return map_ == other.map_ && bucket_index_ == other.bucket_index_ && elem_ == other.elem_;
        }

        bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
        HashMap* map_;
        size_t bucket_index_;
        ElementType* elem_;
    };

    class ConstIterator {
    public:
        ConstIterator(const HashMap* map = nullptr, size_t bucket_index = 0, const ElementType* elem = nullptr)
            : map_(map), bucket_index_(bucket_index), elem_(elem) {}

        const EntryType& operator*() const { return elem_->entry; }

        const EntryType* operator->() const { return &elem_->entry; }

        ConstIterator& operator++() {
            if (!map_ || !elem_) {
                bucket_index_ = map_ ? map_->capacity_ : 0;
                elem_ = nullptr;
                return *this;
            }

            if (elem_->next) {
                elem_ = elem_->next;
                return *this;
            }

            ++bucket_index_;
            while (bucket_index_ < map_->capacity_) {
                if (map_->buckets_[bucket_index_]) {
                    elem_ = map_->buckets_[bucket_index_];
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
            return map_ == other.map_ && bucket_index_ == other.bucket_index_ && elem_ == other.elem_;
        }

        bool operator!=(const ConstIterator& other) const { return !(*this == other); }

    private:
        const HashMap* map_;
        size_t bucket_index_;
        const ElementType* elem_;
    };

public:
    HashMap(size_t capacity = 8)
        : size_(0)
        , capacity_(capacity)
        , buckets_(nullptr)
        , allocator_() {
        initialize_buckets();
    }

    HashMap(std::initializer_list<EntryType> init)
        : size_(0)
        , capacity_(8)
        , buckets_(nullptr)
        , allocator_() {
        initialize_buckets();
        for (const auto& e : init) {
            put(e.key, e.value);
        }
    }

    HashMap(const HashMap& other)
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
                copy->entry.key = cur->entry.key;
                copy->entry.value = cur->entry.value;
                copy->hash = cur->hash;
                *tail = copy;
                tail = &((*tail)->next);
                cur = cur->next;
                ++size_;
            }
        }
    }

    HashMap(HashMap&& other) noexcept
        : size_(other.size_)
        , capacity_(other.capacity_)
        , buckets_(other.buckets_)
        , allocator_(std::move(other.allocator_)) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.buckets_ = nullptr;
    }

    ~HashMap() {
        clear();
        delete[] buckets_;
        buckets_ = nullptr;
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return *this;
        }

        HashMap tmp(other);
        swap(tmp);
        return *this;
    }

    HashMap& operator=(HashMap&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        clear();
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

    void remove(const KeyType& key) {
        if (!buckets_) {
            return;
        }
        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);

        ElementType* cur = buckets_[idx];
        ElementType* prev = nullptr;
        while (cur) {
            if (cur->entry.key == key) {
                if (prev) {
                    prev->next = cur->next;
                } else {
                    buckets_[idx] = cur->next;
                }
                allocator_.deallocate(cur, 1);
                --size_;
                return;
            }
            prev = cur;
            cur = cur->next;
        }
    }

    EntryType& put(const KeyType& key, const ValueType& value) {
        if (!buckets_) {
            initialize_buckets();
        }

        if (static_cast<float64>(size_ + 1) > static_cast<float64>(capacity_) * load_factor_) {
            resize_rehash(capacity_ * 2);
        }

        size_t h = hash(key);
        size_t idx = static_cast<size_t>(h % capacity_);

        ElementType* cur = buckets_[idx];
        while (cur) {
            if (cur->entry.key == key) {
                cur->entry.value = value;
                return cur->entry;
            }
            cur = cur->next;
        }

        ElementType* ne = create_element(key, value, h);
        ne->next = buckets_[idx];
        buckets_[idx] = ne;
        ++size_;
        return ne->entry;
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

    ValueType& get(const KeyType& key) {
        ElementType* e = find_element(key);
        LCHECK_MSG(e, "HashMap: key not found");
        return e->entry.value;
    }

    ValueType* get_ptr(const KeyType& key) {
        ElementType* e = find_element(key);
        return e ? &e->entry.value : nullptr;
    }

    ValueType& get_or_add(const KeyType& key, const ValueType& value) {
        auto* cvalue = get_ptr(key);
        if (cvalue) {
            return *cvalue;
        }
        return put(key, value).value;
    }

    const ValueType& get(const KeyType& key) const {
        const ElementType* e = find_element(key);
        LCHECK_MSG(e, "HashMap: key not found");
        return e->entry.value;
    }

    const ValueType* get_ptr(const KeyType& key) const {
        const ElementType* e = find_element(key);
        return e ? &e->entry.value : nullptr;
    }

    void clear() {
        if (!buckets_) {
            return;
        }
        for (size_t i = 0; i < size_; ++i) {
            ElementType* cur = buckets_[i];
            while (cur) {
                ElementType* next = cur->next;
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

    const ValueType& operator[](const KeyType& key) const {
        return get(key);
    }

    ValueType& operator[](const KeyType& key) {
        ElementType* e = find_element(key);
        if (e) {
            return e->entry.value;
        }
        EntryType& ent = put(key, ValueType{});
        return ent.value;
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
        resize_rehash(capacity_);
    }

    void resize_rehash(size_t new_capacity) {
        if (new_capacity == 0) {
            return;
        }

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

        delete[] buckets_;
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
            if (cur->entry.key == key) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    ElementType* create_element(const KeyType& key, const ValueType& value, size_t h) {
        ElementType* raw = allocator_.allocate(1);
        ElementType* element = new (raw) ElementType(key, value, h);
        return element;
    }

    size_t hash(const KeyType& key) const {
        return std::hash<KeyType>{}(key);
    }

    void initialize_buckets() {
        buckets_ = new ElementType*[capacity_];
        Memory::write(buckets_, 0, sizeof(ElementType*) * capacity_);
    }

    void swap(HashMap& other) noexcept {
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

}  // namespace licht
