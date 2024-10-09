#include <cstring>
#include <es3n1n/common/files.hpp>
#include <gtest/gtest.h>

TEST(files, basics) {
    auto path = std::filesystem::temp_directory_path();
    path /= "common.bin";

    uint8_t test_data[] = {0x11, 0x33, 0x33, 0x77};

    files::write_file(path, test_data, sizeof(test_data));
    EXPECT_TRUE(exists(path));

    auto test_out_data = files::read_file(path);

    EXPECT_EQ(std::memcmp(test_data, test_out_data.data(), sizeof(test_data)), 0);
    remove(path);
}