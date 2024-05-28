#include <es3n1n/common/progress.hpp>
#include <gtest/gtest.h>

TEST(progress, basics) {
    /// Disable wcout/cout
    std::cout.setstate(std::ios::failbit);
    std::wcout.setstate(std::ios::failbit);

    /// Testing the abi
    auto progress = progress::Progress("testing", 5);
    for (std::size_t i = 0; i < 5; ++i) {
        progress.step();
    }

    /// Re-enable wcout/cout
    std::cout.clear();
    std::wcout.clear();
}
