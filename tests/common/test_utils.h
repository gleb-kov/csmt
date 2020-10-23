#ifndef CSMT_TEST_UTILS_H
#define CSMT_TEST_UTILS_H

#include <string>
#include <sstream>
#include <random>

namespace test {

    struct IdentityHashPolicy {
        static std::string leaf_hash(std::string leaf_value) {
            return leaf_value;
        }

        static std::string merge_hash(const std::string &lhs, const std::string &rhs) {
            return lhs + rhs;
        }
    };

    template<typename HP, typename HT, typename VT>
    bool check_proof(const Csmt<HP, HT, VT> &tree, uint64_t key, const std::deque<HT> &proof = {}) {
        bool empty = proof.empty();
        bool contains = tree.contains(key);
        if (empty == contains) {
            return false;
        }
        return tree.membership_proof(key) == proof;
    }

    inline std::string default_value_gen(uint64_t key_index) {
        std::ostringstream s;
        s << "VALUE" << key_index;
        return s.str();
    }

    inline std::mt19937 &rnd() {
        static std::random_device random_device;
        static std::mt19937 generator(random_device());
        return generator;
    }

    inline std::string base64_decode(std::string const &in) {
        static std::string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        static std::vector<size_t> reverse(256, -1);
        static bool first_time = true;
        if (first_time) {
            first_time = false;
            for (size_t i = 0; i < alpha.size(); i++) {
                reverse[alpha[i]] = i;
            }
            reverse['='] = 0;
        }

        std::string out;
        size_t out_size = in.size() / 4 * 3;
        if (in[in.size() - 1] == '=') {
            out_size--;
        }
        if (in[in.size() - 2] == '=') {
            out_size--;
        }

        size_t index = 0;
        for (size_t i = 0; i < in.size(); i += 4) {
            uint32_t value = 0;
            for (size_t b = 0; b < 4; b++) {
                value = (value << 6u) | reverse[in[i + b]];
            }

            for (size_t b = 0; b < 3; b++) {
                if (index < out_size) {
                    index++;
                    out.push_back((value >> (2u - b) * 8u) & 0xFFu);
                }
            }
        }
        return out;
    }

}

#endif // CSMT_TEST_UTILS_H
