#include <iostream>

#define SINGLE_TRANSLATION_UNIT_BUILD_MODE
#include "atomic_impl/atomic_impl.h"

void simple_demo()
{
    uintptr_t dest = 3;
    uintptr_t value = 555;
    struct SquareModuloMillion
    {
        uintptr_t operator()(uintptr_t dest, uintptr_t value) const
        {
            // Use 64-bit so that the multiplication step
            // doesn't overflow.
            uint64_t current = dest;
            size_t count = value;
            for (size_t i = 0; i < value; ++i)
            {
                current = (current * current) % (uint64_t)1000000;
            }
            return static_cast<uintptr_t>(current);
        }
    };
    auto fetched = atomic_impl::modify(dest, value, SquareModuloMillion());
    std::cout << "Result: " << fetched << std::endl;
}

int main()
{
    simple_demo();
    return 0;
}
