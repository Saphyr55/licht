#pragma once

#include "licht/core/core_exports.hpp"

namespace licht {

template <typename... ArgumentTypes>
class Signal;

template <typename... ArgumentTypes>
class Connection {
public:
    void disconnect();

    bool is_connected() const;

    size_t get_id() const;

public:
    Connection() = default;
    Connection(size_t id, Signal<ArgumentTypes...>* signal);
    ~Connection();

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    Connection(Connection&& other) noexcept = default;
    Connection& operator=(Connection&& other) noexcept = default;

private:
    size_t id_ = 0;
    Signal<ArgumentTypes...>* signal_ = nullptr;
    bool connected_ = false;
};

template <typename... Args>
void Connection<Args...>::disconnect() {
    if (connected_ && signal_) {
        signal_->disconnect(id_);
        connected_ = false;
        signal_ = nullptr;
    }
}

template<typename... Args>
bool Connection<Args...>::is_connected() const {
    return connected_;
}

template<typename... Args>
size_t Connection<Args...>::get_id() const {
    return id_;
}

template <typename... ArgumentTypes>
Connection<ArgumentTypes...>::Connection(size_t id, Signal<ArgumentTypes...>* signal)
    : id_(id), signal_(signal), connected_(true) {}

template <typename... Args>
Connection<Args...>::~Connection() {
}

}  //namespace licht
