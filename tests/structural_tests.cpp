#include "benchmark/hash_policy.h"
#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "utils.h"

#include <algorithm>
#include <string>
#include <random>
#include <unordered_set>
#include <vector>

class CsmtStructuralWrapper : public Csmt<HashPolicySHA256Tree> {
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
        size_t count = 1;
        if (!tree->is_leaf()) {
            count += count_memory(tree->left_);
            count += count_memory(tree->right_);
        }
        return count;
    }

public:
    bool check_structure(std::vector<tree_line> const &representation) {
        size_t line = 0;
        return check_structure(root_, representation, line);
    }

    static bool check_same_structure(CsmtStructuralWrapper const &tree1, CsmtStructuralWrapper const &tree2) {
        return check_same_structure(tree1.root_, tree2.root_);
    }

    static size_t count_memory(CsmtStructuralWrapper const &tree) {
        return count_memory(tree.root_);
    }

};

TEST(structural, history_independence_three) {
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    CsmtStructuralWrapper tree1;
    CsmtStructuralWrapper tree2;

    tree1.insert(1, value_gen(1));
    tree1.insert(2, value_gen(2));
    tree1.insert(3, value_gen(3));

    tree2.insert(3, value_gen(3));
    tree2.insert(2, value_gen(2));
    tree2.insert(1, value_gen(1));

    ASSERT_TRUE(CsmtStructuralWrapper::check_same_structure(tree1, tree2));
}

TEST(structural, history_independence_large) {
    constexpr size_t SIZE = 1000;
    constexpr size_t TREES = 10;

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    std::vector<CsmtStructuralWrapper> trees(TREES);
    std::vector<uint64_t> keys;

    for (size_t i = 0; i < SIZE; i++) {
        keys.push_back(key_gen(generator));
    }

    for (size_t i = 0; i < TREES; i++) {
        std::shuffle(keys.begin(), keys.end(), generator);
        for (uint64_t key: keys) {
            trees[i].insert(key, value_gen(key));
        }
    }

    for (size_t i = 1; i < TREES; i++) {
        ASSERT_TRUE(CsmtStructuralWrapper::check_same_structure(trees[i - 1], trees[i]));
    }
}

TEST(structural, full_structure_3_left) {
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    CsmtStructuralWrapper tree_left;

    tree_left.insert(0, value_gen(0));
    tree_left.insert(1, value_gen(1));
    tree_left.insert(2, value_gen(2));

    ASSERT_TRUE(tree_left.check_structure(std::vector<CsmtStructuralWrapper::tree_line>{
            {0, "18cc5a364e2a67b12e295f8aa0e155542325c024bdc2b8fd0d011ff2739dbdce"},
            {1, "9e7fc8c40b8b14155a3e62c60e34f97c5b025106b89b8520b2382904e7517e5b"},
            {2, "94691a6dca8d9b3529a9ba48bd482eace32358548c0dfab12bfa5f860a1d627d"},
            {2, "36b78f882885cf09ea371be0c69e23567382e4a3ca797899f0400e72b14d0028"},
            {1, "8f1a5e72cf5cec1f94cda9cc8e66cd0a5b0dd64a8188dd1067a4fb28a776e39b"}
    }));
}

TEST(structural, full_structure_3_right) {
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    CsmtStructuralWrapper tree_right;

    tree_right.insert(1, value_gen(1));
    tree_right.insert(2, value_gen(2));
    tree_right.insert(3, value_gen(3));

    ASSERT_TRUE(tree_right.check_structure(std::vector<CsmtStructuralWrapper::tree_line>{
            {0, "37021bc02e7bdbb3f9f36a2b913ea449f380b7264cc41c90ef313348835480f1"},
            {1, "36b78f882885cf09ea371be0c69e23567382e4a3ca797899f0400e72b14d0028"},
            {1, "561bb32001f03d225e0ea618e1ee113414197f3aa09e5409bd5f14375db2b07f"},
            {2, "8f1a5e72cf5cec1f94cda9cc8e66cd0a5b0dd64a8188dd1067a4fb28a776e39b"},
            {2, "bc1008460b1fde744c529491bc1eb56a312f59cb3b1756e923d6355c6afee8fc"}
    }));
}

TEST(structural, full_structure_large) {
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    CsmtStructuralWrapper tree;

    std::vector<uint64_t> keys{
            1ull, 1000ull, 121322ull, 127837878213ull,
            2168736525757624ull, 3268768623546567234ull,
            2676886ull, 0ull, 126726767ull, 1224ull,
    };
    for (uint64_t key: keys) {
        tree.insert(key, value_gen(key));
    }

    ASSERT_TRUE(tree.check_structure(std::vector<CsmtStructuralWrapper::tree_line>{
            {0, "512bf76b73ba4fd6c87a781dc984b1526699804ededccc6842acce94fb21b398"},
            {1, "c4dcacf404b871a35edf1dd4d510c9400255438d9ceea037e4986eaecedd5897"},
            {2, "183cabebe4c4c65115fec0abd0c445c31cc61dcada06cb32d3eb91c8fefcc2a1"},
            {3, "652c7ad12b60e0ee0a9332dca284dc268a524a527b3de2784f94c86c3af0139c"},
            {4, "dbfc8c8d8f473e7f68b0b4e9026bc0cb8bbb7022ef81fbd5c0de5df14e05942d"},
            {5, "b3f7bd14cdf373e867525bca6b349516dda7afb07c875425dae6dcf9b5d55e23"},
            {6, "99478f202307cddbde3fbaf986559f38adb8a0efdfb27b390c85f8248b0d1ec0"},
            {7, "8395a9519da154e8dc4d669d82febc3f7c6fa11802892a0254e3f5aba5da68a8"},
            {8, "9e7fc8c40b8b14155a3e62c60e34f97c5b025106b89b8520b2382904e7517e5b"},
            {9, "94691a6dca8d9b3529a9ba48bd482eace32358548c0dfab12bfa5f860a1d627d"},
            {9, "36b78f882885cf09ea371be0c69e23567382e4a3ca797899f0400e72b14d0028"},
            {8, "35b81a2e4955c6b8616c12c2faf92a523666258f1bfb0598e03f538ca4b67ebf"},
            {7, "2bac7b94a6ae77250a4c6c1622e5e206e32ef4a0790779cd29456b12535e2915"},
            {6, "d4ae612adcadc1945cfcb292e2c5320a73cd23491d823a0a8929d66d4e82182e"},
            {5, "fe6214cf008a219ee29535f502b9ed01e31d964eae6c04dbfda8b588fdbcb76a"},
            {4, "aaa0c1897a8e74325936e6ec52bc4374fbb51c1dfa835c8304b95e29911fcde4"},
            {3, "b3a56958d1448d041ab95450b03540ebfd4d05d2d4f2163b4e88e03c6d9d48f9"},
            {2, "e89d5d683cc1ed820a422250159e1d7662cda800416f7288951c2e6a9965f200"},
            {1, "dbd8804924bac497e5bacd60e49e16e68dff93374661defcce3c379d6157b728"}
    }));
}

TEST(structural, low_memory_check) {
    constexpr size_t OPERATIONS = 100000;
    constexpr size_t EACH = 10000;

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    CsmtStructuralWrapper tree;
    std::unordered_set<uint64_t> keys;

    for (size_t iter = 0; iter < OPERATIONS; iter++) {
        uint64_t key = key_gen(generator);
        while (keys.count(key)) {
            key = key_gen(generator);
        }
        keys.insert(key);
        tree.insert(key, value_gen(key));

        if (iter % EACH == 0) {
            ASSERT_LE(CsmtStructuralWrapper::count_memory(tree), 2 * iter + 1);
        }
    }
}
