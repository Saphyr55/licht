#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/renderer/renderer_exports.hpp"

namespace licht {

class StaticMesh;
class RHIBufferPool;

LICHT_RENDERER_API Array<StaticMesh> gltf_static_meshes_load(StringRef filepath);

LICHT_RENDERER_API void gltf_static_meshes_unload(StringRef filepath);

}