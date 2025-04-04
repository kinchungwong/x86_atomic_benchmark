#ifndef ATOMIC_IMPL_MODIFY_H
#define ATOMIC_IMPL_MODIFY_H

#include <cstdint>
#include "atomic_impl/atomic_impl_defs.h"
#include "atomic_impl/atomic_impl_methods.h"
#include "atomic_impl/atomic_impl_traits.h"
#include "atomic_impl/atomic_impl_busywait.h"

namespace atomic_impl {

/**
 * @brief Modifies the value of the destination by applying the binary operation
 *        to (dest, operand) and storing the result in dest.
 *
 * @details This function uses a compare-and-swap loop to ensure that the
 *          read-modify-write steps happened without interference from
 *          other threads operating on the same destination.
 *
 * @remarks This function returns the value of the destination after the operation.
 *          If, instead, the user wants to obtain the destination value immediately
 *          before the successful operation, the user should use fetch_modify().
 *
 * @see add, sub, and, or, xor, nand
 * 
 * @see fetch_modify, fetch_add, fetch_sub, fetch_and, fetch_or, fetch_xor, fetch_nand
 *
 * @tparam Op The type of the binary operation to be applied. This must be a callable
 *            object that takes two arguments (load(dest), operand) and returns a value
 *           that can be assigned back to dest. Its operator() must be const-qualified.
 *
 * @note Generally speaking, Op should be stateless (and satisfied default constructible
 *       and trivial copyable), but currently this is not enforced.
 *
 * @note For one of the six basic operations, consider using the corresponding function:
 *       add, sub, and, or, xor, nand, or their fetch_modify variants, for better
 *       performance.
 *
 * @param dest The destination value to be modified.
 *
 * @param operand The operand to be used in the operation.
 *
 * @param op The binary operation to be applied. 
 *
 * @return The value of the destination after the operation.
 */
template <class Op>
static
FLATTEN
uintptr_t
INLINE_NEVER
modify(uintptr_t& dest, uintptr_t operand, const Op& op)
{
    static_assert(traits::is_binary_op<Op>::value, "Op must be a binary operation");
    detail::busywaiter waiter;
    while (true)
    {
        uintptr_t expected = load(dest);    
        uintptr_t desired = op(expected, operand);
        if (try_replace(dest, expected, desired))
        {
            return desired;
        }
        waiter.wait();
        //! @note after a busywait pause or yield, dest needs reloaded.
    };
    //! @note this line shall be unreachable.
};

} // namespace atomic_impl

#endif // ATOMIC_IMPL_MODIFY_H
