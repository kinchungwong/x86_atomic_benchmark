#ifndef NANO_TIMER_DETAIL_H
#define NANO_TIMER_DETAIL_H

#include <chrono>

namespace timing {
namespace detail {

struct days_seconds_nanos
{
    uint32_t days;
    uint32_t seconds;
    uint32_t nanos;
};

const std::chrono::steady_clock::time_point& stc_get_start();
std::chrono::steady_clock::duration stc_get_elapsed();
uint64_t stc_get_elapsed_nanos();
days_seconds_nanos stc_get_elapsed_days_seconds_nanos();

}} // namespace timing::detail

#endif // NANO_TIMER_DETAIL_H
