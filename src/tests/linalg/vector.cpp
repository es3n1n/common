#include <es3n1n/common/linalg/vector.hpp>
#include <gtest/gtest.h>

using namespace linalg;

/// Test vector construction and basic operations
TEST(vector, construction_and_access) {
    Vector<int, 3> v1{1, 2, 3};
    EXPECT_EQ(v1[0], 1);
    EXPECT_EQ(v1[1], 2);
    EXPECT_EQ(v1[2], 3);

    Vector<double, 2> v2;
    EXPECT_DOUBLE_EQ(v2[0], 0.0);
    EXPECT_DOUBLE_EQ(v2[1], 0.0);

    EXPECT_THROW((linalg::Vector<float, 4>{1.0f, 2.0f, 3.0f}), std::invalid_argument);
}

/// Test vector arithmetic operations
TEST(vector, arithmetic_operations) {
    Vector<float, 3> v1{1.0f, 2.0f, 3.0f};
    Vector<float, 3> v2{4.0f, 5.0f, 6.0f};

    auto sum = v1 + v2;
    EXPECT_FLOAT_EQ(sum[0], 5.0f);
    EXPECT_FLOAT_EQ(sum[1], 7.0f);
    EXPECT_FLOAT_EQ(sum[2], 9.0f);

    auto diff = v2 - v1;
    EXPECT_FLOAT_EQ(diff[0], 3.0f);
    EXPECT_FLOAT_EQ(diff[1], 3.0f);
    EXPECT_FLOAT_EQ(diff[2], 3.0f);

    auto scaled = v1 * 2.0f;
    EXPECT_FLOAT_EQ(scaled[0], 2.0f);
    EXPECT_FLOAT_EQ(scaled[1], 4.0f);
    EXPECT_FLOAT_EQ(scaled[2], 6.0f);

    auto divided = v2 / 2.0f;
    EXPECT_FLOAT_EQ(divided[0], 2.0f);
    EXPECT_FLOAT_EQ(divided[1], 2.5f);
    EXPECT_FLOAT_EQ(divided[2], 3.0f);
}

/// Test vector magnitude and normalization
TEST(vector, magnitude_and_normalization) {
    Vector<double, 3> v{3.0, 4.0, 5.0};

    EXPECT_DOUBLE_EQ(v.magnitude(), std::sqrt(50.0));

    auto normalized = v.normalized();
    double expected_magnitude = 1.0;
    EXPECT_NEAR(normalized.magnitude(), expected_magnitude, 1e-10);
}

/// Test dot product
TEST(vector, dot_product) {
    Vector<int, 4> v1{1, 2, 3, 4};
    Vector<int, 4> v2{5, 6, 7, 8};

    EXPECT_EQ(v1.dot(v2), 70);
}

/// Test cross product (only for 3D vectors)
TEST(vector, cross_product) {
    Vector<double, 3> v1{1.0, 2.0, 3.0};
    Vector<double, 3> v2{4.0, 5.0, 6.0};

    auto cross = v1.cross(v2);
    EXPECT_DOUBLE_EQ(cross[0], -3.0);
    EXPECT_DOUBLE_EQ(cross[1], 6.0);
    EXPECT_DOUBLE_EQ(cross[2], -3.0);
}

/// Test component access methods
TEST(vector, component_access) {
    Vector<int, 4> v{1, 2, 3, 4};

    EXPECT_EQ(v.x(), 1);
    EXPECT_EQ(v.y(), 2);
    EXPECT_EQ(v.z(), 3);
    EXPECT_EQ(v.w(), 4);
}

/// Test comparison operators
TEST(vector, comparison) {
    Vector<int, 3> v1{1, 2, 3};
    Vector<int, 3> v2{1, 2, 3};
    Vector<int, 3> v3{3, 2, 1};

    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);
    EXPECT_LT(v1, v3);
    EXPECT_GT(v3, v1);
}

#if defined(COMMON_HAS_IMGUI)
/// Test ImGui conversions
TEST(vector, imgui_conversions) {
    Vector<float, 2> v2{1.0f, 2.0f};
    ImVec2 iv2 = v2;
    EXPECT_FLOAT_EQ(iv2.x, 1.0f);
    EXPECT_FLOAT_EQ(iv2.y, 2.0f);

    Vector<float, 4> v4{1.0f, 2.0f, 3.0f, 4.0f};
    ImVec4 iv4 = v4;
    EXPECT_FLOAT_EQ(iv4.x, 1.0f);
    EXPECT_FLOAT_EQ(iv4.y, 2.0f);
    EXPECT_FLOAT_EQ(iv4.z, 3.0f);
    EXPECT_FLOAT_EQ(iv4.w, 4.0f);
}
#endif
