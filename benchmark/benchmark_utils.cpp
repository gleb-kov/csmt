#include "contrib/crypto/sha256.h"
#include "utils.h"

#include <iostream>
#include <limits>
#include <random>

namespace log2_impl {
#ifdef __GNUC__
#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))
#endif

    uint64_t naive_log2(uint64_t num) {
        uint64_t res = 0;
        while (num > 1) {
            num >>= 1u;
            ++res;
        }
        return res;
    }

    uint64_t table_log2(uint64_t num) {
        static constexpr uint64_t table[64] = {
                0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61,
                51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4, 62,
                57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
                45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5, 63 };
        num |= num >> 1u;
        num |= num >> 2u;
        num |= num >> 4u;
        num |= num >> 8u;
        num |= num >> 16u;
        num |= num >> 32u;
        return table[(num * 0x03f6eaf2cd271461) >> 58u];
    }
}

void bench_log2() {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<uint64_t> distrib(1, std::numeric_limits<uint64_t>::max());
    constexpr uint64_t ITERATIONS = 1e6;

    {
        std::cout << "BENCH NAIVE LOG2" << std::endl;
        uint64_t total = 0; // do not wipe log2
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            uint64_t num = distrib(generator);
            uint64_t res = log2_impl::naive_log2(num);
            total += res;
        }
        auto elapsed_ms = st.stop_stage<std::chrono::nanoseconds>().count();
        std::cout << "Average: " << elapsed_ms * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH LOG2 INTRINSIC" << std::endl;

#ifndef LOG2
        std::cout << "WORKS ONLY ON GCC" << std::endl;
#else
        uint64_t total = 0; // do not wipe log2
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            uint64_t num = distrib(generator);
            uint64_t res = LOG2(num);
            total += res;
        }
        auto elapsed_ms = st.stop_stage<std::chrono::nanoseconds>().count();
        std::cout << "Average: " << elapsed_ms * 1.0 / ITERATIONS << " ns." << std::endl;
#endif
    }

    {
        std::cout << "BENCH TABLE LOG2" << std::endl;
        uint64_t total = 0; // do not wipe log2
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            uint64_t num = distrib(generator);
            uint64_t res = log2_impl::table_log2(num);
            total += res;
        }
        auto elapsed_ms = st.stop_stage<std::chrono::nanoseconds>().count();
        std::cout << "Average: " << elapsed_ms * 1.0 / ITERATIONS << " ns." << std::endl;
    }
}

void bench_sha256() {
    constexpr uint64_t ITERATIONS = 1e3;

    {
        std::cout << "BENCH SHA256 4 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(4);
            bench_utils::do_not_optimize(str);
            st.start_stage();
            str = SHA256::hash(str);
            bench_utils::clobber();
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH SHA256 32 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(32);
            bench_utils::do_not_optimize(str);
            st.start_stage();
            str = SHA256::hash(str);
            bench_utils::clobber();
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH SHA256 256 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(128);
            bench_utils::do_not_optimize(str);
            st.start_stage();
            str = SHA256::hash(str);
            bench_utils::clobber();
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH SHA256 2048 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(2048);
            bench_utils::do_not_optimize(str);
            st.start_stage();
            str = SHA256::hash(str);
            bench_utils::clobber();
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH SHA256 16384 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(16384);
            bench_utils::do_not_optimize(str);
            st.start_stage();
            str = SHA256::hash(str);
            bench_utils::clobber();
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }
}

void bench_std_hash() {
    constexpr uint64_t ITERATIONS = 1e3;

    {
        std::cout << "BENCH STD HASH 4 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(4);
            st.start_stage();
            size_t h = 0;
            bench_utils::do_not_optimize(h = std::hash<std::string>{}(str));
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH STD HASH 32 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(32);
            st.start_stage();
            size_t h = 0;
            bench_utils::do_not_optimize(h = std::hash<std::string>{}(str));
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH STD HASH 256 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(128);
            st.start_stage();
            size_t h = 0;
            bench_utils::do_not_optimize(h = std::hash<std::string>{}(str));
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH STD HASH 2048 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(2048);
            st.start_stage();
            size_t h = 0;
            bench_utils::do_not_optimize(h = std::hash<std::string>{}(str));
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }

    {
        std::cout << "BENCH STD HASH 16384 chars" << std::endl;
        uint64_t elapsed_ns = 0;
        time_utils::stage_timer<> st;
        for (uint64_t i = 0; i < ITERATIONS; ++i) {
            std::string str = string_utils::generate_random_string(16384);
            st.start_stage();
            size_t h = 0;
            bench_utils::do_not_optimize(h = std::hash<std::string>{}(str));
            elapsed_ns += st.stop_stage<std::chrono::nanoseconds>().count();
        }
        std::cout << "Average: " << elapsed_ns * 1.0 / ITERATIONS << " ns." << std::endl;
    }
}

int main() {
    std::cout << "BENCH LOG2" << std::endl << std::endl;
    bench_log2();

    std::cout << "-----------------------------" << std::endl;

    std::cout << "BENCH SHA256" << std::endl << std::endl;
    bench_sha256();

    std::cout << "-----------------------------" << std::endl;

    std::cout << "BENCH STD HASH" << std::endl << std::endl;
    bench_std_hash();
}
