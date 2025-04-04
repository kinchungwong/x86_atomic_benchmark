#ifndef ATOMIC_IMPL_TRAITS_H
#define ATOMIC_IMPL_TRAITS_H

#include <cstdint>
#include <type_traits>

namespace atomic_impl {
namespace traits {

namespace detail {

/**
 * @internal Do not use from outside atomic_impl.
 * @note Type assertions are subject to change.
 * @todo std::is_invocable requires C++17, need to implement a replacement.
 */
template <
    class Op,
    bool IsDefaultConstructible = std::is_default_constructible<Op>::value,
    bool IsTriviallyCopyable = std::is_trivially_copyable<Op>::value,
    bool IsInvocable = true,
    bool IsResultCompatible = (std::is_assignable<uintptr_t&,
            decltype(std::declval<const Op&>()(
                std::declval<const uintptr_t&>(), 
                std::declval<const uintptr_t&>())
            )>::value
        )
    >
struct _detail_is_valid_binary_op
    : private std::false_type
{
    static_assert(IsDefaultConstructible, "Op must be default constructible");
    static_assert(IsTriviallyCopyable, "Op must be trivially copyable");
    static_assert(IsInvocable, "Op must be invocable");
    static_assert(IsResultCompatible, "Op invocation must be result compatible");
};

template <class Op>
struct _detail_is_valid_binary_op<Op, true, true, true, true>
    : public std::true_type
{
};

} // namespace detail (in atomic_impl::traits)

template <class Op>
struct is_binary_op 
    : public detail::_detail_is_valid_binary_op<Op>
{
};

}} // namespace atomic_impl::traits

#endif // ATOMIC_IMPL_TRAITS_H
