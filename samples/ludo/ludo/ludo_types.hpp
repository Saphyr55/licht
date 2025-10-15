#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/math/matrix4.hpp"

namespace licht {

struct UniformBufferObject {
    alignas(16) Matrix4f model = Matrix4f::identity();
    alignas(16) Matrix4f view = Matrix4f::identity();
    alignas(16) Matrix4f proj = Matrix4f::identity();

    UniformBufferObject() = default;
};

struct RenderCube {
    
    Array<Vector3f> positions = {
        Vector3f(-0.5f, -0.5f, 0.5f),  // 0
        Vector3f(0.5f, -0.5f, 0.5f),   // 1
        Vector3f(0.5f, 0.5f, 0.5f),    // 2
        Vector3f(-0.5f, 0.5f, 0.5f),   // 3

        Vector3f(0.5f, -0.5f, -0.5f),   // 4
        Vector3f(-0.5f, -0.5f, -0.5f),  // 5
        Vector3f(-0.5f, 0.5f, -0.5f),   // 6
        Vector3f(0.5f, 0.5f, -0.5f),    // 7

        Vector3f(-0.5f, -0.5f, -0.5f),  // 8
        Vector3f(-0.5f, -0.5f, 0.5f),   // 9
        Vector3f(-0.5f, 0.5f, 0.5f),    // 10
        Vector3f(-0.5f, 0.5f, -0.5f),   // 11

        Vector3f(0.5f, -0.5f, 0.5f),   // 12
        Vector3f(0.5f, -0.5f, -0.5f),  // 13
        Vector3f(0.5f, 0.5f, -0.5f),   // 14
        Vector3f(0.5f, 0.5f, 0.5f),    // 15

        Vector3f(-0.5f, 0.5f, 0.5f),   // 16
        Vector3f(0.5f, 0.5f, 0.5f),    // 17
        Vector3f(0.5f, 0.5f, -0.5f),   // 18
        Vector3f(-0.5f, 0.5f, -0.5f),  // 19

        Vector3f(-0.5f, -0.5f, -0.5f),  // 20
        Vector3f(0.5f, -0.5f, -0.5f),   // 21
        Vector3f(0.5f, -0.5f, 0.5f),    // 22
        Vector3f(-0.5f, -0.5f, 0.5f),   // 23
    };

    Array<Vector2f> UVs = {
        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),

        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),

        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),

        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),

        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),

        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 1.0f),
    };

    Array<uint32> indices = {
        // Forward
        0, 1, 2, 2, 3, 0,
        // Back
        4, 5, 6, 6, 7, 4,
        // Left
        8, 9, 10, 10, 11, 8,
        // Right
        12, 13, 14, 14, 15, 12,
        // Top
        16, 17, 18, 18, 19, 16,
        // Bottom
        20, 21, 22, 22, 23, 20};
};

}
