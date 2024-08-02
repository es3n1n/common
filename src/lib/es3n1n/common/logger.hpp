#pragma once
#include <cstdint>

#include <format>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>

#include <chrono>
#include <iomanip>

#include "platform.hpp"

namespace logger {
    /// Switch this var to false if you want to ignore all output
    inline bool enabled = true;

    namespace detail {
        /// Config
        ///
        inline bool colors_enabled = true;
        inline bool show_timestamps = true;

        /// Constants
        ///
        constexpr std::size_t kMaxLevelNameSize = 8;
        constexpr std::size_t kIndentationSize = kMaxLevelNameSize; // in spaces

        /// A mutex that would be used to make sure that we are logging one line at the time
        inline std::mutex _mtx = {};

        /// Just a set of colors for the verbose prefix in console
        ///
        namespace colors {
            // clang-format off
            struct col_t { std::uint8_t fg, bg; };
            // clang-format on

            /// @credits: https://stackoverflow.com/a/54062826
            ///
            [[maybe_unused]] inline constexpr col_t NO_COLOR = {0, 0}; // no color

            [[maybe_unused]] inline constexpr col_t BLACK = {30, 40};
            [[maybe_unused]] inline constexpr col_t RED = {31, 41};
            [[maybe_unused]] inline constexpr col_t GREEN = {32, 42};
            [[maybe_unused]] inline constexpr col_t YELLOW = {33, 43};
            [[maybe_unused]] inline constexpr col_t BLUE = {34, 44};
            [[maybe_unused]] inline constexpr col_t MAGENTA = {35, 45};
            [[maybe_unused]] inline constexpr col_t CYAN = {36, 46};
            [[maybe_unused]] inline constexpr col_t WHITE = {37, 47};

            [[maybe_unused]] inline constexpr col_t BRIGHT_BLACK = {90, 100};
            [[maybe_unused]] inline constexpr col_t BRIGHT_RED = {91, 101};
            [[maybe_unused]] inline constexpr col_t BRIGHT_GREEN = {92, 102};
            [[maybe_unused]] inline constexpr col_t BRIGHT_YELLOW = {93, 103};
            [[maybe_unused]] inline constexpr col_t BRIGHT_BLUE = {94, 104};
            [[maybe_unused]] inline constexpr col_t BRIGHT_MAGENTA = {95, 105};
            [[maybe_unused]] inline constexpr col_t BRIGHT_CYAN = {96, 106};
            [[maybe_unused]] inline constexpr col_t BRIGHT_WHITE = {97, 107};
        } // namespace colors

        /// Util function to set console colors
        /// \todo: @es3n1n: maybe add font_style too?
        ///
        inline void apply_style(const std::uint8_t foreground, const std::uint8_t background, const std::function<void()>& callback) noexcept {
#if PLATFORM_IS_WIN
            static std::once_flag once_flag;

            /// In order to use ANSI escape sequences we should make sure that we've activated virtual
            /// terminal before using them
            ///
            std::call_once(once_flag, []() -> void {
                const auto console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

                if (console_handle == nullptr) {
                    colors_enabled = false;
                    return;
                }

                /// Obtaining current console mode
                ///
                DWORD console_flags = 0;
                if (GetConsoleMode(console_handle, &console_flags) == 0) [[unlikely]] {
                    /// wtf? ok lets pray ansi codes would work lol.
                    return;
                }

                /// Activating virtual terminal
                ///
                console_flags |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
                if (SetConsoleMode(console_handle, console_flags) == 0) {
                    console_flags &= ~DISABLE_NEWLINE_AUTO_RETURN;

                    /// Try again without DISABLE_NEWLINE_AUTO_RETURN
                    ///
                    if (SetConsoleMode(console_handle, console_flags) == 0) {
                        colors_enabled = false;
                    }
                }
            });
#endif

            /// Applying style
            ///
            const auto apply = [](const std::uint8_t fg_value, const std::uint8_t bg_value) -> int {
                if (!colors_enabled) {
                    return 0;
                }

                if ((fg_value != 0U) && (bg_value == 0U)) {
                    return std::printf("\033[%dm", fg_value);
                }

                return std::printf("\033[%d;%dm", fg_value, bg_value);
            };
            apply(foreground, background);

            /// Invoke callback
            ///
            callback();

            /// Reset style
            ///
            apply(colors::NO_COLOR.fg, colors::NO_COLOR.bg);
        }

        /// Util traits
        ///
        template <typename Ty>
        concept str_t = requires {
            typename Ty::traits_type;
            typename Ty::allocator_type;
            requires std::is_same_v<Ty, std::basic_string<typename Ty::value_type, typename Ty::traits_type, typename Ty::allocator_type>>;
        };
        template <typename T> concept char_str_view_t = std::is_same_v<T, std::string_view> || std::is_same_v<T, const char*>;
        template <typename T> concept wchar_str_view_t = std::is_same_v<T, std::wstring_view> || std::is_same_v<T, const wchar_t*>;
        template <typename T> concept str_view_t = char_str_view_t<T> || wchar_str_view_t<T>;

        /// The main method that would be used to log lines
        ///
        template <str_t Ty, bool IsWide = std::is_same_v<Ty, std::wstring>>
        void log_line(const std::uint8_t indentation, const std::string_view level_name, //
                      const std::uint8_t color_fg, const std::uint8_t color_bg, //
                      const Ty& msg) noexcept {
            /// If logger is disabled
            ///
            if (!enabled) {
                return;
            }

            /// Locking mutex
            ///
            const std::lock_guard _lock(_mtx);

            const auto indent = [](const std::uint8_t ind) -> void {
                if (ind <= 0) {
                    return;
                }

                for (std::size_t i = 0; i < ind; i++) {
                    std::cout << '|';

                    for (std::size_t j = 0; j < kIndentationSize; j++) {
                        std::cout << ' ';
                    }
                }
            };

            /// Iterating lines
            ///
            std::conditional_t<IsWide, std::wistringstream, std::istringstream> stream(msg);
            for (Ty line; std::getline(stream, line);) {
                if (show_timestamps) {
                    static auto zone = std::chrono::current_zone(); // surely it would not change
                    const auto now = std::chrono::zoned_time{zone, std::chrono::system_clock::now()};
                    const auto tm = now.get_local_time();

                    /// \note @es3n1n: There doesn't seem to be a way to set the number of digits for `%S%`
                    /// so a dirty workaround for this would be manually converting to seconds/milliseconds
                    /// \ref https://eel.is/c++draft/time.format#6
                    const auto now_sec = std::chrono::time_point_cast<std::chrono::seconds>(tm);
                    const auto now_msec = std::chrono::duration_cast<std::chrono::milliseconds>(tm - now_sec).count();

                    std::cout << std::format("{:%H:%M:%S}.{:0>3}", now_sec, now_msec) << " | ";
                }

                indent(indentation);

                std::cout << "[";
                apply_style(color_fg, color_bg, [=]() -> void { std::cout << std::format("{:^{}}", level_name, kMaxLevelNameSize); });
                std::cout << "] ";

                /// \fixme: @es3n1n: this is ugly
                ///
                if constexpr (IsWide) {
                    std::wcout << line << "\n";
                } else {
                    std::cout << line << "\n";
                }
            }
        }
    } // namespace detail

#define MAKE_LOGGER_METHOD(fn_name, prefix, color_fg, color_bg)                          \
    template <std::uint8_t Indentation = 0, typename... Args>                            \
    inline void fn_name(const std::format_string<Args...> fmt, Args... args) noexcept {  \
        auto msg = std::vformat(fmt.get(), std::make_format_args(args...));              \
        detail::log_line(Indentation, prefix, (color_fg).fg, (color_bg).bg, msg);        \
    }                                                                                    \
    template <std::uint8_t Indentation = 0, typename... Args>                            \
    inline void fn_name(const std::wformat_string<Args...> fmt, Args... args) noexcept { \
        auto msg = std::vformat(fmt.get(), std::make_wformat_args(args...));             \
        detail::log_line(Indentation, prefix, (color_fg).fg, (color_bg).bg, msg);        \
    }

    MAKE_LOGGER_METHOD(debug, "debug", detail::colors::BRIGHT_WHITE, detail::colors::NO_COLOR)
    MAKE_LOGGER_METHOD(info, "info", detail::colors::BRIGHT_GREEN, detail::colors::NO_COLOR)
    MAKE_LOGGER_METHOD(warn, "warn", detail::colors::BRIGHT_YELLOW, detail::colors::NO_COLOR)
    MAKE_LOGGER_METHOD(error, "error", detail::colors::BRIGHT_MAGENTA, detail::colors::NO_COLOR)
    MAKE_LOGGER_METHOD(critical, "critical", detail::colors::BRIGHT_WHITE, detail::colors::MAGENTA)

    MAKE_LOGGER_METHOD(msg, "msg", detail::colors::BRIGHT_WHITE, detail::colors::NO_COLOR)
    MAKE_LOGGER_METHOD(todo, "todo", detail::colors::BRIGHT_YELLOW, detail::colors::NO_COLOR)
    MAKE_LOGGER_METHOD(fixme, "fixme", detail::colors::BRIGHT_YELLOW, detail::colors::NO_COLOR)

#undef MAKE_LOGGER_METHOD

#define MAKE_LOGGER_OR_METHOD(fn_name, if_true, if_false)                                                      \
    template <std::uint8_t Indentation = 0, typename... Args>                                                  \
    inline void fn_name(const bool condition, const std::format_string<Args...> fmt, Args... args) noexcept {  \
        if (condition) {                                                                                       \
            if_true<Indentation>(fmt, args...);                                                                \
            return;                                                                                            \
        }                                                                                                      \
        if_false<Indentation>(fmt, args...);                                                                   \
    }                                                                                                          \
    template <std::uint8_t Indentation = 0, typename... Args>                                                  \
    inline void fn_name(const bool condition, const std::wformat_string<Args...> fmt, Args... args) noexcept { \
        if (condition) {                                                                                       \
            if_true<Indentation>(fmt, args...);                                                                \
            return;                                                                                            \
        }                                                                                                      \
        if_false<Indentation>(fmt, args...);                                                                   \
    }

    MAKE_LOGGER_OR_METHOD(info_or_warn, info, warn);
    MAKE_LOGGER_OR_METHOD(info_or_error, info, error);
    MAKE_LOGGER_OR_METHOD(info_or_critical, info, critical);

#undef MAKE_LOGGER_OR_METHOD
} // namespace logger

/// \note es3n1n: clang momento
#define FIXME_NO_ARG(indentation, fmt) logger::fixme<indentation>(fmt)
#define TODO_NO_ARG(indentation, fmt) logger::todo<indentation>(fmt)

#define FIXME(indentation, fmt, ...) logger::fixme<indentation>(fmt, __VA_ARGS__)
#define TODO(indentation, fmt, ...) logger::todo<indentation>(fmt, __VA_ARGS__)
