#ifndef NANO_TIMER_IMPL_H
#define NANO_TIMER_IMPL_H

#include <cstdint>

namespace timing {

class NanoTimer
{
private:
    uint64_t _summed;
    uint64_t _started;
    bool _is_running;
    bool _is_invalid;

public:
    explicit NanoTimer(bool start_now = false);
    void start();
    void stop();
    uint64_t current_nanos() const;
    double current_seconds() const;
    bool is_running() const { return _is_running; }
    bool is_valid() const { return !(_is_invalid); }
    ~NanoTimer();
    NanoTimer(const NanoTimer&);
    NanoTimer(NanoTimer&&);
    NanoTimer& operator=(const NanoTimer&);
    NanoTimer& operator=(NanoTimer&&);
};

} // namespace timing

#endif // NANO_TIMER_IMPL_H
