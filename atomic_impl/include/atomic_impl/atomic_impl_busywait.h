#ifndef ATOMIC_IMPL_BUSYWAIT_H
#define ATOMIC_IMPL_BUSYWAIT_H

#include <cstdint>

namespace atomic_impl {
namespace detail {

struct busywaiter
{
    /**
     * @brief Escalation limits, adjusted from benchmark results.
     */
    static constexpr uint64_t busywait_pause_tsc_goal = 160u;
    static constexpr unsigned escalate_pause_count = 32u;
    static constexpr unsigned escalate_yield_count = 32u;

    unsigned pause_count;
    unsigned yield_count;

    busywaiter();
    ~busywaiter();

    /**
     * @brief Performs wait during a busy-wait loop, such as a
     *        read-modify-write loop inside a non-trivial compare-exchange
     *        operation.
     *
     * @details This implementation first performs a pause (machine instruction),
     *          and then escalates to a yield (system call), and finally throws
     *          an exception if it is stuck for too long.
     *
     * @exception std::runtime_error
     */
    void wait();

    void wait_longer();
};

}} // namespace atomic_impl::detail

#endif // ATOMIC_IMPL_BUSYWAIT_H
