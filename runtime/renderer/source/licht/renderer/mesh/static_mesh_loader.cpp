#include "licht/renderer/mesh/static_mesh_loader.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/renderer/material/material.hpp"
#include "licht/renderer/mesh/static_mesh.hpp"
#include "licht/rhi/rhi_types.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

namespace licht {

static RHIFormat find_format(const tinygltf::Image& image, const bool normal = false) {
    RHIFormat format = RHIFormat::RGB8sRGB;
    if (normal) {
        switch (image.component) {
            case 1:
                format = RHIFormat::R8;
                break;
            case 2:
                format = RHIFormat::RG8;
                break;
            case 3:
                format = RHIFormat::RGB8;
                break;
            case 4:
                format = RHIFormat::RGBA8;
                break;
            default:
                LLOG_WARN("[GLTF]", vformat("Unsupported image component count: %d", image.component));
                break;
        }
    } else {
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

static StaticSubMesh::Buffer gltf_get_accessor_data(tinygltf::Model& model, int32 accessor_index) {
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

template <typename T>
static Array<uint32> gltf_get_indices_type(tinygltf::Model& model, int32 accessor_index) {
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

static Array<uint32> gltf_get_indices(tinygltf::Model& model, const tinygltf::Primitive& primitive) {
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

static void recompute_normals(StaticSubMesh& mesh) {
    size_t vertex_count = mesh.positions.size() / sizeof(Vector3f);
    size_t index_count  = mesh.indices.size();

    Vector3f* positions = reinterpret_cast<Vector3f*>(mesh.positions.data());
    Array<Vector3f> normals;
    normals.resize(vertex_count, Vector3f(0.0));

    for (size_t i = 0; i < index_count; i += 3) {
        uint32 i0 = mesh.indices[i];
        uint32 i1 = mesh.indices[i + 1];
        uint32 i2 = mesh.indices[i + 2];

        Vector3f v0 = positions[i0];
        Vector3f v1 = positions[i1];
        Vector3f v2 = positions[i2];

        Vector3f e1 = v1 - v0;
        Vector3f e2 = v2 - v0;

        Vector3f n = Vector3f::normalize(Vector3f::cross(e1, e2));

        normals[i0] += n;
        normals[i1] += n;
        normals[i2] += n;
    }

    for (Vector3f& n : normals) {
        n = Vector3f::normalize(n);
    }

    mesh.normals = Array<uint8>(reinterpret_cast<uint8*>(normals.data()), vertex_count * sizeof(Vector3f));
}

static void recompute_tangents(StaticSubMesh& mesh) {
    size_t vertex_count = mesh.positions.size() / sizeof(Vector3f);
    size_t index_count  = mesh.indices.size();

    Vector3f* positions = reinterpret_cast<Vector3f*>(mesh.positions.data());
    Vector3f* normals   = reinterpret_cast<Vector3f*>(mesh.normals.data());
    Vector2f* uvs       = reinterpret_cast<Vector2f*>(mesh.uv_textures.data());

    Array<Vector3f> tan1;
    tan1.resize(vertex_count, Vector3f(0.0));

    Array<Vector3f> tan2;
    tan2.resize(vertex_count, Vector3f(0.0));

    Array<Vector4f> tangents;
    tangents.resize(vertex_count, Vector4f(0.0));

    for (size_t i = 0; i < index_count; i += 3) {
        uint32 i0 = mesh.indices[i];
        uint32 i1 = mesh.indices[i + 1];
        uint32 i2 = mesh.indices[i + 2];

        const Vector3f& v0 = positions[i0];
        const Vector3f& v1 = positions[i1];
        const Vector3f& v2 = positions[i2];

        const Vector2f& w0 = uvs[i0];
        const Vector2f& w1 = uvs[i1];
        const Vector2f& w2 = uvs[i2];

        float32 x1 = v1.x - v0.x;
        float32 x2 = v2.x - v0.x;
        float32 y1 = v1.y - v0.y;
        float32 y2 = v2.y - v0.y;
        float32 z1 = v1.z - v0.z;
        float32 z2 = v2.z - v0.z;

        float32 s1 = w1.x - w0.x;
        float32 s2 = w2.x - w0.x;
        float32 t1 = w1.y - w0.y;
        float32 t2 = w2.y - w0.y;

        float32 r = (s1 * t2 - s2 * t1);
        if (std::abs(r) < 1e-8f) r = 1.0f;
        r = 1.0f / r;

        Vector3f sdir = {(t2 * x1 - t1 * x2) * r,
                         (t2 * y1 - t1 * y2) * r,
                         (t2 * z1 - t1 * z2) * r};
        Vector3f tdir = {(s1 * x2 - s2 * x1) * r,
                         (s1 * y2 - s2 * y1) * r,
                         (s1 * z2 - s2 * z1) * r};

        tan1[i0] += sdir;
        tan1[i1] += sdir;
        tan1[i2] += sdir;

        tan2[i0] += tdir;
        tan2[i1] += tdir;
        tan2[i2] += tdir;
    }

    for (size_t i = 0; i < vertex_count; ++i) {
        const Vector3f& n = normals[i];
        const Vector3f& t = tan1[i];

        Vector3f tangent = Vector3f::normalize(t - n * Vector3f::dot(n, t));

        float32 w = (Vector3f::dot(Vector3f::cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;

        tangents[i] = Vector4f(tangent.x, tangent.y, tangent.z, w);
    }

    mesh.tangents = Array<uint8>(reinterpret_cast<uint8*>(tangents.data()), vertex_count * sizeof(Vector4f));
}

static void gltf_create_primitive(tinygltf::Model& model, const tinygltf::Primitive& primitive, StaticSubMesh& out_submesh) {
    out_submesh.positions = gltf_get_accessor_data(model, primitive.attributes.at("POSITION"));

    using attributes_type = decltype(primitive.attributes);

    if (primitive.indices >= 0) {
        out_submesh.indices = gltf_get_indices(model, primitive);
    } else {
        out_submesh.indices.reserve(out_submesh.positions.size());
        for (uint32 i = 0; i < out_submesh.positions.size(); ++i) {
            out_submesh.indices.push_back(i);
        }
    }

    attributes_type::const_iterator uv_it = primitive.attributes.find("TEXCOORD_0");
    if (uv_it != primitive.attributes.end()) {
        out_submesh.uv_textures = gltf_get_accessor_data(model, uv_it->second);
    }

    attributes_type::const_iterator normals_it = primitive.attributes.find("NORMAL");
    if (normals_it != primitive.attributes.end()) {
        out_submesh.normals = gltf_get_accessor_data(model, normals_it->second);
    }
    
    if (out_submesh.normals.empty()) {
        recompute_normals(out_submesh);
    }

    attributes_type::const_iterator tangents_it = primitive.attributes.find("TANGENT");
    if (tangents_it != primitive.attributes.end()) {
        out_submesh.tangents = gltf_get_accessor_data(model, tangents_it->second);
    } 
    
    if (out_submesh.tangents.empty()) {
        recompute_tangents(out_submesh);
    }

}

static void gltf_create_meshes(tinygltf::Model& model, Array<StaticMesh>& out_meshes) {
    out_meshes.reserve(model.meshes.size());
    for (const tinygltf::Mesh& gltf_mesh : model.meshes) {
        StaticMesh mesh;
        size_t primitive_count = 0;
        for (const tinygltf::Primitive& primitive : gltf_mesh.primitives) {
            StaticSubMesh submesh;
            gltf_create_primitive(model, primitive, submesh);

            if (primitive.material >= 0 && primitive.material < model.materials.size()) {
                tinygltf::Material& gltf_material = model.materials[primitive.material];
                tinygltf::PbrMetallicRoughness& pbr = gltf_material.pbrMetallicRoughness;
                tinygltf::Texture& base_color_texture = model.textures[pbr.baseColorTexture.index];
                tinygltf::Image& base_color_image = model.images[base_color_texture.source];

                submesh.material.diffuse_texture.data = Array<uint8>(base_color_image.image.data(), base_color_image.image.size());
                submesh.material.diffuse_texture.format = find_format(base_color_image, true);
                submesh.material.diffuse_texture.width = base_color_image.width;
                submesh.material.diffuse_texture.height = base_color_image.height;
                submesh.material.diffuse_factor = { static_cast<float32>(pbr.baseColorFactor[0]), 
                                                    static_cast<float32>(pbr.baseColorFactor[1]), 
                                                    static_cast<float32>(pbr.baseColorFactor[2]), 
                                                    static_cast<float32>(pbr.baseColorFactor[3]) };
                
                if (gltf_material.normalTexture.index >= 0) {
                    tinygltf::Texture& normal_texture = model.textures[gltf_material.normalTexture.index];
                    tinygltf::Image& normal_image = model.images[normal_texture.source];
                    submesh.material.normal_texture.data = Array<uint8>(normal_image.image.data(), normal_image.image.size());
                    submesh.material.normal_texture.format = find_format(normal_image, true);
                    submesh.material.normal_texture.width = normal_image.width;
                    submesh.material.normal_texture.height = normal_image.height;
                }

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