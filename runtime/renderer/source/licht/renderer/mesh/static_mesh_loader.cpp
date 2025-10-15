#include "licht/renderer/mesh/static_mesh_loader.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/renderer/mesh/static_mesh.hpp"
#include "licht/rhi/rhi_types.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

namespace licht {

// Source: https://github.com/syoyo/tinygltf/blob/release/examples/basic/main.cpp
void dbgModel(tinygltf::Model& model) {
    for (tinygltf::Mesh& mesh : model.meshes) {
        std::cout << "mesh : " << mesh.name << std::endl;
        for (tinygltf::Primitive& primitive : mesh.primitives) {
            const tinygltf::Accessor& indexAccessor =  model.accessors[primitive.indices];

            std::cout << "index_accessor: count " << indexAccessor.count << ", type " << indexAccessor.componentType << std::endl;

            tinygltf::Material& mat = model.materials[primitive.material];
            for (auto& mats : mat.values) {
                std::cout << "mat : " << mats.first.c_str() << std::endl;
            }

            for (tinygltf::Image& image : model.images) {
                std::cout << "image name : " << image.uri << std::endl;
                std::cout << "  size : " << image.image.size() << std::endl;
                std::cout << "  w/h : " << image.width << "/" << image.height << std::endl;
            }

            std::cout << "indices : " << primitive.indices << std::endl;
            std::cout << "mode     : " << "(" << primitive.mode << ")" << std::endl;

            for (auto& attrib : primitive.attributes) {
                std::cout << "attribute : " << attrib.first.c_str() << std::endl;
            }
        }
    }
}

static RHIFormat find_format(const tinygltf::Image& image) {
    RHIFormat format = RHIFormat::RGB8sRGB;
    switch (image.component) {
        case 1:
            format = RHIFormat::R8sRGB;
            break;
        case 2:
            format = RHIFormat::RG8sRGB;
            break;
        case 3:
            format = RHIFormat::RGB8sRGB;
            break;
        case 4:
            format = RHIFormat::RGBA8sRGB;
            break;
        default:
            LLOG_WARN("[GLTF]", vformat("Unsupported image component count: %d", image.component));
            break;
    }

    if (image.bits == 16) {
        switch (image.component) {
            case 1:
                format = RHIFormat::R16;
                break;
            case 2:
                format = RHIFormat::RG16;
                break;
            case 3:
                format = RHIFormat::RGB16;
                break;
            case 4:
                format = RHIFormat::RGBA16;
                break;
            default:
                break;
        }
    }

    if (image.bits == 32) {
        switch (image.component) {
            case 1:
                format = RHIFormat::RG32Float;
                break;
            case 2:
                format = RHIFormat::RG32Float;
                break;
            case 3:
                format = RHIFormat::RGB32Float;
                break;
            case 4:
                format = RHIFormat::RGBA32Float;
                break;
            default:
                break;
        }
    }
    return format;
}

static HashMap<String, SharedRef<tinygltf::Model>> registry;

void gltf_static_meshes_unload(StringRef filepath) {
    registry.remove(String(filepath));
}

StaticSubMesh::Buffer gltf_get_accessor_data(tinygltf::Model& model, int32 accessor_index) {
    if (accessor_index < 0 || accessor_index >= model.accessors.size()) {
        return StaticSubMesh::Buffer();
    }

    const tinygltf::Accessor& accessor = model.accessors[accessor_index];
    if (accessor.bufferView < 0 || accessor.bufferView >= model.bufferViews.size()) {
        return StaticSubMesh::Buffer();
    } 

    const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
    if (buffer_view.buffer < 0 || buffer_view.buffer > model.buffers.size()) {
        return StaticSubMesh::Buffer();
    } 

    tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];
    size_t size_in_bytes = accessor.count * accessor.ByteStride(buffer_view);

    return StaticSubMesh::Buffer(buffer.data.data() + buffer_view.byteOffset + accessor.byteOffset, size_in_bytes);
}

template<typename T>
Array<uint32> gltf_get_indices_type(tinygltf::Model& model, int32 accessor_index) {
    StaticSubMesh::Buffer index_bytes = gltf_get_accessor_data(model, accessor_index);

    Array<uint32> indices;

    const tinygltf::Accessor& accessor = model.accessors[accessor_index];
    indices.reserve(accessor.count);

    const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];

    const size_t stride = accessor.ByteStride(buffer_view);
    for (size_t i = 0; i < accessor.count; i++) {
        const T* index_data = reinterpret_cast<const T*>(index_bytes.data() + i * stride);
        indices.push_back(static_cast<uint32>(*index_data));
    }

    return indices;
}

Array<uint32> gltf_get_indices(tinygltf::Model& model, const tinygltf::Primitive& primitive) {
    tinygltf::Accessor& accessor = model.accessors[primitive.indices];
    switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            return gltf_get_indices_type<int8>(model, primitive.indices);
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            return gltf_get_indices_type<uint8>(model, primitive.indices);
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            return gltf_get_indices_type<int16>(model, primitive.indices);
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            return gltf_get_indices_type<uint16>(model, primitive.indices);
        case TINYGLTF_COMPONENT_TYPE_INT:
            return gltf_get_indices_type<int32>(model, primitive.indices);
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            return gltf_get_indices_type<uint32>(model, primitive.indices);
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            return gltf_get_indices_type<uint32>(model, primitive.indices);
        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
            return gltf_get_indices_type<uint32>(model, primitive.indices);
    }
    return {};
}

void gltf_create_primitive(tinygltf::Model& model, const tinygltf::Primitive& primitive, StaticSubMesh& out_submesh) {
    out_submesh.positions = gltf_get_accessor_data(model, primitive.attributes.at("POSITION"));

    if (primitive.indices >= 0) {
        out_submesh.indices = gltf_get_indices(model, primitive);
    } else {
        out_submesh.indices.reserve(out_submesh.positions.size());
        for (uint32 i = 0; i < out_submesh.positions.size(); ++i) {
            out_submesh.indices.push_back(i);
        }
    }

    auto normals_it = primitive.attributes.find("NORMAL");
    if (normals_it != primitive.attributes.end()) {
        out_submesh.normals = gltf_get_accessor_data(model, normals_it->second);
    }

    auto texcoord0_it = primitive.attributes.find("TEXCOORD_0");
    if (texcoord0_it != primitive.attributes.end()) {
        out_submesh.uv_textures = gltf_get_accessor_data(model, texcoord0_it->second);
    }
}


void gltf_create_meshes(tinygltf::Model& model, Array<StaticMesh>& out_meshes) {
    out_meshes.reserve(model.meshes.size());
    for (const tinygltf::Mesh& gltf_mesh : model.meshes) {
        StaticMesh mesh;
        size_t primitive_count = 0;
        for (const tinygltf::Primitive& primitive : gltf_mesh.primitives) {
            StaticSubMesh submesh;
            gltf_create_primitive(model, primitive, submesh);

            if (primitive.material >= 0 && primitive.material < model.materials.size()) {
                tinygltf::Material& gltf_material = model.materials[primitive.material];
                tinygltf::Texture& base_color_texture = model.textures[gltf_material.pbrMetallicRoughness.baseColorTexture.index];
                tinygltf::Image& image = model.images[base_color_texture.source];
                submesh.material.diffuse_texture.data = Array<uint8>(image.image.data(), image.image.size());
                submesh.material.diffuse_texture.format = find_format(image);
                submesh.material.diffuse_texture.width = image.width; 
                submesh.material.diffuse_texture.height = image.height;
            }

            mesh.append_submesh(submesh);
        }
        out_meshes.append(mesh);
    }
}

Array<StaticMesh> gltf_static_meshes_load(StringRef filepath) {
    Array<StaticMesh> meshes;

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath.data());
    
    gltf_create_meshes(model, meshes);

    return meshes;
}

}  //namespace licht