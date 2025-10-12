#pragma once

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

namespace licht {

/**
 * @brief Initializes the platform-specific time system.
 * This function should be called once at application startup. 
 * It may initialize high-resolution timers or other platform-specific time-related resources.
 */
LICHT_CORE_API void platform_time_init();

/**
 * @brief Pauses execution for a specified duration.
 * @param milliseconds The amount of time, in milliseconds, to delay execution.
 */
LICHT_CORE_API void platform_delay(float64 milliseconds);

/**
 * @brief Gets a low-resolution tick count since the system started.
 * The unit is usually milliseconds, but the resolution is platform-dependent.
 * This is generally not suitable for precise delta time calculation.
 * @return uint64 The number of ticks (usually milliseconds) since system start.
 */
LICHT_CORE_API uint64 platform_get_ticks();

/**
 * @brief Gets a high-resolution, monotonically increasing performance counter value.
 * This counter is used in conjunction with @ref platform_get_performance_frequency() to calculate precise elapsed time.
 * @return uint64 The current value of the high-resolution performance counter.
 */
LICHT_CORE_API uint64 platform_get_performance_counter();

/**
 * @brief Gets the frequency of the performance counter.
 * This value is the number of counts per second for the counter returned by @ref platform_get_performance_counter().
 * @return uint64 The performance counter's frequency in counts per second (Hz).
 */
LICHT_CORE_API uint64 platform_get_performance_frequency();

/**
 * @brief Gets the current time since the application or system started.
 * The resolution may vary, but the return value is typically in seconds.
 * @return float64 The elapsed time in seconds.
 */
LICHT_CORE_API float64 platform_get_time();

/**
 * @brief Gets the current high-resolution time since the application or system started.
 * This uses the most precise available timer on the platform, typically based on the performance counter, 
 * and returns the result in seconds.
 * @return float64 The high-resolution elapsed time in seconds.
 */
LICHT_CORE_API float64 platform_get_high_resolution_time();

/**
 * @brief Calculates the time difference between two performance counter values.
 * This is the preferred method for calculating delta time in a game or simulation loop.
 * @param previous_counter The performance counter value from the previous frame/update.
 * @param current_counter The performance counter value from the current frame/update.
 * @return float64 The elapsed time (delta time) in seconds.
 */
LICHT_CORE_API float64 platform_calculate_delta_time(uint64 previous_counter, uint64 current_counter);

}  //namespace licht