#pragma once

#include <licht/core/containers/array.hpp>
#include <licht/core/containers/index_range.hpp>
#include <licht/core/containers/option.hpp>
#include <licht/core/defines.hpp>
#include <licht/core/io/file_handle.hpp>
#include <licht/core/io/file_system.hpp>
#include <licht/core/memory/memory.hpp>
#include <licht/core/memory/memory_trace.hpp>
#include <licht/core/memory/shared_ref.hpp>
#include <licht/core/string/format.hpp>
#include <licht/core/trace/trace.hpp>
#include <licht/core/platform/display.hpp>
#include <licht/core/platform/display_message_handler.hpp>
#include <licht/core/platform/platform.hpp>
#include <licht/core/modules/module.hpp>
#include <licht/core/modules/module_registry.hpp>
#include <licht/core/string/string_ref.hpp>

namespace licht::demo {

class DemoModule : public Module {
public:
    static constexpr StringRef Name = "demo.licht";

    virtual void on_load() override {
    }

    virtual void on_activate() override {
    }

    virtual void on_shutdown() override {
    }

    virtual void on_unload() override {
    }

};

LICHT_REGISTER_MODULE(DemoModule, "licht.demo")

}  //namespace licht::demo

