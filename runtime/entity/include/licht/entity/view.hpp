#pragma once

#include <tuple>

#include "component_pool.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/entity/entity.hpp"

namespace licht {

template <typename... Components>
class EntityView {
public:
    using ComponentTypes = std::tuple<Components...>;

    class Iterator {
    public:
        using value_type = std::tuple<Entity, Components&...>;
        using reference = value_type;
        using pointer = void;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        Iterator(EntityView* view, size_t index)
            : view_(view), index_(index) {}

        bool operator!=(const Iterator& other) const { return index_ != other.index_; }

        bool operator==(const Iterator& other) const { return index_ == other.index_; }

        Iterator& operator++() {
            index_++;
            return *this;
        }

        Iterator& operator--() {
            index_--;
            return *this;
        }

        value_type operator*() {
            Entity entity = view_->entities_[index_];
            return view_->get_components_tuple(entity);
        }

    private:
        EntityView* view_;
        size_t index_;
    };

public:
    EntityView(const Array<Entity>& entities,
               const std::tuple<ComponentPool<Components>*...>& pools)
        : entities_(entities)
        , pools_(pools) {}

    template <typename Func>
    void for_each(Func&& func) {
        for (Entity entity : entities_) {
            if (has_all_components(entity)) {
                invoke_with_components(entity, std::forward<Func>(func));
            }
        }
    }

    template <typename Func>
    void for_each(Func&& func) const {
        for (Entity entity : entities_) {
            if (has_all_components(entity)) {
                invoke_with_components_const(entity, std::forward<Func>(func));
            }
        }
    }

    size_t size() const { return entities_.size(); }

    bool empty() const { return entities_.empty(); }

    Iterator begin() { return Iterator(this, 0); }

    Iterator end() { return Iterator(this, entities_.size()); }

private:
    bool has_all_components(Entity entity) const {
        return std::apply([entity](auto*... pools) {
            return (pools->has_component(entity) && ...);
        },
                          pools_);
    }

    template <typename Func>
    void invoke_with_components(Entity entity, Func&& func) {
        std::apply([entity, &func](auto*... pools) {
            func(entity, *(pools->get_component(entity))...);
        },
                   pools_);
    }

    template <typename Func>
    void invoke_with_components_const(Entity entity, Func&& func) const {
        std::apply([entity, &func](auto*... pools) {
            func(entity, *(pools->get_component(entity))...);
        },
                   pools_);
    }

    auto get_components_tuple(Entity entity) {
        return std::apply([entity](auto*... pools) {
            return std::make_tuple(entity, std::ref(*(pools->get_component(entity)))...);
        },
                          pools_);
    }

private:
    const Array<Entity>& entities_;
    std::tuple<ComponentPool<Components>*...> pools_;
};

}  //namespace licht
