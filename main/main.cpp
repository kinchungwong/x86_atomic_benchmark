#include <iostream>

#define SINGLE_TRANSLATION_UNIT_BUILD_MODE
#include "atomic_impl/atomic_impl.h"

void simple_demo()
{
    uintptr_t dest = 3;
    uintptr_t value = 555;
    auto square_modulo_million = [=](uintptr_t current)
    {
        // Use 64-bit so that the multiplication step
        // doesn't overflow.
        uint64_t result = current;
        for (size_t i = 0; i < value; ++i)
        {
            result = (result * result) % (uint64_t)1000000;
        }
        return static_cast<uintptr_t>(result);
    };
    auto result = atomic_impl::modify(dest, square_modulo_million);
    std::cout << "Before: " << result.first << std::endl;
    std::cout << "After: " << result.second << std::endl;
}

int main()
{
    simple_demo();
    return 0;
}
