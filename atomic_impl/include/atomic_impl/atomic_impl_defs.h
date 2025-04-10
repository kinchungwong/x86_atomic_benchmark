#ifndef ATOMIC_IMPL_DEFS_H
#define ATOMIC_IMPL_DEFS_H

#ifdef INLINE_NEVER
#  undef INLINE_NEVER
#endif

#ifdef INLINE_ALWAYS
#  undef INLINE_ALWAYS
#endif

#ifdef FLATTEN
#  undef FLATTEN
#endif

#define INLINE_NEVER __attribute__((noinline))
#define INLINE_ALWAYS __attribute__((always_inline))
#define FLATTEN __attribute__((flatten))

#endif // ATOMIC_IMPL_DEFS_H
