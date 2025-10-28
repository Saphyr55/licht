#pragma once

#include "entity.hpp"
#include "licht/entity/entity_exports.hpp"
#include "licht/core/containers/sparse_set.hpp"
#include "licht/entity/entity.hpp"

namespace licht {

class LICHT_ENTITY_API ComponentPoolInterface {
public:
    virtual void remove_component(Entity entity) = 0;

    virtual void dispose() = 0;
    
    virtual bool has_component(Entity e) const = 0;
    
    virtual Array<Entity>& entities() = 0;

    virtual ~ComponentPoolInterface() = default;
};

template <typename ComponentType>
class ComponentPool : public ComponentPoolInterface {
public:
    using component_type = ComponentType;

public:
    ComponentType& add_component(Entity e, const ComponentType& component) {
        return *storage_.put(e, component);
    }

    virtual void remove_component(Entity e) override {
        if (has_component(e)) {
            storage_.remove(e);
        }
    }

    virtual bool has_component(Entity e) const override {
        return storage_.get(e) != nullptr;
    }

    ComponentType* get_component(Entity e) {
        return storage_.get(e);
    }

    template <typename Func>
    void for_each(Func&& func) {
        auto& values = storage_.elements();
        for (auto& v : values) {
            func(v);
        }
    }

    virtual void dispose() override {
        storage_.clear();
    }

    const SparseSet<ComponentType, Entity>& storage() const {
        return storage_;
    }

    SparseSet<ComponentType, Entity>& storage() {
        return storage_;
    }

    virtual Array<Entity>& entities() override { 
        return storage_.dense(); 
    }

private:
    SparseSet<ComponentType, Entity> storage_;
};
}  //namespace licht