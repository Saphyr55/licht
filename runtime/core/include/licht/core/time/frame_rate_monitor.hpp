#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/platform/platform_time.hpp"
#include "licht/core/core_exports.hpp"

namespace licht {

class LICHT_CORE_API FrameRateMonitor {
public:
    bool update(float64 current_frame_time = 0.0);

public:
    inline void reset() {
        frame_count_ = 0;
        last_time_ = platform_get_ticks();
        fps_ = 0.0;
        frame_time_avg_ = 0.0;
        min_frame_time_ = 0.0;
        max_frame_time_ = 0.0;
    }

    inline float64 get_fps() const { 
        return fps_; 
    }

    inline float64 get_average_frame_time() const { 
        return frame_time_avg_; 
    }

    inline float64 get_min_frame_time() const { 
        return min_frame_time_; 
    }

    inline float64 get_max_frame_time() const { 
        return max_frame_time_; 
    }
    
public:
    FrameRateMonitor() {
        last_time_ = platform_get_ticks();
    }

private:
    float64 fps_ = 0.0;
    float64 frame_time_avg_ = 0.0;
    float64 min_frame_time_ = 0.0;
    float64 max_frame_time_ = 0.0;
    size_t frame_count_ = 0;
    size_t last_time_ = 0;
};

}  //namespace licht