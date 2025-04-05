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
    uintptr_t _target_00;
    char _pad_004[64 - sizeof(_target_00)];
    uintptr_t _target_01;
    char _pad_005[64 - sizeof(_target_01)];
    uintptr_t _target_11;
    char _pad_006[64 - sizeof(_target_11)];
    uint64_t _tsc_per_1_sec;
    char _pad_008[64 - sizeof(_tsc_per_1_sec)];
    uint64_t _tsc_epoch;
    char _pad_009[64 - sizeof(_tsc_epoch)];

    Benchmark1()
    {
        _lcg_helper.reset(new LcgHelper());
        _calibration.reset(new std::vector<uint64_t>(2u, 0u));
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
        run_calibration();
        run_competing_threads();
    }

    void run_calibration()
    {
        std::cout << "Calibration start. Takes about 2 seconds." << std::endl;
        _calibration->assign(2u, 0u);
        std::thread t1(&Benchmark1::calibration_func, this, 0);
        std::thread t2(&Benchmark1::calibration_func, this, 1);
        t1.join();
        t2.join();
        uint64_t v1 = _calibration->at(0);
        uint64_t v2 = _calibration->at(1);
        std::cout << "Calibration done: " << v1 << ", " << v2 << std::endl;
        _tsc_per_1_sec = (v1 + v2) / 2u;
        std::cout << "Average tsc per seconds: " << _tsc_per_1_sec << std::endl;
    }

    void calibration_func(int tid)
    {
        //! @note we make sure tid is a valid index into the vector.
        uint64_t& ref_output = _calibration->at(tid);
        unsigned int c_start{};
        unsigned int c_stop{};
        uint64_t t_start{};
        uint64_t t_stop{};
        /**
         * @note For a 1-second timing to be valid, the timing loop needs
         * to start and finish while staying on the same CPU core. If not,
         * we repeat the timing loop until we get a valid result.
         */
        pause_1_sec();
        do
        {
            t_start = __rdtscp(&c_start);
            pause_1_sec();
            t_stop = __rdtscp(&c_stop);
        }
        while (c_start != c_stop);
        ref_output = t_stop - t_start;
    }

    void run_competing_threads()
    {
        _tsc_epoch = __rdtsc();
        std::cout << "busy_func_repeat_count: " << busy_func_repeat_count << std::endl;
        std::cout << "Approximate total running time: 5 seconds" << std::endl;
        std::cout << "Running competing threads..." << std::endl;
        std::cout << "_tsc_epoch = " << _tsc_epoch << std::endl;
        atomic_impl::store(_target_00, 0u);
        atomic_impl::store(_target_01, 0u);
        atomic_impl::store(_target_11, 0u);
        std::thread t1(&Benchmark1::thread_func, this, 0);
        std::thread t2(&Benchmark1::thread_func, this, 1);
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
        /**
         * @note These are tsc offsets from _tsc_epoch.
         *
         * The reason is that TSC occasionally wraps around; subtracting 
         * _tsc_epoch from future TSC reads gives a (signed) timing offset
         * which we can safely compare with TSC-based time durations.
         *
         * Moreover, because _tsc_epoch is initialized before the threads,
         * we can guarantee that these signed timing offsets will be always
         * in the positive range, because it is improbable that the test
         * would last for more than (2^63 / _tsc_per_1_sec) seconds.
         */
        static constexpr uint64_t toff_infinite = ((uint64_t)1u << 63);
        uint64_t toff_compete_00_start = toff_infinite;
        uint64_t toff_compete_00_stop = toff_infinite;
        uint64_t toff_compete_01_start = toff_infinite;
        uint64_t toff_compete_01_stop = toff_infinite;
        uint64_t toff_compete_11_start = toff_infinite;
        uint64_t toff_compete_11_stop = toff_infinite;
        uint64_t toff_join = 0u;
        /**
         * @note Time periods:
         * ... (from 0.0 to 1.0) not doing anything
         * ... (from 1.0 to 2.0) tid #0 increments its own (target_00)
         * ... (from 2.0 to 3.0) tid #0, #1 increments the same (target_01)
         * ... (from 3.0 to 4.0) tid #1 increments its own (target_11)
         * ... (from 4.0 to 5.0) not doing anything
         * ... (at 5.0) both threads join
         */
        switch (tid)
        {
            case 0:
            {
                toff_compete_00_start = _tsc_per_1_sec * 1u;
                toff_compete_00_stop = _tsc_per_1_sec * 2u;
                toff_compete_01_start = _tsc_per_1_sec * 2u;
                toff_compete_01_stop = _tsc_per_1_sec * 3u;
                toff_compete_11_start = toff_infinite;
                toff_compete_11_stop = toff_infinite;
                toff_join = _tsc_per_1_sec * 5u;
                break;
            }
            case 1:
            {
                toff_compete_00_start = toff_infinite;
                toff_compete_00_stop = toff_infinite;
                toff_compete_01_start = _tsc_per_1_sec * 2u;
                toff_compete_01_stop = _tsc_per_1_sec * 3u;
                toff_compete_11_start = _tsc_per_1_sec * 3u;
                toff_compete_11_stop = _tsc_per_1_sec * 4u;
                toff_join = _tsc_per_1_sec * 5u;
                break;
            }
            default:
            {
                return;
            }
        };
        while (true)
        {
            /**
             * @note allow modular arithmetic underflow; result shall be treated
             * as signed. In practice, it is guaranteed to be in the positive range.
             */
            uint64_t toff = (__rdtsc() - _tsc_epoch);
            if (toff >= toff_join)
            {
                break;
            }
            uintptr_t* p_target = nullptr;
            if (toff >= toff_compete_00_start && toff < toff_compete_00_stop)
            {
                p_target = &_target_00;
            }
            else if (toff >= toff_compete_01_start && toff < toff_compete_01_stop)
            {
                p_target = &_target_01;
            }
            else if (toff >= toff_compete_11_start && toff < toff_compete_11_stop)
            {
                p_target = &_target_11;
            }
            if (!p_target)
            {
                continue;
            }
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
                // Using non-atomic modify
                // ... Likely being clobbered.
                *p_target = busy_func(*p_target);
#endif
            } // for(modify_repeat_count)
        } // while(true)
        return;
    } // thread_func(tid)

}; // class Benchmark1

#endif // BENCHMARK_1_CPP
