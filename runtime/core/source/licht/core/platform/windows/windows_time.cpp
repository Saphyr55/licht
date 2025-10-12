#ifdef _WIN32

#include <Windows.h>
#include "licht/core/platform/platform_time.hpp"

namespace licht {

void platform_time_init() {
}

void platform_delay(float64 seconds) {
    DWORD milliseconds = static_cast<DWORD>(seconds * 1000.0);
    ::Sleep(milliseconds);
}

uint64 platform_get_ticks() {
    return static_cast<uint64>(GetTickCount64());
}

uint64 platform_get_performance_counter() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<uint64>(counter.QuadPart);
}

uint64 platform_get_performance_frequency() {
    static LARGE_INTEGER frequency = {0};
    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    }
    return static_cast<size_t>(frequency.QuadPart);
}

float64 platform_get_time() {
    return static_cast<float64>(platform_get_ticks()) / 1000.0;
}

float64 platform_get_high_resolution_time() {
    LARGE_INTEGER counter, frequency;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return static_cast<float64>(counter.QuadPart) / static_cast<float64>(frequency.QuadPart);
}

float64 platform_calculate_delta_time(uint64 previous_counter, uint64 current_counter) {
    size_t frequency = platform_get_performance_frequency();
    if (frequency == 0) {
        return 0.0;
    }

    return static_cast<float64>(current_counter - previous_counter) / static_cast<float64>(frequency);
}

}  //namespace licht

#endif