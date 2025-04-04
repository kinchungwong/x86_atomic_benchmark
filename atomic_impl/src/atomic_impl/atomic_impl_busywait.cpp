#ifndef ATOMIC_IMPL_BUSYWAIT_CPP
#define ATOMIC_IMPL_BUSYWAIT_CPP

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

void busywaiter::wait()
{
    if (pause_count >= escalate_pause_count)
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
    else 
    {
        ++pause_count;
        busywait_pause();
    }            
}

}} // namespace atomic_impl::detail

#endif // ATOMIC_IMPL_BUSYWAIT_CPP
