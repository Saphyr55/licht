#include "licht/core/time/delta_timer.hpp"
#include "licht/core/platform/platform_time.hpp"

namespace licht {

float64 DeltaTimer::tick() {
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

float64 DeltaTimer::limit(float64 delta_time, float32 target_frame_rate) {
    if (delta_time < target_frame_rate) {
        float64 sleep_time = target_frame_rate - delta_time;
        platform_delay(sleep_time);
        return tick();
    }
    return delta_time;
}

}  //namespace licht