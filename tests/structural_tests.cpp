#include "benchmark/hash_policy.h"
#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "tests/common/test_utils.h"

#include <string>
#include <algorithm>

using namespace test;

typedef Csmt<HashPolicySHA256Tree> tree_t;

class structural_tests {

private:
    static void check_structure(tree_t const &tree, std::string const &representation, size_t level) {

    }

    static bool check_same_structure(
            std::unique_ptr<tree_t::Node> const &tree1,
            std::unique_ptr<tree_t::Node> const &tree2
    ) {
        if (tree1->is_leaf()) {
            if (!tree2->is_leaf()) {
                return false;
            }
            return tree1->get_key() == tree2->get_key() &&
                   tree1->get_value() == tree2->get_value();
        } else {
            if (tree2->is_leaf()) {
                return false;
            }
            return check_same_structure(tree1->left_, tree2->left_) &&
                   check_same_structure(tree1->right_, tree2->right_);
        }
    }

    static size_t count_memory(std::unique_ptr<tree_t::Node> const &tree) {

    }

public:
    static bool check_structure(tree_t const &tree, std::string const &representation) {

    }

    static bool check_same_structure(tree_t const &tree1, tree_t const &tree2) {
        return check_same_structure(tree1.root_, tree2.root_);
    }

    static size_t count_memory(tree_t const &tree) {

    }

};

TEST(structural, history_independence_three) {
    tree_t tree1;
    tree_t tree2;

    tree1.insert(1, default_value_gen(1));
    tree1.insert(2, default_value_gen(2));
    tree1.insert(3, default_value_gen(3));

    tree2.insert(3, default_value_gen(3));
    tree2.insert(2, default_value_gen(2));
    tree2.insert(1, default_value_gen(1));

    ASSERT_TRUE(structural_tests::check_same_structure(tree1, tree2));
}

TEST(structural, history_independence_large) {
    constexpr size_t SIZE = 1000;
    constexpr size_t TREES = 10;

    std::vector<uint64_t> keys;
    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    for (size_t i = 0; i < SIZE; i++) {
        keys.push_back(key_gen(rnd()));
    }

    std::vector<tree_t> trees(TREES);
    for (size_t i = 0; i < TREES; i++) {
        std::shuffle(keys.begin(), keys.end(), rnd());
        for (uint64_t key: keys) {
            trees[i].insert(key, default_value_gen(key));
        }
    }

    for (size_t i = 1; i < TREES; i++) {
        ASSERT_TRUE(structural_tests::check_same_structure(trees[i - 1], trees[i]));
    }
}