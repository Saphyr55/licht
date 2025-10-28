#pragma once

#include <tuple>
#include <typeindex>
#include <utility>

#include "licht/core/containers/hash_map.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/entity/component_pool.hpp"
#include "licht/entity/entity_exports.hpp"

namespace licht {

class LICHT_ENTITY_API EntityRegistry {
public:
    using ComponentPoolMap = HashMap<std::type_index, SharedRef<ComponentPoolInterface>>;

public:
    Entity create() {
        Entity e = next_id_++;
        living_entities_.push_back(e);
        return e;
    }

    void destroy(Entity e) {
        for (auto& [_, pool] : component_pools_) {
            pool->remove_component(e);
        }

        for (size_t i = 0; i < living_entities_.size(); ++i) {
            if (living_entities_[i] == e) {
                living_entities_.remove(e);
                break;
            }
        }
    }

    bool valid(Entity e) const {
        return living_entities_.contains(e);
    }

    template <typename ComponentType>
    ComponentType& add(Entity e, const ComponentType& component = ComponentType()) {
        return get_pool<ComponentType>().add_component(e, component);
    }

    template <typename ComponentType>
    void remove(Entity e) {
        ComponentPoolMap::iterator it = component_pools_.find(std::type_index(typeid(ComponentType)));
        if (it != component_pools_.end()) {
            static_ref_cast<ComponentPool<ComponentType>>(it->value)->remove_component(e);
        }
    }

    template <typename ComponentType>
    bool has(Entity e) const {
        ComponentPoolMap::const_iterator it = component_pools_.find(std::type_index(typeid(ComponentType)));
        if (it == component_pools_.end()) {
            return false;
        }
        return static_ref_cast<ComponentPool<ComponentType>>(it->value)->has_component(e);
    }

    template <typename... Components>
    bool has_all(Entity e) const {
        return (... && has<Components>(e));
    }

    template <typename ComponentType>
    ComponentType* get(Entity e) {
        ComponentPoolMap::iterator it = component_pools_.find(std::type_index(typeid(ComponentType)));
        if (it == component_pools_.end()) {
            return nullptr;
        }
        return static_ref_cast<ComponentPool<ComponentType>>(it->value)->get_component(e);
    }

    template <typename... Components, typename Func>
    void each(Func&& func) {
        ComponentPoolInterface* smallest_pool = nullptr;
        size_t min_size = std::numeric_limits<size_t>::max();

        auto pools = std::make_tuple(&get_pool<Components>()...);

        std::apply([&](auto*... pool_ptrs) {
            size_t sizes[] = {pool_ptrs->storage().size()...};
            size_t min_index = 0;
            for (size_t i = 0; i < sizeof...(Components); ++i) {
                if (sizes[i] < min_size) {
                    min_size = sizes[i];
                    min_index = i;
                }
            }

            ComponentPoolInterface* pool_ptrs_array[] = {pool_ptrs...};
            smallest_pool = pool_ptrs_array[min_index];
        },
                   pools);

        if (!smallest_pool || min_size == 0) {
            return;
        }

        auto& entities = static_cast<ComponentPool<std::tuple_element_t<0, std::tuple<Components...>>>*>(smallest_pool)->entities();

        for (Entity entity : entities) {
            if (has_all<Components...>(entity)) {
                invoke_with_components<Components...>(entity, std::forward<Func>(func));
            }
        }
    }

    template <typename ComponentType>
    ComponentPool<ComponentType>& get_pool() {
        auto key = std::type_index(typeid(ComponentType));
        auto it = component_pools_.find(key);
        if (it == component_pools_.end()) {
            SharedRef<ComponentPool<ComponentType>> pool = new_ref<ComponentPool<ComponentType>>();
            ComponentPool<ComponentType>* ptr = pool.get_resource();
            component_pools_[key] = std::move(pool);
            return *ptr;
        }
        return *static_ref_cast<ComponentPool<ComponentType>>(it->value);
    }

    void dispose() {
        for (auto& [type, pool] : component_pools_) {
            pool->dispose();
        }

        component_pools_.clear();
        living_entities_.clear();
        next_id_ = 1;
    }

    EntityRegistry()
        : next_id_(1) {
    }

    ~EntityRegistry() {
        dispose();
    }

private:
    template <typename... Components, typename Func>
    void invoke_with_components(Entity entity, Func&& func) {
        if constexpr (sizeof...(Components) == 1) {
            auto* component = get<std::tuple_element_t<0, std::tuple<Components...>>>(entity);
            if (component) {
                func(entity, *component);
            }
        } else {
            auto components = std::make_tuple(get<Components>(entity)...);
            std::apply([&](auto*... comps) { func(entity, *comps...); }, components);
        }
    }

private:
    ComponentPoolMap component_pools_;
    Array<Entity> living_entities_;
    Entity next_id_ = 0;  // 0 reserved as invalid.
};

}  //namespace licht