#pragma once

#include <type_traits>
#include "allocator.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/traits/aligned_storage.hpp"
#include "memory.hpp"

namespace licht {

template <typename ResourceType>
class MemoryPool {
public:
    union Block {
        AlignedStorageType<ResourceType> storage;
        Block* next;
    };

public:
    void initialize_pool(Allocator* allocator, size_t block_count = 64);

    ResourceType* allocate_resource();

    void deallocate_resource(ResourceType* resource);

    template <typename... Args>
    ResourceType* new_resource(Args&&... args);

    void destroy_resource(ResourceType* resource);

    void dispose();

    Allocator* get_allocator() {
        return allocator_;
    }

    size_t get_block_count() {
        return block_count_;
    }

    size_t get_block_size() {
        return blocks_.size();
    }

    size_t get_block_capacity() {
        return blocks_.capacity();
    }

public:
    MemoryPool();
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    ~MemoryPool();

private:
    void grow();

private:
    Allocator* allocator_;
    Block* free_block_;
    Array<Block*> blocks_;
    size_t block_count_;
    size_t grow_factor_;
};

template <typename ResourceType>
MemoryPool<ResourceType>::MemoryPool()
    : allocator_(&DefaultAllocator::get_instance()), free_block_(nullptr), block_count_(64), blocks_(NoAllocationOnConstructionPolicy()), grow_factor_(2) {}

template <typename ResourceType>
MemoryPool<ResourceType>::~MemoryPool() {
    dispose();
}

template <typename ResourceType>
void MemoryPool<ResourceType>::initialize_pool(Allocator* allocator, size_t block_count) {
    allocator_ = allocator;
    block_count_ = block_count;
    blocks_ = Array<Block*>();
    grow();
}

template <typename ResourceType>
ResourceType* MemoryPool<ResourceType>::allocate_resource() {
    if (!free_block_) {
        grow();
    }

    Block* block = free_block_;
    free_block_ = block->next;
    return reinterpret_cast<ResourceType*>(&block->storage);
}

template <typename ResourceType>
void MemoryPool<ResourceType>::deallocate_resource(ResourceType* resource) {
    if (!resource) {
        return;
    }

    Block* block = reinterpret_cast<Block*>(resource);
    block->next = free_block_;
    free_block_ = block;
}

template <typename ResourceType>
template <typename... Args>
ResourceType* MemoryPool<ResourceType>::new_resource(Args&&... args) {
    ResourceType* resource = allocate_resource();
    lplacement_new(resource) ResourceType(std::forward<Args>(args)...);
    return resource;
}

template <typename ResourceType>
void MemoryPool<ResourceType>::destroy_resource(ResourceType* resource) {
    if (!resource) {
        return;
    }

    if constexpr (std::is_destructible_v<ResourceType>) {
        resource->~ResourceType();
    }

    deallocate_resource(resource);
}

template <typename ResourceType>
void MemoryPool<ResourceType>::dispose() {
    for (Block* block : blocks_) {
        allocator_->deallocate(block, block_count_ * sizeof(Block));
    }
    blocks_.clear();
    free_block_ = nullptr;
}

template <typename ResourceType>
void MemoryPool<ResourceType>::grow() {
    Block* new_block = static_cast<Block*>(allocator_->allocate(block_count_ * sizeof(Block)));
    blocks_.append(new_block);

    for (size_t i = 0; i < block_count_ - 1; i++) {
        new_block[i].next = &new_block[i + 1];
    }

    new_block[block_count_ - 1].next = free_block_;
    free_block_ = new_block;
}

}  //namespace licht