#ifndef NANO_TIMER_IMPL_CPP
#define NANO_TIMER_IMPL_CPP

#include "nano_timer/nano_timer_impl.h"
#include "nano_timer/nano_timer_detail.h"

namespace timing {

NanoTimer::NanoTimer(bool start_now)
    : _summed(0u)
    , _started(0u)
    , _is_running(false)
    , _is_invalid(false)
{
    if (start_now)
    {
        start();
    }
}

void NanoTimer::start()
{
    if (_is_running)
    {
        return;
    }
    _is_running = true;
    _started = detail::stc_get_elapsed_nanos();
}

void NanoTimer::stop()
{
    if (!_is_running)
    {
        return;
    }
    _is_running = false;
    uint64_t now = detail::stc_get_elapsed_nanos();
    uint64_t diff = now - _started;
    if (static_cast<int64_t>(diff) < 0u)
    {
        _is_invalid = true;
        diff = 0u;
    }
    _summed += diff;
    _started = 0u;
}

uint64_t NanoTimer::current_nanos() const
{
    uint64_t diff = 0u;
    if (_is_running)
    {
        uint64_t now = detail::stc_get_elapsed_nanos();
        diff = now - _started;
    }
    return _summed + diff;
}

double NanoTimer::current_seconds() const
{
    return 1.0e-9 * current_nanos();
}

NanoTimer::~NanoTimer() = default;
NanoTimer::NanoTimer(const NanoTimer&) = default;
NanoTimer::NanoTimer(NanoTimer&&) = default;
NanoTimer& NanoTimer::operator=(const NanoTimer&) = default;
NanoTimer& NanoTimer::operator=(NanoTimer&&) = default;

} // namespace timing

#endif // NANO_TIMER_IMPL_CPP
