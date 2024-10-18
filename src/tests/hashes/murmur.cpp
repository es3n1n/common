#include "es3n1n/common/hashes/murmur.hpp"
#include <gtest/gtest.h>

/// Ensure compile-time hashing works
static_assert("hello"_murmur3_32 == 0x248bfa47);

TEST(murmur, murmur3_32) {
    EXPECT_EQ(hashes::Murmur3_32::hash("hello"), 0x248bfa47);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"hello"), 0x248bfa47);
    EXPECT_EQ("hello"_murmur3_32, 0x248bfa47);
}
