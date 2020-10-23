#include "benchmark/hash_policy.h"
#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "tests/common/test_utils.h"

#include <string>
#include <algorithm>
#include <vector>

using namespace test;

class Csmt_debug : public Csmt<HashPolicySHA256Tree> {

public:

    typedef std::pair<size_t, std::string> tree_line;

private:
    static bool check_structure(std::unique_ptr<Node> const &tree, std::vector<tree_line> const &repr, size_t &line) {
        if (line >= repr.size()) {
            return false;
        }
        bool is_leaf = (line == repr.size() - 1 || repr[line].first >= repr[line + 1].first);
        if (tree->get_value() != repr[line++].second) {
            return false;
        }
        if (tree->is_leaf()) {
            return is_leaf;
        } else {
            return !is_leaf &&
                   check_structure(tree->left_, repr, line) &&
                   check_structure(tree->right_, repr, line);
        }
    }

    static bool check_same_structure(
            std::unique_ptr<Node> const &tree1,
            std::unique_ptr<Node> const &tree2
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

    static size_t count_memory(std::unique_ptr<Node> const &tree) {

    }

public:
    bool check_structure(std::vector<tree_line> const &representation) {
        size_t line = 0;
        return check_structure(root_, representation, line);
    }

    static bool check_same_structure(Csmt_debug const &tree1, Csmt_debug const &tree2) {
        return check_same_structure(tree1.root_, tree2.root_);
    }

    static size_t count_memory(Csmt_debug const &tree) {

    }

};

TEST(structural, history_independence_three) {
    Csmt_debug tree1;
    Csmt_debug tree2;

    tree1.insert(1, default_value_gen(1));
    tree1.insert(2, default_value_gen(2));
    tree1.insert(3, default_value_gen(3));

    tree2.insert(3, default_value_gen(3));
    tree2.insert(2, default_value_gen(2));
    tree2.insert(1, default_value_gen(1));

    ASSERT_TRUE(Csmt_debug::check_same_structure(tree1, tree2));
}

TEST(structural, history_independence_large) {
    constexpr size_t SIZE = 1000;
    constexpr size_t TREES = 10;

    std::vector<uint64_t> keys;
    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    for (size_t i = 0; i < SIZE; i++) {
        keys.push_back(key_gen(rnd()));
    }

    std::vector<Csmt_debug> trees(TREES);
    for (size_t i = 0; i < TREES; i++) {
        std::shuffle(keys.begin(), keys.end(), rnd());
        for (uint64_t key: keys) {
            trees[i].insert(key, default_value_gen(key));
        }
    }

    for (size_t i = 1; i < TREES; i++) {
        ASSERT_TRUE(Csmt_debug::check_same_structure(trees[i - 1], trees[i]));
    }
}

TEST(structural, full_structure_3_left) {
    Csmt_debug tree_left;

    tree_left.insert(0, default_value_gen(0));
    tree_left.insert(1, default_value_gen(1));
    tree_left.insert(2, default_value_gen(2));

    ASSERT_TRUE(tree_left.check_structure(std::vector<Csmt_debug::tree_line>{
            {0, "18cc5a364e2a67b12e295f8aa0e155542325c024bdc2b8fd0d011ff2739dbdce"},
            {1, "9e7fc8c40b8b14155a3e62c60e34f97c5b025106b89b8520b2382904e7517e5b"},
            {2, "94691a6dca8d9b3529a9ba48bd482eace32358548c0dfab12bfa5f860a1d627d"},
            {2, "36b78f882885cf09ea371be0c69e23567382e4a3ca797899f0400e72b14d0028"},
            {1, "8f1a5e72cf5cec1f94cda9cc8e66cd0a5b0dd64a8188dd1067a4fb28a776e39b"}
    }));
}

TEST(structural, full_structure_3_right) {
    Csmt_debug tree_right;

    tree_right.insert(1, default_value_gen(1));
    tree_right.insert(2, default_value_gen(2));
    tree_right.insert(3, default_value_gen(3));

    ASSERT_TRUE(tree_right.check_structure(std::vector<Csmt_debug::tree_line>{
            {0, "37021bc02e7bdbb3f9f36a2b913ea449f380b7264cc41c90ef313348835480f1"},
            {1, "36b78f882885cf09ea371be0c69e23567382e4a3ca797899f0400e72b14d0028"},
            {1, "561bb32001f03d225e0ea618e1ee113414197f3aa09e5409bd5f14375db2b07f"},
            {2, "8f1a5e72cf5cec1f94cda9cc8e66cd0a5b0dd64a8188dd1067a4fb28a776e39b"},
            {2, "bc1008460b1fde744c529491bc1eb56a312f59cb3b1756e923d6355c6afee8fc"}
    }));
}