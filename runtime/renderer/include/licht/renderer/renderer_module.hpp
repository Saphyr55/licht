#pragma once

#include "licht/core/modules/module.hpp"
#include "licht/renderer/renderer_exports.hpp"

namespace licht {

class LICHT_RENDERER_API RendererModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;
};

}  //namespace licht
