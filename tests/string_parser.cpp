#include <es3n1n/common/string_parser.hpp>
#include <gtest/gtest.h>

TEST(string_parser, basics) {
    EXPECT_EQ(string_parser::parse_int64("-123"), -123);
    EXPECT_EQ(string_parser::parse_int64("0x123", 16), 0x123);
    EXPECT_EQ(string_parser::parse_int64("0X123", 16), 0x123);
    EXPECT_EQ(string_parser::parse_int64("-0X123", 16), -0x123);
    EXPECT_EQ(string_parser::parse_int64("-0x123", 16), -0x123);

    EXPECT_EQ(string_parser::parse_uint64("123"), 123);
    EXPECT_EQ(string_parser::parse_uint64("0x123", 16), 0x123);
    EXPECT_EQ(string_parser::parse_uint64("0X123", 16), 0x123);

    EXPECT_EQ(string_parser::parse_int32("-123"), -123);
    EXPECT_EQ(string_parser::parse_int32("0x123", 16), 0x123);
    EXPECT_EQ(string_parser::parse_int32("0X123", 16), 0x123);
    EXPECT_EQ(string_parser::parse_int32("-0X123", 16), -0x123);
    EXPECT_EQ(string_parser::parse_int32("-0x123", 16), -0x123);

    EXPECT_EQ(string_parser::parse_uint32("123"), 123);
    EXPECT_EQ(string_parser::parse_uint32("0x123", 16), 0x123);
    EXPECT_EQ(string_parser::parse_uint32("0X123", 16), 0x123);

    EXPECT_EQ(string_parser::parse_int8("-12"), -12);
    EXPECT_EQ(string_parser::parse_int8("0x12", 16), 0x12);
    EXPECT_EQ(string_parser::parse_int8("0X12", 16), 0x12);
    EXPECT_EQ(string_parser::parse_int8("-0X12", 16), -0x12);
    EXPECT_EQ(string_parser::parse_int8("-0x12", 16), -0x12);

    EXPECT_EQ(string_parser::parse_uint8("12"), 12);
    EXPECT_EQ(string_parser::parse_uint8("0x12", 16), 0x12);
    EXPECT_EQ(string_parser::parse_uint8("0X12", 16), 0x12);

    EXPECT_EQ(string_parser::parse_bool("true"), true);
    EXPECT_EQ(string_parser::parse_bool("1"), true);
    EXPECT_EQ(string_parser::parse_bool("false"), false);
    EXPECT_EQ(string_parser::parse_bool("0"), false);
    EXPECT_EQ(string_parser::parse_bool("asd"), false);

    EXPECT_EQ(string_parser::serialize(0UL), "0");

    std::any var;
    var.emplace<uint32_t>(0UL);
    string_parser::parse_to_any(var, "333");
    EXPECT_EQ(std::any_cast<uint32_t>(var), 333);
    EXPECT_EQ(string_parser::serialize_any(var), "333");
}
