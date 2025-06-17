#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/hash/hasher.hpp"
#include "licht/core/memory/allocator.hpp"
#include "licht/core/memory/memory.hpp"

#include <initializer_list>
#include <type_traits>

namespace licht {

template <typename KeyType, typename ValueType>
struct HashMapEntry {
    KeyType key;
    ValueType value;

    HashMapEntry() = default;

    HashMapEntry(const KeyType& in_key, const ValueType& in_value)
        : key(in_key)
        , value(in_value) {}
};

template <typename KeyType, typename ValueType>
struct HashMapElement {
    HashMapEntry<KeyType, ValueType> entry;
    HashMapElement<KeyType, ValueType>* next = nullptr;

    HashMapElement() = default;

    HashMapElement(const KeyType& key, const ValueType& value)
        : entry(key, value)
        , next(nullptr) {}
};

template <typename KeyType,
          typename ValueType,
          typename AllocatorType = DefaultAllocator<HashMapElement<KeyType, ValueType>>,
          typename HasherType = Hasher>
class HashMap {
public:
    using ElementType = HashMapElement<KeyType, ValueType>;
    using EntryType = HashMapEntry<KeyType, ValueType>;

public:

    class Iterator {
    public:
        EntryType& operator*() {
            return current_->entry;
        }

        const EntryType& operator*() const {
            return current_->entry;
        }

        EntryType* operator->() {
            return current_->entry;
        }

        const EntryType* operator->() const {
            return current_->entry;
        }

        Iterator& operator++() {
            if (current_ != nullptr) {
                current_ = current_->next;
                if (current_ == nullptr) {
                    next();
                }
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current_ != other.current_;
        }

        Iterator(HashMap<KeyType, ValueType>& map, size_t i, ElementType* element)
            : map_(map)
            , index_(i)
            , current_(element) {
            if (current_ == nullptr) {
                next();
            }
        }

    private:
        void next() {
            while (current_ == nullptr && index_ < map_.capacity_ - 1) {
                ++index_;
                current_ = map_.elements_[index_];
            }
        }

    private:
        HashMap<KeyType, ValueType>& map_;
        ElementType* current_;
        size_t index_;
    };

public:

    void remove(const KeyType& key) {
        size_t index = hash(key);

        ElementType* current = elements_[index];
        ElementType* prev = nullptr;

        while (current != nullptr) {
            if (current->entry.key == key) {
                if (prev == nullptr) {
                    elements_[index] = current->next;
                } else {
                    prev->next = current->next;
                }

                allocator_.deallocate(current, 1);
                --size_;

                return;
            }
            prev = current;
            current = current->next;
        }
    }

    EntryType& insert(const KeyType& key, const ValueType& value) {
        if (size_ >= capacity_) {
            resize_rehash(capacity_ * 2);
        }
        
        uint32 index = hash(key);

        ElementType* old_element = elements_[index];
        ElementType* current = old_element;

        while (current != nullptr) {
            if (current->entry.key == key) {  // TODO: Use a comparator.
                current->entry.value = value;
                return old_element->entry;
            }
            current = current->next;
        }

        ElementType* new_element = create_element(key, value);
        new_element->next = old_element;
        elements_[index] = new_element;
        size_++;

        return new_element->entry;
    }

    bool contains(const KeyType& key) const {
        return get_ptr(key) != nullptr;
    }

    ValueType& get(const KeyType& key) {
        uint32 index = hash(key);

        ElementType* element = lookup(index, key);
        LCHECK(element);

        return element->entry.value;
    }

    ValueType* get_ptr(const KeyType& key) {
        uint32 index = hash(key);

        ElementType* element = lookup(index, key);
        if (element) {
            return &element->entry.value;
        }

        return nullptr;
    }

    const ValueType& get(const KeyType& key) const {
        uint32 index = hash(key);

        ElementType* element = lookup(index, key);
        LCHECK(element);

        return element->entry.value;
    }

    const ValueType* get_ptr(const KeyType& key) const {
        uint32 index = hash(key);
        ElementType* element = lookup(index, key);

        if (element) {
            return &element->entry.value;
        }

        return nullptr;
    }

    void clear() {
        if (!elements_ || size_ == 0) {
            return;
        }

        for (size_t i = 0; i < capacity_; i++) {
            if (elements_[i]) {
                allocator_.deallocate(elements_[i], 1);
                elements_[i] = nullptr;
            }
        }

        size_ = 0;
    }

     size_t size() {
        return size_;
    }

     size_t capacity() {
        return capacity_;
    }

    HashMap(size_t capacity = 8)
        : size_(0)
        , capacity_(capacity)
        , elements_(nullptr)
        , allocator_(AllocatorType()) {
        elements_ = allocate_elements(capacity_);
    }

    HashMap(std::initializer_list<EntryType> init)
        : size_(0)
        , capacity_(init.size())
        , elements_(nullptr)
        , allocator_(AllocatorType()) {
        elements_ = allocate_elements(capacity_);
        for (const EntryType& entry : init) {
            insert(entry.key, entry.value);
        }
    }

    HashMap(const HashMap& other)
        : size_(other.size_)
        , capacity_(other.capacity_)
        , elements_(nullptr)
        , allocator_(AllocatorType()) {
        elements_ = allocate_elements(capacity_);
        copy_from_other(other.elements_, other.capacity_);
    }

    ~HashMap() {
        clear();

        if (elements_) {
            Memory::free(elements_, capacity_ * sizeof(ElementType*));
        }
    }

    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            capacity_ = other.capacity_;
            size_ = 0;
            elements_ = static_cast<ElementType**>(Memory::allocate(capacity_ * sizeof(ElementType*)));
            copy_from_other(other.elements_, other.capacity_);
        }

        return *this;
    }

    const ValueType& operator[](const KeyType& key) const {
        return get(key);
    }

    ValueType& operator[](const KeyType& key) {
        uint32 index = hash(key);

        ElementType* element = lookup(index, key);
        if (element == nullptr) {
            if constexpr (std::is_default_constructible_v<ValueType>) {
                return insert(key, ValueType()).value;
            } else if constexpr (std::is_pointer_v<ValueType>) {
                return insert(key, (ValueType)(nullptr)).value;
            } else {
                auto name = typeid(ValueType).name();
                LCHECK_MSG(false, vformat("No default construstor found for ValueType: %s", name));
            }
        }

        return element->entry.value;
    }

public:
    Iterator begin() {
        return Iterator(*this, 0, elements_[0]);
    }

    Iterator end() {
        return Iterator(*this, capacity_ - 1, nullptr);
    }

    const Iterator cbegin() {
        return Iterator(*this, 0, elements_[0]);
    }

    const Iterator cend() {
        return Iterator(*this, capacity_ - 1, nullptr);
    }

private:
    ElementType* lookup(uint32 index, const KeyType& key) const {
        ElementType* element = elements_[index];

        while (element != nullptr) {
            if (element->entry.key == key) {  // TODO: Use a comparator.
                return element;
            }
            element = element->next;
        }

        return nullptr;
    }

    void resize_rehash(size_t new_capacity) {
        size_t old_capacity = capacity_;
        ElementType** old_elements = elements_;

        size_t old_allocation_size = old_capacity * sizeof(ElementType*);
        ElementType** new_elements = allocate_elements(new_capacity);

        for (size_t i = 0; i < old_capacity; i++) {
            ElementType* old_element = old_elements[i];
            while (old_element != nullptr) {
                insert(old_element->entry.key, old_element->entry.value);
                ElementType* temp = old_element;
                old_element = old_element->next;
            }
            new_elements[i] = old_element;
        }

        elements_ = new_elements;
        Memory::free(old_elements, old_allocation_size);
    }

    ElementType* create_element(const KeyType& key, const ValueType& value) {
        ElementType* new_element = allocator_.allocate(1);
        new_element->entry = EntryType(key, value);
        return new_element;
    }

    void copy_from_other(ElementType** other_elements, size_t other_capacity) {
        for (size_t i = 0; i < other_capacity; ++i) {
            ElementType* current = allocator_.allocate(1);
            Memory::copy(current, other_elements[i], sizeof(ElementType));
            while (current != nullptr) {
                insert(current->entry.key, current->entry.value);
                current = current->next;
            }
        }
    }
    
    uint32_t hash(const KeyType& key) const {
        return Hasher::hash<KeyType>(key) % capacity_;
    }

    static ElementType** allocate_elements(size_t capacity) {
        size_t allocation_size = capacity * sizeof(ElementType*);
        ElementType** elements = static_cast<ElementType**>(Memory::allocate(allocation_size));
        for (size_t i = 0; i < capacity; i++) {
            elements[i] = nullptr;
        }
        return elements;
    }

private:
    ElementType** elements_;
    AllocatorType allocator_;
    size_t size_{};
    size_t capacity_{};
};

}  //namespace licht