#include "utils.h"

#include <iostream>

int main() {
    time_utils::stage_timer st;
    std::cout << "benchmark stage 1" << std::endl;
    std::cout << "Elapsed: " << st.stop_stage().count() << " ms." << std::endl;
    std::cout << "benchmark stage 2" << std::endl;
    std::cout << "Elapsed: " << st.stop_stage().count() << " ms.";
}
