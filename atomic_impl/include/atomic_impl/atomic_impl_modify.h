#ifndef ATOMIC_IMPL_MODIFY_H
#define ATOMIC_IMPL_MODIFY_H

#include <cstdint>
#include <type_traits>
#include <utility>
#include "atomic_impl/atomic_impl_defs.h"
#include "atomic_impl/atomic_impl_methods.h"
#include "atomic_impl/atomic_impl_busywait.h"

namespace atomic_impl {

/**
 * @brief Performs a read-modify-write operation on a destination value, where
 *        the new value is computed with the provided callable object.
 *
 * @param dest The destination value to be modified.
 *
 * @param op A callable object that accepts a value loaded from the destination,
 *           and returns a new value to be written back to the destination.
 *           This object can be stateful; state changes are visible to the caller.
 *
 * @returns The (old, new) pair of values from a successful operation.
 *
 * @remark This function uses a compare-and-swap loop to ensure that the
 *         read-modify-write steps happened without interference from
 *         other threads operating on the same destination.
 *
 * @remark For one of the six basic operations, consider using the corresponding function:
 *         add, sub, and, or, xor, nand, or their fetch_modify variants, for better
 *         performance.
 *
 * @see fetch_modify, fetch_add, fetch_sub, fetch_and, fetch_or, fetch_xor, fetch_nand
 */
template <class Op>
static
FLATTEN
std::pair<uintptr_t, uintptr_t>
INLINE_NEVER
modify(uintptr_t& dest, Op& op)
{
    using op_result_type = decltype(op(std::declval<uintptr_t>()));
    static_assert(std::is_assignable<uintptr_t&, op_result_type>::value,
                  "Op invocation must be result compatible");
    detail::busywaiter waiter;
    while (true)
    {
        //! @note reload from dest is required after a wait().
        uintptr_t loaded = load(dest);
        uintptr_t computed = op(loaded);
        uintptr_t expected_or_actual = loaded;
        if (try_replace(dest, expected_or_actual, computed))
        {
            return std::make_pair(loaded, computed);
        }
        waiter.wait();
    }
    //! @note this line shall be unreachable.
};

} // namespace atomic_impl

#endif // ATOMIC_IMPL_MODIFY_H
