#include "contrib/crypto/sha256.h"

#include <iostream>

int main() {
    std::cout << SHA256::hash("grape") << std::endl;
    return 0;
}
