# Compact Sparse Merkle Tree

Paper: https://eprint.iacr.org/2018/955.pdf

Tree operations:
- Inserting a key: insert(key, value)
- Membership proof for a key: membership_proof(key)
- Deleting a key: erase(key)
- Contains a key: contains(key)
- Size of tree: size()

Structure: nearly balanced.
Space: O(n).

## How to start
One-include lib. Just [csmt.h](/src/csmt.h) file.

See examples of usage in tests.

## Tests
Unit and integration tests are [here](/tests), implemented with GTest.

## Performance
Benchmarks of CSMT structure and utils code are [here](/benchmark).
