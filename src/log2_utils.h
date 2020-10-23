#ifndef CSMT_LOG2_UTILS_H
#define CSMT_LOG2_UTILS_H

namespace log2_impl {
#ifdef __GNUC__
#define LOG2(X) ((unsigned) (8 * sizeof (unsigned long long) - __builtin_clzll((X)) - 1))
#endif
    inline uint64_t naive_log2(uint64_t num) {
        uint64_t res = 0;
        while (num > 1) {
            num >>= 1u;
            ++res;
        }
        return res;
    }

    inline uint64_t table_log2(uint64_t num) {
        static constexpr uint64_t table[64] = {
                0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61,
                51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4, 62,
                57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
                45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5, 63};
        num |= num >> 1u;
        num |= num >> 2u;
        num |= num >> 4u;
        num |= num >> 8u;
        num |= num >> 16u;
        num |= num >> 32u;
        return table[(num * 0x03f6eaf2cd271461) >> 58u];
    }
}

#endif // CSMT_LOG2_UTILS_H
