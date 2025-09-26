#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/hash_map.hpp"
#include "licht/core/function/function_ref.hpp"
#include "licht/core/memory/shared_ref.hpp"

namespace licht {

template <typename... ArgumentTypes>
class Connection;

template <typename... ArgumentTypes>
class Signal {
public:
    template <typename Callable>
    Connection<ArgumentTypes...> connect(Callable&& callable);

    template <typename ObjectType>
    Connection<ArgumentTypes...> connect(ObjectType* object, void (ObjectType::*method)(ArgumentTypes...));

    template <typename ObjectType>
    Connection<ArgumentTypes...> connect(SharedRef<ObjectType> object, void (ObjectType::*method)(ArgumentTypes...));

    void emit(ArgumentTypes... args);

    void operator()(ArgumentTypes... args);

    void disconnect(size_t id);

    void disconnect_all();

    size_t connection_count() const;

    bool empty() const;

public:
    Signal() = default;
    ~Signal() = default;

    Signal(const Signal&) = delete;
    Signal& operator=(const Signal&) = delete;
    Signal(Signal&& other) noexcept = default;
    Signal& operator=(Signal&& other) noexcept = default;

private:
    void cleanup_pending();
    friend class Connection<ArgumentTypes...>;

private:
    size_t next_id_ = 1;
    HashMap<size_t, FunctionRef<void(ArgumentTypes...)>> handlers_;
    Array<size_t> pending_removals_;
    bool emitting_ = false;
};

template <typename... ArgumentTypes>
template <typename Callable>
Connection<ArgumentTypes...> Signal<ArgumentTypes...>::connect(Callable&& callable) {
    size_t id = next_id_++;
    handlers_[id] = std::forward<Callable>(callable);
    return Connection<ArgumentTypes...>(id, *this);
}

template <typename... ArgumentTypes>
template <typename ObjectType>
Connection<ArgumentTypes...> Signal<ArgumentTypes...>::connect(ObjectType* object, void (ObjectType::*method)(ArgumentTypes...)) {
    return connect([object, method](ArgumentTypes... args) {
        (object->*method)(std::forward<ArgumentTypes>(args)...);
    });
}

template <typename... ArgumentTypes>
template <typename ObjectType>
Connection<ArgumentTypes...> Signal<ArgumentTypes...>::connect(SharedRef<ObjectType> object, void (ObjectType::*method)(ArgumentTypes...)) {
    return connect([&object, method](ArgumentTypes... args) {
        if (auto* shared_obj = &object) {
            (shared_obj->*method)(std::forward<ArgumentTypes>(args)...);
        }
    });
}

template <typename... ArgumentTypes>
void Signal<ArgumentTypes...>::emit(ArgumentTypes... args) {
    if (handlers_.empty()) {
        return;
    }

    emitting_ = true;

    HashMap<size_t, FunctionRef<void(ArgumentTypes...)>> handlers_copy = handlers_;

    for (auto& [id, handler] : handlers_copy) {
        if (handlers_.contains(id)) {
            handler(std::forward<ArgumentTypes>(args)...);
        }
    }

    emitting_ = false;
    cleanup_pending();
}

template <typename... ArgumentTypes>
void Signal<ArgumentTypes...>::operator()(ArgumentTypes... args) {
    emit(std::forward<ArgumentTypes>(args)...);
}

template <typename... Args>
void Signal<Args...>::disconnect(size_t id) {
    if (emitting_) {
        pending_removals_.append(id);
        return;
    }

    handlers_.remove(id);
}

template <typename... Args>
void Signal<Args...>::disconnect_all() {
    if (!emitting_) {
        handlers_.clear();
        return;
    }

    for (auto& [id, _] : handlers_) {
        pending_removals_.append(id);
    }
}

template <typename... Args>
void Signal<Args...>::cleanup_pending() {
    for (size_t id : pending_removals_) {
        handlers_.erase(id);
    }
    pending_removals_.clear();
}

template <typename... Args>
size_t Signal<Args...>::connection_count() const {
    return handlers_.size();
}

template <typename... Args>
bool Signal<Args...>::empty() const {
    return handlers_.empty();
}



}  //namespace licht