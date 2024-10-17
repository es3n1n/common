#include "es3n1n/common/hashes/fnv.hpp"
#include <gtest/gtest.h>

class Holder {
public:
    explicit Holder(hashes::Fnv1_32::Value value): value(value) { }

    hashes::Fnv1_32::Value value;
};

TEST(hash_value_type, implicit_conversion_from_string) {
    Holder holder("hello");
    EXPECT_EQ(holder.value, "hello"_fnv1_32);
    EXPECT_EQ(holder.value, 0xb6fa7167);
}

TEST(hash_value_type, implicit_conversion_from_wstring) {
    Holder holder(L"hello");
    EXPECT_EQ(holder.value, "hello"_fnv1_32);
    EXPECT_EQ(holder.value, 0xb6fa7167);
}

TEST(hash_value_type, implicit_conversion_from_integral) {
    Holder holder("hello"_fnv1_32);
    EXPECT_EQ(holder.value, "hello"_fnv1_32);
    EXPECT_EQ(holder.value, 0xb6fa7167);
}
