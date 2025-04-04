#ifndef BENCHMARK_1_CPP
#define BENCHMARK_1_CPP

#include <x86intrin.h>
#include <thread>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <memory>

#include "atomic_impl/atomic_impl.h"

class LcgHelper
{
public:
    static constexpr uint32_t period = 32768u;
    static uint32_t next(uint32_t value)
    {
        return ((value & 65535u) * 51u + 1u) & 65535u;
    }
    static uint32_t get_modified(uint32_t value)
    {
        return ((value >> 2) << 1) | (value & 1);
    }
    static uint32_t get_unmodified(uint32_t value)
    {
        return ((value >> 1) << 2) | (value & 1);
    }
public:
    std::vector<uint32_t> _values;
    std::vector<uint32_t> _inverse;
    LcgHelper()
        : _values(period, (uint32_t)-1)
        , _inverse(period, (uint32_t)-1)
    {
        uint32_t value = 0;
        for (uint32_t i = 0; i < period; ++i)
        {
            uint32_t modified = get_modified(value);
            if (modified >= period)
            {
                std::cout << "Bad at i=" << i << ", modified=" << modified << std::endl;
                throw std::out_of_range("LcgHelper");
            }
            uint32_t& ref_value = _values.at(i);
            uint32_t& ref_inverse = _inverse.at(modified);
            if (ref_value != (uint32_t)-1 || ref_inverse != (uint32_t)-1)
            {
                std::cout << "Bad at i=" << i << ", modified=" << modified << std::endl;
                std::cout << "ref_value=" << ref_value << ", ref_inverse=" << ref_inverse << std::endl;
                throw std::runtime_error("LcgHelper");
            }
            ref_value = modified;
            ref_inverse = i;
            value = next(value);
        }
    }

    uint32_t find(uint32_t value)
    {
        uint32_t modified = get_modified(value);
        return _inverse.at(modified);
    }
};

class Benchmark1
{
public:
    static constexpr size_t busy_func_repeat_count = 1000u;

    std::unique_ptr<LcgHelper> _lcg_helper;
    std::unique_ptr<std::vector<uint64_t>> _calibration;
    char _pad_000[64 - sizeof(_lcg_helper) - sizeof(_calibration)];
    uintptr_t _active_0;
    char _pad_001[64 - sizeof(_active_0)];
    uintptr_t _active_1;
    char _pad_002[64 - sizeof(_active_1)];
    uintptr_t _asked_to_join;
    char _pad_003[64 - sizeof(_asked_to_join)];
    uintptr_t _target_00;
    char _pad_004[64 - sizeof(_target_00)];
    uintptr_t _target_01;
    char _pad_005[64 - sizeof(_target_01)];
    uintptr_t _target_11;
    char _pad_006[64 - sizeof(_target_11)];

    Benchmark1()
    {
        _lcg_helper.reset(new LcgHelper());
        _calibration.reset(new std::vector<uint64_t>(2u, 0u));
        _active_0 = 0u;
        _active_1 = 0u;
        _asked_to_join = 0u;
        _target_00 = 0u;
        _target_01 = 0u;
        _target_11 = 0u;
    }

    ~Benchmark1()
    {
    }

    static void pause_1_sec()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void run()
    {
        _calibration->assign(2u, 0u);
        run_calibration();
        run_competing_threads();
    }

    void run_calibration()
    {
        std::thread t1(&Benchmark1::calibration_func, this, 0);
        std::thread t2(&Benchmark1::calibration_func, this, 1);
        t1.join();
        t2.join();
        std::cout
            << "Calibration done: " << _calibration->at(0)
            << ", " << _calibration->at(1)
            << std::endl;
    }

    void run_competing_threads()
    {
        std::cout << "busy_func_repeat_count: " << busy_func_repeat_count << std::endl;
        std::cout << "Approximate total running time: 5 seconds" << std::endl;
        std::cout << "Running competing threads..." << std::endl;
        atomic_impl::store(_active_0, 0u);
        atomic_impl::store(_active_1, 0u);
        atomic_impl::store(_asked_to_join, 0u);
        atomic_impl::store(_target_00, 0u);
        atomic_impl::store(_target_01, 0u);
        atomic_impl::store(_target_11, 0u);
        std::thread t1(&Benchmark1::thread_func, this, 0);
        std::thread t2(&Benchmark1::thread_func, this, 1);
        pause_1_sec();
        atomic_impl::store(_active_0, 1u);
        pause_1_sec();
        atomic_impl::store(_active_1, 1u);
        pause_1_sec();
        atomic_impl::store(_active_0, 0u);
        pause_1_sec();
        atomic_impl::store(_active_1, 0u);
        pause_1_sec();
        atomic_impl::store(_asked_to_join, 1u);
        t1.join();
        t2.join();
        std::cout << "Threads joined." << std::endl;
        uintptr_t target_00 = atomic_impl::load(_target_00);
        uintptr_t target_01 = atomic_impl::load(_target_01);
        uintptr_t target_11 = atomic_impl::load(_target_11);
        std::cout << "Target 00 (before decoding): " << target_00 << std::endl;
        std::cout << "Target 01 (before decoding): " << target_01 << std::endl;
        std::cout << "Target 11 (before decoding): " << target_11 << std::endl;
        uintptr_t target_00_decoded = decode_final_value(target_00);
        uintptr_t target_01_decoded = decode_final_value(target_01);
        uintptr_t target_11_decoded = decode_final_value(target_11);
        std::cout << "Target 00 (decoded): " << target_00_decoded << std::endl;
        std::cout << "Target 01 (decoded): " << target_01_decoded << std::endl;
        std::cout << "Target 11 (decoded): " << target_11_decoded << std::endl;
        bool target_00_valid = (target_00_decoded % busy_func_repeat_count) == 0;
        bool target_01_valid = (target_01_decoded % busy_func_repeat_count) == 0;
        bool target_11_valid = (target_11_decoded % busy_func_repeat_count) == 0;
        std::cout << "Target 00 decoded value is valid: " << (target_00_valid ? "yes" : "no") << std::endl;
        std::cout << "Target 01 decoded value is valid: " << (target_01_valid ? "yes" : "no") << std::endl;
        std::cout << "Target 11 decoded value is valid: " << (target_11_valid ? "yes" : "no") << std::endl;
    }

    void calibration_func(int tid)
    {
        while (true)
        {
            unsigned int c_start = (unsigned int)-1;
            uint64_t t_start = __rdtscp(&c_start);
            pause_1_sec();
            unsigned int c_stop = (unsigned int)-1;
            uint64_t t_stop = __rdtscp(&c_stop);
            if (c_start == c_stop)
            {
                _calibration->at(tid) = t_stop - t_start;
                return;
            }
        }
    }

    /**
     * @brief An obscured 30-bit counter.
     * 
     * @details
     * This obscuring is necessary so that no known compiler
     * optimization technology can perform strength-reduction on the 
     * workload into some constant-time arithmetic, which defeats
     * the purpose of the benchmark.
     *
     * The extra arithmetic work is also good for benchmark purpose,
     * because it ensures that there's some delay between the
     * atomic load-acquire and the compare-exchange. The longer the
     * delay, the more likely that the competing thread will
     * invalidate the target variable.
     *
     * However, that also means the probability of invalidation
     * is dependent on the machine code generated for this function,
     * and can be indirectly influenced by compiler version and
     * optimization flags.
     *
     * The value is interpreted as two 16-bit parts (upper, lower).
     * Both parts are processed with an LCG function of period 32768,
     * where 0 is a valid starting point. After advancing the lower
     * part, if the result is 0 (completing one period), the upper
     * is advanced.
     *
     * Note that, although the period is 32768, values generated by
     * the LCG occupies 16-bit, because some values are never
     * generated (they are not members of the ring).
     *
     * Refer to the LcgHelper class for decoding.
     */
    static uintptr_t busy_func(uintptr_t value)
    {
#if 1
        // Proper implementation (obscured 30-bit counter).
        static_assert(sizeof(uintptr_t) >= sizeof(uint32_t), "");
        static_assert(sizeof(uintptr_t) >= 4, "");
        uint32_t lower = value & 65535u;
        uint32_t upper = (value >> 16) & 65535u;
        uint32_t new_lower = LcgHelper::next(lower);
        uint32_t new_upper = (new_lower) ? upper : LcgHelper::next(upper);
        return ((uintptr_t)new_upper << 16) | new_lower;
#else
        // Dummy implementation.
        //
        // If performed atomically, the final target value should be a 
        // multiple of busy_func_repeat_count. The value does not need
        // to be decoded because it does not come from the LCG.
        return value + 1u;
#endif
    }

    uint64_t decode_final_value(uint64_t value)
    {
        uint32_t lower = value & 65535u;
        uint32_t upper = (value >> 16) & 65535u;
        uint32_t decoded_lower = _lcg_helper->find(lower);
        uint32_t decoded_upper = _lcg_helper->find(upper);
        return ((uint64_t)decoded_upper << 15) | decoded_lower;
    }

    void thread_func(int tid)
    {
        while (!atomic_impl::load(_asked_to_join))
        {
            uintptr_t active_0 = atomic_impl::load(_active_0);
            uintptr_t active_1 = atomic_impl::load(_active_1);
            uintptr_t* p_target = (
                (tid == 1) ? 
                (active_1 ? &_target_01 : &_target_11) : 
                (active_0 ? &_target_01 : &_target_00)
            );
            for (size_t modify_repeat_count = 0;
                 modify_repeat_count < busy_func_repeat_count;
                 ++modify_repeat_count)
            {
#if 1
                // Code under test
                atomic_impl::modify(*p_target, busy_func);
#elif 0
                // Using built-in increment
                // ... Likely compiled to a lock-add instruction on x86, 
                // ... which is very efficient and requires no loop.
                __atomic_fetch_add(p_target, 1u, __ATOMIC_SEQ_CST);
#else
                // Using non-atomic increment
                // ... Likely to produce invalid result due to clobbering
                *p_target += 1u;
#endif
            }
        }
    }
};

#endif // BENCHMARK_1_CPP
