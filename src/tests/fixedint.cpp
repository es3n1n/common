#include <es3n1n/common/math/fixedint.hpp>
#include <gtest/gtest.h>

TEST(bigint, multiplication) {
    auto a = math::Int128(std::numeric_limits<std::uint64_t>::max());
    a *= math::Int128(std::numeric_limits<std::uint64_t>::max());
    EXPECT_EQ(a.to_string(), "fffffffffffffffe0000000000000001");
}
