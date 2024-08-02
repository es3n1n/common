#include <es3n1n/common/logger.hpp>
#include <es3n1n/common/memory/address.hpp>
#include <gtest/gtest.h>

TEST(logger, basics) {
    /// Just test the abi that everything is still there

    auto pass = []() -> void {
        logger::debug("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::debug(L"wide hey! {} {}", 1337, L"hehe");
        logger::debug<2>(L"nested hello!");

        logger::info("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::info(L"wide hey! {} {}", 1337, L"hehe");
        logger::info<2>(L"nested hello!");

        logger::warn("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::warn(L"wide hey! {} {}", 1337, L"hehe");
        logger::warn<2>(L"nested hello!");

        logger::error("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::error(L"wide hey! {} {}", 1337, L"hehe");
        logger::error<2>(L"nested hello!");

        logger::critical("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::critical(L"wide hey! {} {}", 1337, L"hehe");
        logger::critical<2>(L"nested hello!");

        logger::msg("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::msg(L"wide hey! {} {}", 1337, L"hehe");
        logger::msg<2>(L"nested hello!");

        logger::todo("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::todo(L"wide hey! {} {}", 1337, L"hehe");
        logger::todo<2>(L"nested hello!");

        logger::fixme("hey! {:#x} {}", memory::address(0x1337), "hehe");
        logger::fixme(L"wide hey! {} {}", 1337, L"hehe");
        logger::fixme<2>(L"nested hello!");

        logger::info_or_warn(true, "Info or warn - {}", "info");
        logger::info_or_warn(false, "Info or warn - {}", "warn");

        logger::info_or_error(true, "Info or error - {}", "info");
        logger::info_or_error(false, "Info or error - {}", "error");

        logger::info_or_critical(true, "Info or critical - {}", "info");
        logger::info_or_critical(false, "Info or critical - {}", "critical");

        FIXME_NO_ARG(0, "fixme no arguments");
        TODO_NO_ARG(0, "todo no arguments");
        FIXME(0, "fixme with {}", "an argument");
        TODO(0, "todo with {}", "an argument");
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
