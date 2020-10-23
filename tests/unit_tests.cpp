#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "tests/common/test_utils.h"

#include <functional>

using namespace test;

TEST(basic, blank_erase) {
    Csmt<> tree;

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(look_for_key(tree, 0u));

    tree.erase(0);

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(look_for_key(tree, 0u));
}

TEST(basic, insert_erase) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0, {"hello"}));

    tree.erase(0);

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(look_for_key(tree, 0u));
}

TEST(basic, update) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0, {"hello"}));

    tree.insert(0, "world");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0, {"world"}));
}

TEST(basic, two_nodes) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");
    tree.insert(3, "world");

    ASSERT_TRUE(tree.contains(2));

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(look_for_key(tree, 2, {"hello", "helloworld"}));
    ASSERT_TRUE(look_for_key(tree, 3, {"world", "helloworld"}));

    tree.erase(6);

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(look_for_key(tree, 0));
    ASSERT_TRUE(look_for_key(tree, 3, {"world", "helloworld"}));
}

TEST(basic, two_nodes_erase) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");
    tree.insert(3, "world");

    ASSERT_TRUE(tree.contains(2));

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(look_for_key(tree, 2, {"hello", "helloworld"}));
    ASSERT_TRUE(look_for_key(tree, 3, {"world", "helloworld"}));

    tree.erase(2);

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0));
    ASSERT_TRUE(look_for_key(tree, 3, {"world"}));
}

TEST(basic, not_intersects) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");

    ASSERT_TRUE(look_for_key(tree, 2, {"hello"}));

    tree.erase(3);

    ASSERT_TRUE(tree.size() == 1);
    ASSERT_TRUE(look_for_key(tree, 2, {"hello"}));
    ASSERT_TRUE(look_for_key(tree, 3, {}));
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

    ASSERT_TRUE(look_for_key(tree, 0, {"0", "01", "0123", "01234567"}));
    ASSERT_TRUE(look_for_key(tree, 1, {"1", "01", "0123", "01234567"}));
    ASSERT_TRUE(look_for_key(tree, 5, {"5", "45", "4567", "01234567"}));
    ASSERT_TRUE(look_for_key(tree, 6, {"6", "67", "4567", "01234567"}));
}
