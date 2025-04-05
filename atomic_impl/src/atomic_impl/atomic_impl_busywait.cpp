#ifndef ATOMIC_IMPL_BUSYWAIT_CPP
#define ATOMIC_IMPL_BUSYWAIT_CPP

#include "atomic_impl/atomic_impl_defs.h"
#include "atomic_impl/atomic_impl_busywait.h"
#include "atomic_impl/atomic_impl_methods.h"

namespace atomic_impl {
namespace detail {

busywaiter::busywaiter()
    : pause_count(0u)
    , yield_count(0u)
{
}

busywaiter::~busywaiter()
{
}

FLATTEN
void
busywaiter::wait()
{
    if (pause_count >= escalate_pause_count)
    {
        wait_longer();
    }
    else 
    {
        ++pause_count;
        busywait_pause(busywait_pause_tsc_goal);
    }            
}

void
INLINE_NEVER
busywaiter::wait_longer()
{
    pause_count = 0;
    if (yield_count >= escalate_yield_count)
    {
        busywait_noreturn();
        //! @note this line shall be unreachable.
    }
    ++yield_count;
    busywait_yield();
}

}} // namespace atomic_impl::detail

#endif // ATOMIC_IMPL_BUSYWAIT_CPP
