#include "es3n1n/common/hashes/murmur.hpp"
#include <gtest/gtest.h>

/// Ensure compile-time hashing works
static_assert("hello"_murmur3_32 == 0x248bfa47);

TEST(murmur, murmur3_32) {
    // hash(L"hello") != hash("hello")
    EXPECT_EQ(hashes::Murmur3_32::hash("hello"), 0x248bfa47);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"hello"), 0xd7c31989);

    // shorter than 4 bytes
    EXPECT_EQ(hashes::Murmur3_32::hash("ab"), 0x9bbfd75f);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"ab"), 0x165860ca);
    // exactly 4 bytes
    EXPECT_EQ(hashes::Murmur3_32::hash("abcd"), 0x43ed676a);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"abcd"), 0x44fa9d34);
    // longer than 4 bytes but not a multiple of 4
    EXPECT_EQ(hashes::Murmur3_32::hash("abcdefg"), 0x883c9b06);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"abcdefg"), 0x9516bf23);
    // longer than 4 bytes and a multiple of 4
    EXPECT_EQ(hashes::Murmur3_32::hash("abcdefghijklmnop"), 0xe76291ed);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"abcdefghijklmnop"), 0x22c9eb8);
    // long strings with multiple blocks
    EXPECT_EQ(hashes::Murmur3_32::hash("This is a long string to test multiple blocks in the Murmur3 hash function implementation."), 0xedf8ff70);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"This is a long string to test multiple blocks in the Murmur3 hash function implementation."), 0xaeb1e6d9);
    // unicode characters
    EXPECT_EQ(hashes::Murmur3_32::hash(L"Hi あいうえお"), 0x35cabd5);
    // empty string
    EXPECT_EQ(hashes::Murmur3_32::hash(""), 0);
    EXPECT_EQ(hashes::Murmur3_32::hash("\0"), 0);
    EXPECT_EQ(hashes::Murmur3_32::hash(L""), 0);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"\0"), 0);
}
