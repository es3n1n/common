#include "es3n1n/common/hashes/fnv.hpp"
#include <gtest/gtest.h>

/// Ensure compile-time hashing works
static_assert("hello"_fnv1_32 == 0xb6fa7167);
static_assert("hello"_fnv1_64 == 0x7b495389bdbdd4c7);

static_assert("hello"_fnv1a_32 == 0x4f9f2cab);
static_assert("hello"_fnv1a_64 == 0xa430d84680aabd0b);

TEST(fnv, fnv1) {
    EXPECT_EQ("hello"_fnv1_32, 0xb6fa7167);
    EXPECT_EQ(hashes::Fnv1_32::hash("hello"), 0xb6fa7167);
    EXPECT_EQ(hashes::Fnv1_32::hash(L"hello"), 0xb6fa7167);

    EXPECT_EQ("hello"_fnv1_64, 0x7b495389bdbdd4c7);
    EXPECT_EQ(hashes::Fnv1_64::hash("hello"), 0x7b495389bdbdd4c7);
    EXPECT_EQ(hashes::Fnv1_64::hash(L"hello"), 0x7b495389bdbdd4c7);
}

TEST(fnv, fnv1a) {
    EXPECT_EQ("hello"_fnv1a_32, 0x4f9f2cab);
    EXPECT_EQ(hashes::Fnv1a_32::hash("hello"), 0x4f9f2cab);
    EXPECT_EQ(hashes::Fnv1a_32::hash(L"hello"), 0x4f9f2cab);

    EXPECT_EQ("hello"_fnv1a_64, 0xa430d84680aabd0b);
    EXPECT_EQ(hashes::Fnv1a_64::hash("hello"), 0xa430d84680aabd0b);
    EXPECT_EQ(hashes::Fnv1a_64::hash(L"hello"), 0xa430d84680aabd0b);
}
