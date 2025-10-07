#include "licht/rhi/texture.hpp"

namespace licht {

const RHITextureDescription& RHITexture::get_description() const {
    return description_;
}

}  //namespace licht
