#pragma once

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace test_support {

inline int failures = 0;

inline void check(bool condition, const char* expression, const char* file, int line) {
    if (!condition) {
        ++failures;
        std::cerr << file << ':' << line << ": check failed: " << expression << '\n';
    }
}

template <typename Left, typename Right>
void check_equal(const Left& left, const Right& right,
                 const char* left_expression, const char* right_expression,
                 const char* file, int line) {
    if (!(left == right)) {
        ++failures;
        std::cerr << file << ':' << line << ": expected " << left_expression
                  << " == " << right_expression << '\n';
    }
}

inline void section(const char* name) {
    std::cout << "[test] " << name << '\n';
}

}  // namespace test_support

#define CHECK(condition) \
    ::test_support::check((condition), #condition, __FILE__, __LINE__)
#define CHECK_EQ(left, right) \
    ::test_support::check_equal((left), (right), #left, #right, __FILE__, __LINE__)
#define TEST_SECTION(name) ::test_support::section(name)
