#pragma once

#include "licht/core/platform/platform_time.hpp"
#include "ludo_exports.hpp"

namespace licht {

class LUDO_API DeltaTimer {
private:
    size_t last_counter_ = 0;
    bool first_call_ = true;
    float64 delta_time_ = 0.0;
    float64 time_scale_ = 1.0f;

public:
    DeltaTimer() = default;
    
    float64 tick() {
        size_t current_counter = platform_get_performance_counter();
        
        if (first_call_) {
            last_counter_ = current_counter;
            first_call_ = false;
            delta_time_ = 0.0;
            return 0.0;
        }
        
        delta_time_ = platform_calculate_delta_time(last_counter_, current_counter);
        last_counter_ = current_counter;
        
        return delta_time_ * time_scale_;
    }
    
    float64 get_delta_time() const { 
        return delta_time_; 
    }
    
    void set_time_scale(float64 scale) { 
        time_scale_ = scale; 
    }

    float64 get_time_scale() const { 
        return time_scale_; 
    }
    
    void reset() {
        first_call_ = true;
        delta_time_ = 0.0;
    }
};

class LUDO_API FrameRateMonitor {
public:
    FrameRateMonitor() {
        last_time_ = platform_get_ticks();
    }

    bool update(float64 current_frame_time = 0.0) {
        frame_count_++;

        if (current_frame_time > 0.0) {
            if (min_frame_time_ == 0.0 || current_frame_time < min_frame_time_) {
                min_frame_time_ = current_frame_time;
            }
            if (current_frame_time > max_frame_time_) {
                max_frame_time_ = current_frame_time;
            }
        }

        size_t current_time = platform_get_ticks();
        size_t elapsed = current_time - last_time_;

        if (elapsed >= 1000) {
            fps_ = static_cast<float64>(frame_count_) / (static_cast<float64>(elapsed) / 1000.0);
            frame_time_avg_ = 1000.0 / fps_;

            frame_count_ = 0;
            last_time_ = current_time;
            return true;
        }
        return false;
    }

    void reset() {
        frame_count_ = 0;
        last_time_ = platform_get_ticks();
        fps_ = 0.0;
        frame_time_avg_ = 0.0;
        min_frame_time_ = 0.0;
        max_frame_time_ = 0.0;
    }

    float64 get_fps() const { 
        return fps_; 
    }

    float64 get_average_frame_time() const { 
        return frame_time_avg_; 
    }

    float64 get_min_frame_time() const { 
        return min_frame_time_; 
    }

    float64 get_max_frame_time() const { 
        return max_frame_time_; 
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