#pragma once

#define COMMON_CAT_(x, y) x##y
#define COMMON_CAT(x, y) COMMON_CAT_(x, y)

/// \note @es3n1n: I don't feel like including the whole platform.hpp for a single check.
#if defined(_MSC_VER)
    #define COMMON_PACKED(d) __pragma(pack(push, 1)) d __pragma(pack(pop))
    #define COMMON_PACKED_START __pragma(pack(push, 1))
    #define COMMON_PACKED_END __pragma(pack(pop))
#else
    #define COMMON_PACKED(d) d __attribute__((packed))
    #define COMMON_PACKED_START _Pragma("pack(push, 1)")
    #define COMMON_PACKED_END _Pragma("pack(pop)")
#endif
