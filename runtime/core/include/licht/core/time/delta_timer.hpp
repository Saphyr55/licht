#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

namespace licht {

class LICHT_CORE_API DeltaTimer {
public:
    float64 tick();
    
    float64 limit(float64 delta_time, float32 target_frame_rate); 
    
    inline float64 get_delta_time() const {
        return delta_time_;
    }

    inline void set_time_scale(float64 scale) {
        time_scale_ = scale;
    }

    inline float64 get_time_scale() const {
        return time_scale_;
    }

    inline void reset() {
        first_call_ = true;
        delta_time_ = 0.0;
    }

public:
    DeltaTimer() = default;
    ~DeltaTimer() = default;

private:
    size_t last_counter_ = 0;
    bool first_call_ = true;
    float64 delta_time_ = 0.0;
    float64 time_scale_ = 1.0f;
};

}  //namespace licht