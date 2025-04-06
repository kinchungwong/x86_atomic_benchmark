#ifndef NANO_TIMER_DETAIL_CPP
#define NANO_TIMER_DETAIL_CPP

#include <type_traits>
#include <limits>
#include <mutex> // for std::call_once
// #include <chrono> // already in nano_timer_detail.h
#include "nano_timer/nano_timer_detail.h"

namespace timing {
namespace detail {

const std::chrono::steady_clock::time_point& stc_get_start()
{
    static std::once_flag _stc_start_flag;
    static std::chrono::steady_clock::time_point _stc_start;
    std::call_once(_stc_start_flag, []()
    {
        _stc_start = std::chrono::steady_clock::now();
    });
    return _stc_start;
}

std::chrono::steady_clock::duration stc_get_elapsed()
{
    auto now = std::chrono::steady_clock::now();
    return now - stc_get_start();
}

uint64_t stc_get_elapsed_nanos()
{
    using integral_nanos = std::chrono::nanoseconds;
    auto elaps = stc_get_elapsed();
    auto elaps_nanos = std::chrono::duration_cast<integral_nanos>(elaps);
    int64_t s64_nanos = elaps_nanos.count();
    return (s64_nanos < 0) ? 0 : static_cast<uint64_t>(s64_nanos);
}

days_seconds_nanos stc_get_elapsed_days_seconds_nanos()
{
    using integral_seconds = std::chrono::seconds;
    using integral_nanos = std::chrono::nanoseconds;
    static_assert(std::is_integral<integral_seconds::rep>::value, "Integral seconds type is not integral");
    static_assert(std::is_integral<integral_nanos::rep>::value, "Integral seconds type is not integral");
    static constexpr uint32_t seconds_per_day = 24u * 60u * 60u;
    auto elaps = stc_get_elapsed();
    auto elaps_seconds = std::chrono::duration_cast<integral_seconds>(elaps);
    auto elaps_nanos = std::chrono::duration_cast<integral_nanos>(elaps - elaps_seconds);
    int64_t s64_seconds = elaps_seconds.count();
    if (s64_seconds < 0 || s64_seconds >= (std::numeric_limits<uint32_t>::max)())
    {
        return days_seconds_nanos{0u, 0u, 0u};
    }
    uint32_t u32_seconds = static_cast<uint32_t>(s64_seconds);
    uint32_t u32_days = static_cast<uint32_t>(u32_seconds / seconds_per_day);
    u32_seconds %= seconds_per_day;
    int64_t s64_nanos = elaps_nanos.count();
    static constexpr int32_t nanos_per_second = 1000u * 1000u * 1000u;
    if (s64_nanos < 0 || s64_nanos >= nanos_per_second)
    {
        return days_seconds_nanos{0u, 0u, 0u};
    }
    uint32_t u32_nanos = static_cast<uint32_t>(s64_nanos);
    return days_seconds_nanos{ u32_days, u32_seconds, u32_nanos };
}

}} // namespace timing::detail

#endif // NANO_TIMER_DETAIL_CPP
