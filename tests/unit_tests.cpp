#include "contrib/gtest/gtest.h"
#include "contrib/crypto/sha256.h"
#include "src/csmt.h"
#include "src/log2_utils.h"
#include "tests/common/test_utils.h"

#include <functional>
#include <string>
#include <vector>

using namespace test;

TEST(log2, correct) {
    uint64_t key = 1;
    uint64_t next_key = 2;
    uint64_t power = 0;

    for (; key < (1ull << 15u); key++) {
        if (key == next_key) {
            next_key <<= 1u;
            power++;
        }
#ifdef __GNUC__
        ASSERT_EQ(LOG2(key), power);
#endif
        ASSERT_EQ(log2_impl::table_log2(key), power);
    }
}

TEST(sha256, correct) {
    std::vector<std::pair<std::string, std::string>> codes{
            {"",       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
            {"banana", "b493d48364afe44d11c0165cf470a4164d1e2609911ef998be868d46ade3de4e"},
            {"cat",    "77af778b51abd4a3c51c5ddd97204a9c3ae614ebccb75a606c3b6865aed6744e"},
            {"polina", "b34cd4a97cfc6649655a00bfa9cd3fb3acb5f73e59498ee0475b4547db516131"}
    };

    for (auto const &pair: codes) {
        ASSERT_EQ(SHA256::hash(pair.first), pair.second);
    }
}

TEST(basic, blank_erase) {
    Csmt<> tree;

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(check_proof(tree, 0u));

    tree.erase(0);

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(check_proof(tree, 0u));
}

TEST(basic, insert_erase) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(check_proof(tree, 0, {"hello"}));

    tree.erase(0);

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(check_proof(tree, 0u));
}

TEST(basic, update) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(check_proof(tree, 0, {"hello"}));

    tree.insert(0, "world");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(check_proof(tree, 0, {"world"}));
}

TEST(basic, two_nodes) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");
    tree.insert(3, "world");

    ASSERT_TRUE(tree.contains(2));

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(check_proof(tree, 2, {"hello", "helloworld"}));
    ASSERT_TRUE(check_proof(tree, 3, {"world", "helloworld"}));

    tree.erase(6);

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(check_proof(tree, 0));
    ASSERT_TRUE(check_proof(tree, 3, {"world", "helloworld"}));
}

TEST(basic, two_nodes_erase) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");
    tree.insert(3, "world");

    ASSERT_TRUE(tree.contains(2));

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(check_proof(tree, 2, {"hello", "helloworld"}));
    ASSERT_TRUE(check_proof(tree, 3, {"world", "helloworld"}));

    tree.erase(2);

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(check_proof(tree, 0));
    ASSERT_TRUE(check_proof(tree, 3, {"world"}));
}

TEST(basic, not_intersects) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");

    ASSERT_TRUE(check_proof(tree, 2, {"hello"}));

    tree.erase(3);

    ASSERT_TRUE(tree.size() == 1);
    ASSERT_TRUE(check_proof(tree, 2, {"hello"}));
    ASSERT_TRUE(check_proof(tree, 3, {}));
}

TEST(basic, insert_trick) {
    Csmt<> tree;

    tree.insert(12, default_value_gen(12));
    tree.insert(13, default_value_gen(13));
    tree.insert(12, default_value_gen(12));

    ASSERT_TRUE(tree.contains(13));
}

TEST(basic, binary_tree_proof) {
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return std::to_string(key_index);
    };

    Csmt<IdentityHashPolicy> tree;
    for (uint64_t key_index = 0; key_index < 8; ++key_index) {
        tree.insert(key_index, value_gen(key_index));
    }

    ASSERT_TRUE(check_proof(tree, 0, {"0", "01", "0123", "01234567"}));
    ASSERT_TRUE(check_proof(tree, 1, {"1", "01", "0123", "01234567"}));
    ASSERT_TRUE(check_proof(tree, 5, {"5", "45", "4567", "01234567"}));
    ASSERT_TRUE(check_proof(tree, 6, {"6", "67", "4567", "01234567"}));
}