#pragma once

namespace licht {

struct Viewport {
    float x;
    float y;
    float width;
    float height;
    float min_depth;
    float max_depth;
};

struct Rect2D {
    float x;
    float y;
    float width;
    float height;
};

}  //namespace licht