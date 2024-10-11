#include "es3n1n/common/strong_integral.hpp"
#include <gtest/gtest.h>

using TestInt0 = types::StrongIntegral<int, struct TestTag0>;
using TestInt0_1 = types::StrongIntegral<int, struct TestTag0>;
using TestInt1 = types::StrongIntegral<int, struct TestTag1>;
using TestInt = types::StrongIntegral<int, struct TestTag>;

/// We can compare two strong integrals if they have the same tag
static_assert(std::is_same_v<TestInt0, TestInt0_1>);
static_assert(std::is_invocable_r_v<bool, std::equal_to<>, TestInt0, TestInt0_1>);

/// We cannot compare two strong integrals if they have different tags
static_assert(!std::is_same_v<TestInt0, TestInt1>);
static_assert(!std::is_invocable_r_v<bool, std::equal_to<>, TestInt0, TestInt1>);

TEST(strong_integral, default_constructor) {
    TestInt a;
    EXPECT_EQ(a.value(), 0);
}

TEST(strong_integral, value_constructor) {
    TestInt a(42);
    EXPECT_EQ(a.value(), 42);
}

TEST(strong_integral, explicit_conversion) {
    TestInt a(42);
    int value = static_cast<int>(a);
    EXPECT_EQ(value, 42);
}

TEST(strong_integral, increment) {
    TestInt a(41);
    ++a;
    EXPECT_EQ(a.value(), 42);

    TestInt b(41);
    TestInt c = b++;
    EXPECT_EQ(b.value(), 42);
    EXPECT_EQ(c.value(), 41);
}

TEST(strong_integral, decrement) {
    TestInt a(43);
    --a;
    EXPECT_EQ(a.value(), 42);

    TestInt b(43);
    TestInt c = b--;
    EXPECT_EQ(b.value(), 42);
    EXPECT_EQ(c.value(), 43);
}

TEST(strong_integral, addition_assignment) {
    TestInt a(40);
    TestInt b(2);
    a += b;
    EXPECT_EQ(a.value(), 42);
}

TEST(strong_integral, substraction_assignment) {
    TestInt a(44);
    TestInt b(2);
    a -= b;
    EXPECT_EQ(a.value(), 42);
}

TEST(strong_integral, multiplication_assignment) {
    TestInt a(21);
    TestInt b(2);
    a *= b;
    EXPECT_EQ(a.value(), 42);
}

TEST(strong_integral, division_assignment) {
    TestInt a(84);
    TestInt b(2);
    a /= b;
    EXPECT_EQ(a.value(), 42);
}

TEST(strong_integral, modulo_assignment) {
    TestInt a(44);
    TestInt b(5);
    a %= b;
    EXPECT_EQ(a.value(), 4);
}

TEST(strong_integral, comparasion) {
    TestInt a(41);
    TestInt b(42);
    TestInt c(42);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(b >= c);
    EXPECT_TRUE(b <= c);
    EXPECT_TRUE(b == c);
    EXPECT_TRUE(a != b);
}
