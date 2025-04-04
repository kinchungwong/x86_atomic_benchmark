#ifndef ATOMIC_IMPL_METHODS_H
#define ATOMIC_IMPL_METHODS_H

#include <cstdint>

namespace atomic_impl {

/**
 * @brief Indicates to CPU that the current thread is in a busy-wait loop,
 *        allowing the CPU to optimize its performance by either reducing
 *        power consumption, perform a hyperthreading yield, or
 *        temporarily boosting the speed of other cores.
 *
 * @remarks This function shall never invoke any system call.
 */
void busywait_pause();

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
void busywait_yield();

/**
 * @todo Replace with signal, do not throw an exception.
 */
[[noreturn]] void busywait_noreturn();

/**
 * @brief Load a value atomically from the given destination.
 */
uintptr_t load(uintptr_t& dest);

/**
 * @brief Store a value atomically to the given destination.
 */
void store(uintptr_t& dest, uintptr_t value);

/**
 * @brief Perform compare-and-swap at the given destination.
 *
 * @details The destination value is loaded and compared with the
 *          expected value. If they are equal, the destination is
 *          updated with the desired value, and then the function
 *          returns true. Otherwise, the destination is not
 *          modified.
 *
 * @note This function does not return the actual value of the
 *       destination on failure. Users who need that value should
 *       consider using: GCC Atomic Builtins
 *
 * @see GCC Atomic Builtins
 * 
 * @return true if the operation was successful, false otherwise.
 */
bool try_replace(uintptr_t& dest, uintptr_t expected, uintptr_t desired);

} // namespace atomic_impl

#endif // ATOMIC_IMPL_METHODS_H
