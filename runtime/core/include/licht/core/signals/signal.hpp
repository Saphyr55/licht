#pragma once

#include <functional>
#include "licht/core/trace/trace.hpp"
#include "licht/core/signals/connection.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/containers/hash_map.hpp"
#include "licht/core/function/function_ref.hpp"
#include "licht/core/memory/shared_ref.hpp"

namespace licht {

template <typename... ArgumentTypes>
class Signal {
public:
    using connection_t = Connection<ArgumentTypes...>;

public:
    template <typename Callable>
    connection_t connect(Callable&& callable);

    template <typename ObjectType>
    connection_t connect(ObjectType* object, void (ObjectType::*method)(ArgumentTypes...));

    template <typename ObjectType>
    connection_t connect(SharedRef<ObjectType> object, void (ObjectType::*method)(ArgumentTypes...));

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
    HashMap<size_t, Function<void(ArgumentTypes...)>> handlers_;
    Array<size_t> pending_removals_;
    bool emitting_ = false;
};

template <typename... ArgumentTypes>
template <typename Callable>
auto Signal<ArgumentTypes...>::connect(Callable&& callable) -> connection_t {
    size_t id = next_id_++;
    handlers_[id] = [callable = std::move(callable)](ArgumentTypes... args) mutable -> void {
        callable(std::forward<ArgumentTypes>(args)...);
    };
    return Connection<ArgumentTypes...>(id, this);
}

template <typename... ArgumentTypes>
template <typename ObjectType>
auto Signal<ArgumentTypes...>::connect(ObjectType* object, void (ObjectType::*method)(ArgumentTypes...)) -> connection_t {
    auto callback = [object, method](ArgumentTypes... args) {
        if (object) {
            (object->*method)(args...);
        }
    };
    return connect(callback);
}

template <typename... ArgumentTypes>
template <typename ObjectType>
auto Signal<ArgumentTypes...>::connect(SharedRef<ObjectType> object, void (ObjectType::*method)(ArgumentTypes...)) -> connection_t {
    return connect(object.get_resource(), method);
}

template <typename... ArgumentTypes>
void Signal<ArgumentTypes...>::emit(ArgumentTypes... args) {
    if (handlers_.empty()) {
        return;
    }

    emitting_ = true;

    for (auto& [id, handler] : handlers_) {
        handler(std::forward<ArgumentTypes>(args)...);
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
        handlers_.remove(id);
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