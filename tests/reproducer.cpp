#include "src/csmt.h"

#include <functional>
#include <iostream>
#include <unordered_set>

int main() {
    std::unordered_set<uint64_t> in_tree;

    size_t op_index = 0;
    std::string operation;
    uint64_t key;

    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    Csmt<> tree;

    while (std::cin >> operation >> key) {
        if (operation == "insert") {
            tree.insert(key, value_gen(key));
            in_tree.insert(key);
        } else if (operation == "erase") {
            tree.erase(key);
            in_tree.erase(key);
        } else if (operation == "contains") {
            bool tree_verdict = tree.contains(key);
            bool set_verdict = in_tree.find(key) != in_tree.end();

            std::cout << "CONTAINS " << key << ' ' << tree_verdict << ' ' << set_verdict << std::endl;


            if (tree_verdict != set_verdict) {
                std::cerr << "FAILED. Operation: index " << op_index <<
                        ' ' << operation << ' ' << key << std::endl;
                std::abort();
            }
        }
        ++op_index;
    }
    std::cout << "PASSED";
}