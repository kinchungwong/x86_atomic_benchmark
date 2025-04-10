// include guard for source file is only needed for single-translation-unit build mode.
#ifndef ATOMIC_IMPL_METHODS_CPP
#define ATOMIC_IMPL_METHODS_CPP

#include <cstdint> // already in atomic_impl_methods.h
#include <cstddef>
#include <stdatomic.h>
#include <x86intrin.h>
#include <stdexcept>
#include <utility>
#include <thread>

#include "atomic_impl/atomic_impl_defs.h"
#include "atomic_impl/atomic_impl_methods.h"

namespace atomic_impl
{

/**
 * @brief Indicates to CPU that the current thread is in a busy-wait loop,
 *        allowing the CPU to optimize its performance by either reducing
 *        power consumption, perform a hyperthreading yield, or
 *        temporarily boosting the speed of other cores.
 *
 * @remarks This function shall never invoke any system call.
 */
FLATTEN
void busywait_pause(uint64_t tsc_goal)
{
    /**
     * @ref https://www.intel.com/content/www/us/en/developer/articles/technical/a-common-construct-to-avoid-the-contention-of-threads-architecture-agnostic-spin-wait-loops.html
     */
    uint64_t start = __rdtsc();
    do
    {
        _mm_pause();
    }
    while ((__rdtsc() - start) < tsc_goal);
}

/**
 * @brief Indicates to the operating system that the current thread is
 *        in a busy-wait loop that appears to be stuck, allowing the
 *        operating system to intervene in several ways: boosting
 *        the priority of other threads, or raising a signal to the
 *        handler to indicate a failed spinlock.
 *
 * @remark This function may invoke a system call.
 *
 * @remark A failed spinlock must not be allowed to resume execution
 *         because resumption with a postcondition violation leads
 *         to undefined behavior.
 */
void
INLINE_NEVER
busywait_yield()
{
    std::this_thread::yield();
}

/**
 * @todo Replace with signal, do not throw an exception.
 */
[[noreturn]]
void
INLINE_NEVER
busywait_noreturn()
{
    throw std::runtime_error("Busywait raise");
}

FLATTEN
uintptr_t load(uintptr_t& dest)
{
    return __atomic_load_n(&dest, __ATOMIC_SEQ_CST);
}

FLATTEN
void store(uintptr_t& dest, uintptr_t value)
{
    __atomic_store_n(&dest, value, __ATOMIC_SEQ_CST);
}

FLATTEN
bool try_replace(uintptr_t& dest, uintptr_t expected, uintptr_t desired)
{
    return __atomic_compare_exchange_n(&dest, &expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

} // namespace atomic_impl

#endif // ATOMIC_IMPL_METHODS_CPP
