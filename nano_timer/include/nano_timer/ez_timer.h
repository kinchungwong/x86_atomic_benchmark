#ifndef NANO_TIMER_EZ_TIMER_H
#define NANO_TIMER_EZ_TIMER_H

#include <cstdint>
#include <string>
#include <functional>
#include <iostream>
#include "nano_timer/nano_timer_impl.h"

namespace timing {

class EzTimer
{
private:
    std::string _what;
    NanoTimer _timer;
    std::function<void(const EzTimer&)> _callback;

public:
    explicit EzTimer(const std::string& what, bool start = true);

    EzTimer(EzTimer&&) = default;
    EzTimer& operator=(EzTimer&&) = default;
    ~EzTimer();

    void set_callback(std::function<void(const EzTimer&)> callback);

    void start()
    {
        _timer.start();
    }

    void stop()
    {
        _timer.stop();
    }

    uint64_t current_nanos() const
    {
        return _timer.current_nanos();
    }

    double current_seconds() const
    {
        return _timer.current_seconds();
    }

    std::string what() const
    {
        return _what;
    }

private:
    EzTimer(const EzTimer&) = delete;
    EzTimer& operator=(const EzTimer&) = delete;
};

} // namespace timing

#endif // NANO_TIMER_EZ_TIMER_H
