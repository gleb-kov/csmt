#include "hash_policy.h"
#include "src/csmt.h"
#include "utils.h"

#include <bitset>
#include <iostream>

/***  choose hash policy for benchmarks ***/
//#define DEFAULT_POLICY
//#define SHA256_POLICY
#define SHA256_TREE_POLICY

constexpr size_t DEF_KEYS = 50'000;

#ifdef DEFAULT_POLICY
    using tree_type = Csmt<>;
    const char *POLICY_STR = "Default";
#elif defined(SHA256_POLICY)
    using tree_type = Csmt<HashPolicySHA256>;
    const char *POLICY_STR = "SHA256";
#elif defined(SHA256_TREE_POLICY)
    using tree_type = Csmt<HashPolicySHA256Tree>;
    const char *POLICY_STR = "SHA256_TREE";
#elif
    using tree_type = Csmt<>;
#endif

template <size_t VALUE_SIZE, size_t KEYS = DEF_KEYS>
void spam_insert() {
    std::cout << "BENCH SPAM INSERT. Operations: " << KEYS
              << ". Value size: " << VALUE_SIZE << std::endl;

    uint64_t elapsed_us = 0;
    uint64_t min_elapsed_us = std::numeric_limits<uint64_t>::max();
    uint64_t max_elapsed_us = 0;

    time_utils::stage_timer<> st;
    tree_type tree;

    for (size_t idx = 0; idx < KEYS; ++idx) {
        std::string val_str = string_utils::generate_random_string(VALUE_SIZE);
        st.start_stage();
        tree.insert(idx, val_str);
        uint64_t iter_elapsed_us = st.stop_stage<std::chrono::microseconds>().count();
        elapsed_us += iter_elapsed_us;
        min_elapsed_us = std::min(min_elapsed_us, iter_elapsed_us);
        max_elapsed_us = std::max(max_elapsed_us, iter_elapsed_us);
    }
    std::cout << "Average elapsed: " << elapsed_us * 1.0 / KEYS << " us." << std::endl;
    std::cout << "Min elapsed: " << min_elapsed_us << " us." << std::endl;
    std::cout << "Max elapsed: " << max_elapsed_us << " us." << std::endl;
}

template <size_t VALUE_SIZE, size_t KEYS = DEF_KEYS>
void spam_erase() {
    std::cout << "BENCH SPAM ERASE. Operations: " << KEYS
              << ". Value size: " << VALUE_SIZE << std::endl;

    // add blocks before measure erase()
    tree_type tree;

    for (size_t idx = 0; idx < KEYS; ++idx) {
        std::string val_str = string_utils::generate_random_string(VALUE_SIZE);
        tree.insert(idx, val_str);
    }

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<uint64_t> key_gen(0, KEYS - 1);
    time_utils::stage_timer<> st;

    std::bitset<KEYS> already_erased;

    uint64_t elapsed_us = 0;
    uint64_t min_elapsed_us = std::numeric_limits<uint64_t>::max();
    uint64_t max_elapsed_us = 0;
    uint64_t missed_calls = 0;

    for (size_t idx = 0; idx < KEYS; ++idx) {
        uint64_t key_index = key_gen(generator);
        st.start_stage();
        tree.erase(key_index);
        uint64_t iter_elapsed_us = st.stop_stage<std::chrono::microseconds>().count();

        if (already_erased[key_index]) {
            ++missed_calls;
        }
        already_erased[key_index] = true;
        elapsed_us += iter_elapsed_us;
        min_elapsed_us = std::min(min_elapsed_us, iter_elapsed_us);
        max_elapsed_us = std::max(max_elapsed_us, iter_elapsed_us);
    }

    std::cout << "Average elapsed: " << elapsed_us * 1.0 / KEYS << " us." << std::endl;
    std::cout << "Min elapsed: " << min_elapsed_us << " us." << std::endl;
    std::cout << "Max elapsed: " << max_elapsed_us << " us." << std::endl;
    std::cout << "Missed erase() calls: " << missed_calls << std::endl;
}

template <size_t VALUE_SIZE, size_t KEYS = DEF_KEYS>
void spam_contains() {
    std::cout << "BENCH SPAM CONTAINS. Operations: " << KEYS
              << ". Value size: " << VALUE_SIZE << std::endl;

    // add blocks before measure erase()
    tree_type tree;

    for (size_t idx = 0; idx < KEYS; ++idx) {
        std::string val_str = string_utils::generate_random_string(VALUE_SIZE);
        tree.insert(idx, val_str);
    }

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<uint64_t> key_gen(0, 2 * KEYS - 1);
    time_utils::stage_timer<> st;

    uint64_t elapsed_us = 0;
    uint64_t min_elapsed_us = std::numeric_limits<uint64_t>::max();
    uint64_t max_elapsed_us = 0;
    uint64_t missed_calls = 0;

    for (size_t idx = 0; idx < KEYS; ++idx) {
        uint64_t key_index = key_gen(generator);
        st.start_stage();
        bench_utils::do_not_optimize(tree.contains(key_index));
        uint64_t iter_elapsed_us = st.stop_stage<std::chrono::microseconds>().count();

        if (key_index < KEYS) {
            ++missed_calls;
        }
        elapsed_us += iter_elapsed_us;
        min_elapsed_us = std::min(min_elapsed_us, iter_elapsed_us);
        max_elapsed_us = std::max(max_elapsed_us, iter_elapsed_us);
    }

    std::cout << "Average elapsed: " << elapsed_us * 1.0 / KEYS << " us." << std::endl;
    std::cout << "Min elapsed: " << min_elapsed_us << " us." << std::endl;
    std::cout << "Max elapsed: " << max_elapsed_us << " us." << std::endl;
    std::cout << "Missed contains() calls: " << missed_calls << std::endl;
}

template <size_t VALUE_SIZE, size_t KEYS = DEF_KEYS>
void spam_all() {
    std::cout << "BENCH SPAM ALL. Operations: " << KEYS
              << ". Value size: " << VALUE_SIZE << std::endl;

    // add blocks before measure erase()
    tree_type tree;

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> op_gen(0, 2);
    std::uniform_int_distribution<uint64_t> key_gen(0, 4 * KEYS - 1);
    time_utils::stage_timer<> st;

    uint64_t elapsed_us = 0;

    for (size_t idx = 0; idx < KEYS; ++idx) {
        int op = op_gen(generator);
        uint64_t key_index = key_gen(generator);

        uint64_t iter_elapsed_us = 0;

        if (op == 0) {
            std::string val_str = string_utils::generate_random_string(VALUE_SIZE);
            st.start_stage();
            tree.insert(key_index, val_str);
            iter_elapsed_us = st.stop_stage<std::chrono::microseconds>().count();
        } else if (op == 1) {
            st.start_stage();
            tree.erase(key_index);
            iter_elapsed_us = st.stop_stage<std::chrono::microseconds>().count();
        } else if (op == 2) {
            st.start_stage();
            bench_utils::do_not_optimize(tree.contains(key_index));
            iter_elapsed_us = st.stop_stage<std::chrono::microseconds>().count();
        }

        elapsed_us += iter_elapsed_us;
    }

    std::cout << "Average elapsed: " << elapsed_us * 1.0 / KEYS << " us." << std::endl;
}

void run_spam_insert() {
    spam_insert<32>();
    spam_insert<256>();
    spam_insert<2048>();
}

void run_spam_erase() {
    spam_erase<32>();
    spam_erase<256>();
    spam_erase<2048>();
}

void run_spam_contains() {
    spam_contains<32, 100'000>();
    spam_contains<256, 100'000>();
    spam_contains<2048, 100'000>();
}

void run_spam_all() {
    spam_all<32>();
    spam_all<256>();
    spam_all<2048>();
}

int main() {
    std::cout << "RUN CSMT BENCHMARKS. HASH POLICY: " << POLICY_STR << std::endl << std::endl;

    run_spam_insert();
    std::cout << "-------------------------------------------" << std::endl;
    run_spam_erase();
    std::cout << "-------------------------------------------" << std::endl;
    run_spam_contains();
    std::cout << "-------------------------------------------" << std::endl;
    run_spam_all();
}
