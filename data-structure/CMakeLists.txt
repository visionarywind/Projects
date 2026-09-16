cmake_minimum_required(VERSION 3.16)
project(rbtree LANGUAGES CXX)

option(RBTREE_BUILD_TESTS "Build red-black tree tests" ON)
option(RBTREE_BUILD_EXAMPLES "Build red-black tree examples" ON)
option(RBTREE_BUILD_BENCHMARKS "Build red-black tree benchmarks" OFF)
option(RBTREE_BUILD_ORDERED_BENCHMARKS "Compare rb_tree, skip_list, and std::set" OFF)

add_library(rbtree INTERFACE)
target_include_directories(rbtree INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(rbtree INTERFACE cxx_std_17)

if(RBTREE_BUILD_EXAMPLES)
    add_executable(rbtree_demo examples/basic_usage.cpp)
    target_link_libraries(rbtree_demo PRIVATE rbtree)
    target_compile_options(rbtree_demo PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(RBTREE_BUILD_TESTS)
    enable_testing()
    add_executable(rbtree_tests
        tests/test_main.cpp
        tests/rb_tree_basic_test.cpp
        tests/rb_tree_insert_test.cpp
        tests/rb_tree_erase_test.cpp
        tests/rb_tree_iterator_test.cpp
        tests/rb_tree_differential_test.cpp
        tests/rb_map_test.cpp
        tests/rb_map_differential_test.cpp
        tests/skip_list_test.cpp
        tests/llrb_tree_test.cpp
    )
    target_link_libraries(rbtree_tests PRIVATE rbtree)
    target_compile_options(rbtree_tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    add_test(NAME rbtree_tests COMMAND rbtree_tests)
endif()

if(RBTREE_BUILD_BENCHMARKS)
    add_executable(rbtree_benchmark benchmark/rb_map_benchmark.cpp)
    target_link_libraries(rbtree_benchmark PRIVATE rbtree)
    target_compile_options(rbtree_benchmark PRIVATE -O3 -DNDEBUG -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(RBTREE_BUILD_ORDERED_BENCHMARKS)
    add_executable(ordered_benchmark benchmark/ordered_benchmark.cpp)
    target_link_libraries(ordered_benchmark PRIVATE rbtree)
    target_compile_options(ordered_benchmark PRIVATE -O3 -DNDEBUG -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
