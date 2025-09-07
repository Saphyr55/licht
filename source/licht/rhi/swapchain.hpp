#pragma once

#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/rhi.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/texture.hpp"

namespace licht {

class RHIFrameContext {
public:
    Array<RHISemaphoreHandle> frame_available_semaphores;
    Array<RHISemaphoreHandle> render_finished_semaphores;
    Array<RHIFenceHandle> in_flight_fences;
    Array<RHIFenceHandle*> frame_in_flight_fences;

    uint32 frame_width;
    uint32 frame_height;

    uint32 frame_index = 0;
    uint32 current_frame = 0;
    uint32 frame_count = 2;

    bool success;
    bool suboptimal;
    bool out_of_date;

public:
    inline RHISemaphoreHandle current_frame_available_semaphore();
    
    inline RHISemaphoreHandle current_render_finished_semaphore();
    
    inline RHIFenceHandle current_in_flight_fence();    

    inline void next_frame() {
        current_frame = (current_frame + 1) % frame_count;
        success = false;
        suboptimal = false;
        out_of_date = false;
    }
};

class RHISwapchain {
public:
    virtual void acquire_next_frame(RHIFrameContext& context) = 0;

    virtual uint32 get_width() = 0;

    virtual uint32 get_height() = 0;

    virtual RHIFormat get_format() = 0;

    virtual const Array<RHITextureViewHandle>& get_texture_views() = 0;
};

using RHISwapchainHandle = SharedRef<RHISwapchain>;

inline RHISemaphoreHandle RHIFrameContext::current_frame_available_semaphore() {
    return frame_available_semaphores[current_frame]; 
}

inline RHISemaphoreHandle RHIFrameContext::current_render_finished_semaphore() {
    return frame_available_semaphores[current_frame];
}

inline RHIFenceHandle RHIFrameContext::current_in_flight_fence() {
    return in_flight_fences[current_frame];
}

}  //namespace licht