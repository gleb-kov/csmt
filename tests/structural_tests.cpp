#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "utils.h"

struct CsmtMock : Csmt<> {
public:
    bool is_root_empty() const {
        return root_ == nullptr;
    }
};
