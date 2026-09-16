#include "test_support.hpp"

void run_basic_tests();
void run_insert_tests();
void run_erase_tests();
void run_iterator_tests();
void run_differential_tests();
void run_map_tests();
void run_map_differential_tests();
void run_skip_list_tests();
void run_llrb_tree_tests();

int main() {
    run_basic_tests();
    run_insert_tests();
    run_erase_tests();
    run_iterator_tests();
    run_differential_tests();
    run_map_tests();
    run_map_differential_tests();
    run_skip_list_tests();
    run_llrb_tree_tests();

    if (test_support::failures != 0) {
        std::cerr << test_support::failures << " check(s) failed\n";
        return 1;
    }
    std::cout << "all checks passed\n";
    return 0;
}
