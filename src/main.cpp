#include "contrib/crypto/sha256.h"
#include "csmt.h"

#include <iostream>

/*
 * This dummy file just enables all compile checks like -Wall and -Wextra.
 */

int main() {
    std::cout << SHA256::hash("grape") << std::endl;

    Csmt<> c;
    c.insert(10, "banana");
    std::cout << c.contains(10) << std::endl;
    c.erase(10);

    return 0;
}
