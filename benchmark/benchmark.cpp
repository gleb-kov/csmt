#include "hash_policy.h"
#include "src/csmt.h"
#include "utils.h"

#include <iostream>

/***  choose hash policy for benchmarks ***/
// #define DEFAULT_POLICY
#define IDENTITY_POLICY
// #define SHA256_POLICY
// #define SHA256_TREE_POLICY


#ifdef DEFAULT_POLICY
    using tree_type = Csmt<>;
    const char *POLICY_STR = "Default";
#elif defined(IDENTITY_POLICY)
    using tree_type = Csmt<IdentityHashPolicy>;
    const char *POLICY_STR = "Identity";
#elif defined(SHA256_POLICY)
    using tree_type = Csmt<HashPolicySHA256>;
    const char *POLICY_STR = "SHA256";
#elif defined(SHA256_TREE_POLICY)
    using tree_type = Csmt<HashPolicySHA256>;
    const char *POLICY_STR = "SHA256_TREE";
#elif
    using tree_type = Csmt<>;
#endif

int main() {
    std::cout << "RUN CSMT BENCHMARKS. HASH POLICY: " << POLICY_STR << std::endl << std::endl;
}
