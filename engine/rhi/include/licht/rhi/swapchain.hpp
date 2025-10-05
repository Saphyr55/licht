#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/rhi/rhi_fowards.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

class RHIFrameContext {
public:
    Array<RHISemaphore*> frame_available_semaphores;
    Array<RHISemaphore*> render_finished_semaphores;
    Array<RHIFence*> in_flight_fences;
    Array<RHIFence**> frame_in_flight_fences;

    uint32 frame_width;
    uint32 frame_height;

    uint32 frame_index = 0;
    uint32 current_frame = 0;
    uint32 frame_count = 3;

    bool success = false;
    bool suboptimal = false;
    bool out_of_date = false;

public:
    RHISemaphore* current_frame_available_semaphore();

    RHISemaphore* current_render_finished_semaphore();

    RHIFence* current_in_flight_fence();    

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

    virtual const Array<RHITextureView*>& get_texture_views() = 0;
};


inline RHISemaphore* RHIFrameContext::current_frame_available_semaphore() {
    return frame_available_semaphores[current_frame]; 
}

inline RHISemaphore* RHIFrameContext::current_render_finished_semaphore() {
    return frame_available_semaphores[current_frame];
}

inline RHIFence* RHIFrameContext::current_in_flight_fence() {
    return in_flight_fences[current_frame];
}

}  //namespace licht