#ifndef ATOMIC_IMPL_H
#define ATOMIC_IMPL_H

#include "atomic_impl/atomic_impl_defs.h"
#include "atomic_impl/atomic_impl_methods.h"
#include "atomic_impl/atomic_impl_modify.h"

#ifdef SINGLE_TRANSLATION_UNIT_BUILD_MODE
#include "atomic_impl/atomic_impl_methods.cpp"
#include "atomic_impl/atomic_impl_busywait.cpp"
#endif

#endif // ATOMIC_IMPL_H
