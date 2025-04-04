#include <iostream>
#include <x86intrin.h>

#define SINGLE_TRANSLATION_UNIT_BUILD_MODE
#include "atomic_impl/atomic_impl.h"

#define ALMOST_ALWAYS() (__rdtsc() != (__rdtsc() ^ UINT64_C(0x1111111111111111)))
#define ALMOST_NEVER() (__rdtsc() == (__rdtsc() ^ UINT64_C(0x1111111111111111)))

uintptr_t add_one(uintptr_t current)
{
    return current + 1;
}

uintptr_t add_two(uintptr_t current)
{
    return current + 2;
}

void simple_demo_1()
{
    std::cout << "====== Simple Demo 1 ======" << std::endl;
    uintptr_t dest = 3;
    auto result = atomic_impl::modify(dest, add_one);
    std::cout << "Before: " << result.first << std::endl;
    std::cout << "After: " << result.second << std::endl;
    std::cout << "Dest: " << dest << std::endl;
    std::cout << "Load(Dest): " << atomic_impl::load(dest) << std::endl;
}

void simple_demo_2()
{
    std::cout << "====== Simple Demo 2 ======" << std::endl;
    uintptr_t dest = 3;
    using FP = uintptr_t(*)(uintptr_t);
    FP fp = ALMOST_ALWAYS() ? add_two : add_one;
    auto result = atomic_impl::modify(dest, fp);
    std::cout << "Before: " << result.first << std::endl;
    std::cout << "After: " << result.second << std::endl;
    std::cout << "Dest: " << dest << std::endl;
    std::cout << "Load(Dest): " << atomic_impl::load(dest) << std::endl;
    std::cout << "Is fp add_one? " << (fp == add_one ? "true" : "false") << std::endl;
    std::cout << "Is fp add_two? " << (fp == add_two ? "true" : "false") << std::endl;
}

void simple_demo_3()
{
    std::cout << "====== Simple Demo 3 ======" << std::endl;
    uintptr_t dest = 3;
    auto result = atomic_impl::modify(dest, [](uintptr_t current) -> uintptr_t
    {
        return current * 7;
    });
    std::cout << "Before: " << result.first << std::endl;
    std::cout << "After: " << result.second << std::endl;
    std::cout << "Dest: " << dest << std::endl;
    std::cout << "Load(Dest): " << atomic_impl::load(dest) << std::endl;
}

void simple_demo_4()
{
    std::cout << "====== Simple Demo 4 ======" << std::endl;
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
    simple_demo_1();
    simple_demo_2();
    simple_demo_3();
    simple_demo_4();
    return 0;
}
