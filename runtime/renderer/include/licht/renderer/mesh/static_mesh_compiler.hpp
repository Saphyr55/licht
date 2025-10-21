#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/renderer/draw_item.hpp"
#include "licht/renderer/mesh/static_mesh.hpp"
#include "licht/rhi/device_memory_uploader.hpp"


namespace licht {

class StaticMeshCompiler {
public:
    virtual Array<DrawItem> compile(const SharedRef<RHIDevice>& device,
                                    RHIDeviceMemoryUploader& uploader,
                                    StaticMesh& mesh) = 0;
};

}  //namespace licht