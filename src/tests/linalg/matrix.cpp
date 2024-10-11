#include <es3n1n/common/linalg/matrix.hpp>
#include <gtest/gtest.h>

TEST(matrix, construction_and_access) {
    linalg::Matrix<int, 2, 2> m{{1, 2}, {3, 4}};

    EXPECT_EQ(m(0, 0), 1);
    EXPECT_EQ(m(0, 1), 2);
    EXPECT_EQ(m(1, 0), 3);
    EXPECT_EQ(m(1, 1), 4);

    EXPECT_EQ(m[0][0], 1);
    EXPECT_EQ(m[0][1], 2);
    EXPECT_EQ(m[1][0], 3);
    EXPECT_EQ(m[1][1], 4);
}

TEST(matrix, addition) {
    linalg::Matrix<int, 2, 2> m1{{1, 2}, {3, 4}};
    linalg::Matrix<int, 2, 2> m2{{5, 6}, {7, 8}};

    auto result = m1 + m2;

    EXPECT_EQ(result(0, 0), 6);
    EXPECT_EQ(result(0, 1), 8);
    EXPECT_EQ(result(1, 0), 10);
    EXPECT_EQ(result(1, 1), 12);
}

TEST(matrix, subtraction) {
    linalg::Matrix<int, 2, 2> m1{{5, 6}, {7, 8}};
    linalg::Matrix<int, 2, 2> m2{{1, 2}, {3, 4}};

    auto result = m1 - m2;

    EXPECT_EQ(result(0, 0), 4);
    EXPECT_EQ(result(0, 1), 4);
    EXPECT_EQ(result(1, 0), 4);
    EXPECT_EQ(result(1, 1), 4);
}

TEST(matrix, scalar_multiplication) {
    linalg::Matrix<int, 2, 2> m{{1, 2}, {3, 4}};

    auto result = m * 2;

    EXPECT_EQ(result(0, 0), 2);
    EXPECT_EQ(result(0, 1), 4);
    EXPECT_EQ(result(1, 0), 6);
    EXPECT_EQ(result(1, 1), 8);
}

TEST(matrix, scalar_division) {
    linalg::Matrix<float, 2, 2> m{{2.0f, 4.0f}, {6.0f, 8.0f}};

    auto result = m / 2.0f;

    EXPECT_FLOAT_EQ(result(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(result(0, 1), 2.0f);
    EXPECT_FLOAT_EQ(result(1, 0), 3.0f);
    EXPECT_FLOAT_EQ(result(1, 1), 4.0f);
}

TEST(matrix, matrix_multiplication) {
    linalg::Matrix<int, 2, 2> m1{{1, 2}, {3, 4}};
    linalg::Matrix<int, 2, 2> m2{{5, 6}, {7, 8}};

    auto result = m1 * m2;

    EXPECT_EQ(result(0, 0), 19);
    EXPECT_EQ(result(0, 1), 22);
    EXPECT_EQ(result(1, 0), 43);
    EXPECT_EQ(result(1, 1), 50);
}

TEST(matrix, vector_multiplication) {
    linalg::Matrix<int, 2, 2> m{{1, 2}, {3, 4}};
    linalg::Vector<int, 2> v{5, 6};

    auto result = m * v;

    EXPECT_EQ(result[0], 17);
    EXPECT_EQ(result[1], 39);
}

TEST(matrix, comparison) {
    linalg::Matrix<int, 2, 2> m1{{1, 2}, {3, 4}};
    linalg::Matrix<int, 2, 2> m2{{1, 2}, {3, 4}};
    linalg::Matrix<int, 2, 2> m3{{5, 6}, {7, 8}};

    EXPECT_EQ(m1, m2);
    EXPECT_NE(m1, m3);
    EXPECT_LT(m1, m3);
    EXPECT_GT(m3, m1);
}

TEST(matrix, compound_assignment) {
    linalg::Matrix<int, 2, 2> m{{1, 2}, {3, 4}};

    m += linalg::Matrix<int, 2, 2>{{1, 1}, {1, 1}};
    EXPECT_EQ(m(0, 0), 2);
    EXPECT_EQ(m(0, 1), 3);
    EXPECT_EQ(m(1, 0), 4);
    EXPECT_EQ(m(1, 1), 5);

    m -= linalg::Matrix<int, 2, 2>{{1, 1}, {1, 1}};
    EXPECT_EQ(m(0, 0), 1);
    EXPECT_EQ(m(0, 1), 2);
    EXPECT_EQ(m(1, 0), 3);
    EXPECT_EQ(m(1, 1), 4);

    m *= 2;
    EXPECT_EQ(m(0, 0), 2);
    EXPECT_EQ(m(0, 1), 4);
    EXPECT_EQ(m(1, 0), 6);
    EXPECT_EQ(m(1, 1), 8);

    m /= 2;
    EXPECT_EQ(m(0, 0), 1);
    EXPECT_EQ(m(0, 1), 2);
    EXPECT_EQ(m(1, 0), 3);
    EXPECT_EQ(m(1, 1), 4);
}

TEST(matrix, invalid_construction) {
    EXPECT_THROW((linalg::Matrix<int, 2, 2>{{1, 2, 3}, {4, 5, 6}}), std::invalid_argument);
    EXPECT_THROW((linalg::Matrix<int, 2, 2>{{1, 2}, {3, 4}, {5, 6}}), std::invalid_argument);
}
