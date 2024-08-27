#include <es3n1n/common/memory/reader.hpp>
#include <es3n1n/common/memory/address.hpp>
#include <gtest/gtest.h>

inline std::expected<std::size_t, memory::e_error_code> read_impl(void* buffer, const std::uintptr_t address, const std::size_t size) {
    throw std::runtime_error("");
}

inline std::expected<std::size_t, memory::e_error_code> write_impl(std::uintptr_t address, const void* buffer, const std::size_t size) {
    throw std::runtime_error("");
}

TEST(reader, read_write_impls) {
    memory::reader.read_primitive(read_impl);
    memory::reader.write_primitive(write_impl);

    EXPECT_THROW(memory::reader.read<int>(0x1234), std::runtime_error);
    EXPECT_THROW(memory::reader.write<int>(nullptr, 0), std::runtime_error);

    EXPECT_THROW(memory::address(0x1234).read<int>(), std::runtime_error);
    EXPECT_THROW(memory::address(0x1234).write<int>(0), std::runtime_error);
}
