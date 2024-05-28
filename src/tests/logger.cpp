#include <es3n1n/common/logger.hpp>
#include <gtest/gtest.h>

TEST(logger, basics) {
    /// Just test the abi that everything is still there

    auto pass = []() -> void {
        logger::debug("hey! {:#x} {}", 0x1337, "hehe");
        logger::debug(L"wide hey! {} {}", 1337, L"hehe");
        logger::debug<2>(L"nested hello!");

        logger::info("hey! {:#x} {}", 0x1337, "hehe");
        logger::info(L"wide hey! {} {}", 1337, L"hehe");
        logger::info<2>(L"nested hello!");

        logger::warn("hey! {:#x} {}", 0x1337, "hehe");
        logger::warn(L"wide hey! {} {}", 1337, L"hehe");
        logger::warn<2>(L"nested hello!");

        logger::error("hey! {:#x} {}", 0x1337, "hehe");
        logger::error(L"wide hey! {} {}", 1337, L"hehe");
        logger::error<2>(L"nested hello!");

        logger::critical("hey! {:#x} {}", 0x1337, "hehe");
        logger::critical(L"wide hey! {} {}", 1337, L"hehe");
        logger::critical<2>(L"nested hello!");

        logger::msg("hey! {:#x} {}", 0x1337, "hehe");
        logger::msg(L"wide hey! {} {}", 1337, L"hehe");
        logger::msg<2>(L"nested hello!");

        logger::todo("hey! {:#x} {}", 0x1337, "hehe");
        logger::todo(L"wide hey! {} {}", 1337, L"hehe");
        logger::todo<2>(L"nested hello!");

        logger::fixme("hey! {:#x} {}", 0x1337, "hehe");
        logger::fixme(L"wide hey! {} {}", 1337, L"hehe");
        logger::fixme<2>(L"nested hello!");
    };

    /// Disable wcout/cout
    std::cout.setstate(std::ios::failbit);
    std::wcout.setstate(std::ios::failbit);

    /// Test our stuff in different configurations
    logger::enabled = true;
    logger::detail::show_timestamps = true;
    pass();
    logger::detail::show_timestamps = false;
    pass();

    /// Re-enable wcout/cout
    std::cout.clear();
    std::wcout.clear();
}
