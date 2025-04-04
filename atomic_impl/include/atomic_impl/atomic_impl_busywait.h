#ifndef ATOMIC_IMPL_BUSYWAIT_H
#define ATOMIC_IMPL_BUSYWAIT_H

#include <cstdint>

namespace atomic_impl {
namespace detail {

struct busywaiter
{
    //! @todo Change to some reasonable value based on benchmark result.
    static constexpr unsigned escalate_pause_count = 1024u;
    
    //! @todo Change to some reasonable value based on benchmark result.
    static constexpr unsigned escalate_yield_count = 1024u;

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
