#ifndef NANO_TIMER_EZ_TIMER_CPP
#define NANO_TIMER_EZ_TIMER_CPP

#include <cstdint>
#include <string>
#include <functional>
#include <iostream>
#include "nano_timer/ez_timer.h"

namespace timing {

EzTimer::EzTimer(const std::string& what, bool start)
    : _what(what)
    , _timer(start)
{
}

EzTimer::~EzTimer()
{
    _timer.stop();
    if (_callback)
    {
        _callback(*this);
        return;
    }
    static constexpr size_t buf_sz = 1024u;
    char buffer[buf_sz];
    if (!_timer.is_valid())
    {
        snprintf(buffer, sizeof(buffer),
            "[EzTimer: %s] invalid timer state.",
            _what.c_str());
        buffer[buf_sz - 1u] = '\0';
        std::cerr << buffer << std::endl;
        return;
    }
    uint64_t nanos = _timer.current_nanos();
    double seconds = _timer.current_seconds();
    snprintf(buffer, sizeof(buffer),
        "[EzTimer: %s] %lu (ns), %.6f (s)",
        _what.c_str(), nanos, seconds);
    buffer[buf_sz - 1u] = '\0';
    std::cout << buffer << std::endl;
}

void EzTimer::set_callback(std::function<void(const EzTimer&)> callback)
{
    _callback = callback;
}

} // namespace timing

#endif // NANO_TIMER_EZ_TIMER_CPP
