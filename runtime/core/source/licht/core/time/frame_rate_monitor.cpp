#include "licht/core/time/frame_rate_monitor.hpp"

namespace licht {

bool FrameRateMonitor::update(float64 current_frame_time) {
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

}  //namespace licht