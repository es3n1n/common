#include "es3n1n/common/hashes/murmur.hpp"
#include <gtest/gtest.h>

/// Ensure compile-time hashing works
static_assert("hello"_murmur3_32 == 0x248bfa47);

TEST(murmur, murmur3_32) {
    constexpr auto kWideCharsIsLong = sizeof(wchar_t) == 4;

    // hash(L"hello") != hash("hello") because its hashing bytes instead of characters
    EXPECT_EQ(hashes::Murmur3_32::hash("hello"), 0x248bfa47);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"hello"), !kWideCharsIsLong ? 0xd7c31989 : 0xd0e2ee4b);

    // shorter than 4 bytes
    EXPECT_EQ(hashes::Murmur3_32::hash("ab"), 0x9bbfd75f);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"ab"), !kWideCharsIsLong ? 0x165860ca : 0x7cf3789f);
    // exactly 4 bytes
    EXPECT_EQ(hashes::Murmur3_32::hash("abcd"), 0x43ed676a);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"abcd"), !kWideCharsIsLong ? 0x44fa9d34 : 0xb44ed6e7);
    // longer than 4 bytes but not a multiple of 4
    EXPECT_EQ(hashes::Murmur3_32::hash("abcdefg"), 0x883c9b06);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"abcdefg"), !kWideCharsIsLong ? 0x9516bf23 : 0x939ff012);
    // longer than 4 bytes and a multiple of 4
    EXPECT_EQ(hashes::Murmur3_32::hash("abcdefghijklmnop"), 0xe76291ed);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"abcdefghijklmnop"), !kWideCharsIsLong ? 0x22c9eb8 : 0xf75c9c46);
    // long strings with multiple blocks
    EXPECT_EQ(hashes::Murmur3_32::hash("This is a long string to test multiple blocks in the Murmur3 hash function implementation."), 0xedf8ff70);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"This is a long string to test multiple blocks in the Murmur3 hash function implementation."),
              !kWideCharsIsLong ? 0xaeb1e6d9 : 0x97a66461);
    // unicode characters
    EXPECT_EQ(hashes::Murmur3_32::hash(L"Hi あいうえお"), !kWideCharsIsLong ? 0x35cabd5 : 0x76d77ff1);
    // empty string
    EXPECT_EQ(hashes::Murmur3_32::hash(""), 0);
    EXPECT_EQ(hashes::Murmur3_32::hash("\0"), 0);
    EXPECT_EQ(hashes::Murmur3_32::hash(L""), 0);
    EXPECT_EQ(hashes::Murmur3_32::hash(L"\0"), 0);
}
