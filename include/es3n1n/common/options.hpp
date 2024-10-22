#pragma once

#if !defined(COMMON_PAGE_SIZE)
    #define COMMON_PAGE_SIZE 0x1000
#endif

#if __has_include(<imgui.h>)
    #define COMMON_HAS_IMGUI
#endif
