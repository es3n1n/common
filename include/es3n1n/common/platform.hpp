#pragma once
#include <cstddef>
#include <limits>

/// \name System Detection
/// \{
#if defined(_WIN64)
    #define PLATFORM_IS_WIN true
    #define PLATFORM_IS_WIN32 false
    #define PLATFORM_IS_WIN64 true
    #define PLATFORM_IS_LINUX false
    #define PLATFORM_IS_APPLE false
    #define PLATFORM_IS_UNIX false
    #define PLATFORM_IS_POSIX false
#elif defined(_WIN32)
    #define PLATFORM_IS_WIN true
    #define PLATFORM_IS_WIN32 true
    #define PLATFORM_IS_WIN64 false
    #define PLATFORM_IS_LINUX false
    #define PLATFORM_IS_APPLE false
    #define PLATFORM_IS_UNIX false
    #define PLATFORM_IS_POSIX false
#elif defined(__linux__)
    #define PLATFORM_IS_WIN false
    #define PLATFORM_IS_WIN32 false
    #define PLATFORM_IS_WIN64 false
    #define PLATFORM_IS_LINUX true
    #define PLATFORM_IS_APPLE false
    #define PLATFORM_IS_UNIX true
    #define PLATFORM_IS_POSIX false
#elif defined(__APPLE__)
    #define PLATFORM_IS_WIN false
    #define PLATFORM_IS_WIN32 false
    #define PLATFORM_IS_WIN64 false
    #define PLATFORM_IS_LINUX false
    #define PLATFORM_IS_APPLE true
    #define PLATFORM_IS_UNIX true
    #define PLATFORM_IS_POSIX false
#elif defined(__unix__)
    #define PLATFORM_IS_WIN false
    #define PLATFORM_IS_WIN32 false
    #define PLATFORM_IS_WIN64 false
    #define PLATFORM_IS_LINUX false
    #define PLATFORM_IS_APPLE false
    #define PLATFORM_IS_UNIX true
    #define PLATFORM_IS_POSIX false
#elif defined(_POSIX_VERSION)
    #define PLATFORM_IS_WIN false
    #define PLATFORM_IS_WIN32 false
    #define PLATFORM_IS_WIN64 false
    #define PLATFORM_IS_LINUX false
    #define PLATFORM_IS_APPLE false
    #define PLATFORM_IS_UNIX false
    #define PLATFORM_IS_POSIX true
#else
    #error UNKNOWN SYSTEM
#endif
/// \}

/// \name Compiler Detection
/// \{
#if defined(__GNUC__)
    #define PLATFORM_IS_GCC true
    #define PLATFORM_IS_CLANG false
    #define PLATFORM_IS_MSVC false
#elif defined(__clang__)
    #define PLATFORM_IS_GCC false
    #define PLATFORM_IS_CLANG true
    #define PLATFORM_IS_MSVC false
#elif defined(_MSC_VER)
    #define PLATFORM_IS_GCC false
    #define PLATFORM_IS_CLANG false
    #define PLATFORM_IS_MSVC true
#else
    #error UNKNOWN_COMPILER
#endif
/// \}

namespace platform {
    [[maybe_unused]] constexpr size_t bitness = std::numeric_limits<size_t>::digits;
    [[maybe_unused]] constexpr bool is_x64 = bitness == 64;
    [[maybe_unused]] constexpr bool is_x32 = bitness == 32;

    [[maybe_unused]] constexpr bool is_win = PLATFORM_IS_WIN;
    [[maybe_unused]] constexpr bool is_win32 = PLATFORM_IS_WIN32;
    [[maybe_unused]] constexpr bool is_win64 = PLATFORM_IS_WIN64;

    [[maybe_unused]] constexpr bool is_linux = PLATFORM_IS_LINUX;
    [[maybe_unused]] constexpr bool is_apple = PLATFORM_IS_APPLE;
    [[maybe_unused]] constexpr bool is_unix = PLATFORM_IS_UNIX;
    [[maybe_unused]] constexpr bool is_posix = PLATFORM_IS_POSIX;

    [[maybe_unused]] constexpr bool is_gcc = PLATFORM_IS_GCC;
    [[maybe_unused]] constexpr bool is_clang = PLATFORM_IS_CLANG;
    [[maybe_unused]] constexpr bool is_msvc = PLATFORM_IS_MSVC;
} // namespace platform

#ifndef COMMON_NO_COMMON_INCLUDES
    #if PLATFORM_IS_WIN
        #include <Windows.h>
    #endif
#endif

#if PLATFORM_IS_MSVC
    #define COMMON_FORCE_INLINE __forceinline
#else
    #define COMMON_FORCE_INLINE inline __attribute__((always_inline))
#endif
