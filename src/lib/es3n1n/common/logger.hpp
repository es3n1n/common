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
    /// \brief Flag to enable or disable all logging output
    inline bool enabled = true;

    namespace detail {
        /// \brief Configuration options for the logger
        inline bool colors_enabled = true;
        inline bool show_timestamps = true;

        /// \brief Constants for formatting
        constexpr std::size_t kMaxLevelNameSize = 5;
        constexpr std::size_t kIndentationSize = kMaxLevelNameSize; // in spaces

        /// \brief Mutex to ensure thread-safe logging
        inline std::mutex _mtx = {};

        /// \brief Namespace containing color definitions for console output
        namespace colors {
            /// \brief Console color structure definition
            struct col_t {
                std::uint8_t fg, bg;
            };

            /// \credits https://stackoverflow.com/a/54062826
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

        /// \brief Apply color styling to console output
        /// \param foreground Foreground color code
        /// \param background Background color code
        /// \param callback Function to execute with applied styling
        inline void apply_style(const std::uint8_t foreground, const std::uint8_t background, const std::function<void()>& callback) noexcept {
            static std::once_flag once_flag;

            std::call_once(once_flag, []() -> void {
                /// Disabling stdout buffering
                std::ios::sync_with_stdio(false);
                std::setvbuf(stdout, nullptr, _IONBF, 0);

                /// In order to use ANSI escape sequences on windows, we should make sure that
                /// we've activated virtual terminal

#if PLATFORM_IS_WIN
                const auto console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

                if (console_handle == nullptr) {
                    colors_enabled = false;
                    return;
                }

                /// Obtaining current console mode
                ///
                DWORD console_flags = 0;
                if (GetConsoleMode(console_handle, &console_flags) == 0) [[unlikely]] {
                    /// huh? ok lets pray ansi codes would work lol.
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
#endif
            });

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

        /// \brief Concept to check if a type is a string type
        template <typename Ty>
        concept str_t = requires {
            typename Ty::traits_type;
            typename Ty::allocator_type;
            requires std::is_same_v<Ty, std::basic_string<typename Ty::value_type, typename Ty::traits_type, typename Ty::allocator_type>>;
        };

        /// \brief Main logging function to output formatted log messages
        /// \tparam Ty String type (deduced)
        /// \tparam IsWide Flag indicating if the string is wide (default: false)
        /// \param indentation Number of indentation levels
        /// \param level_name Name of the log level
        /// \param color_fg Foreground color for the log level
        /// \param color_bg Background color for the log level
        /// \param msg The message to log
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
#if __cpp_lib_chrono >= 201907L
                    static auto zone = std::chrono::current_zone(); // surely it would not change
                    const auto now = std::chrono::zoned_time{zone, std::chrono::system_clock::now()};
                    const auto tm = now.get_local_time();

                    /// \note @es3n1n: There doesn't seem to be a way to set the number of digits for `%S%`
                    /// so a dirty workaround for this would be manually converting to seconds/milliseconds
                    /// \ref https://eel.is/c++draft/time.format#6
                    const auto now_sec = std::chrono::time_point_cast<std::chrono::seconds>(tm);
                    const auto now_msec = std::chrono::duration_cast<std::chrono::milliseconds>(tm - now_sec).count();

                    std::cout << std::format("{:%H:%M:%S}.{:0>3}", now_sec, now_msec) << " | ";
#else // Backporting for clang until the first 19.x version gets released
                    const auto now = std::chrono::system_clock::now();
                    const std::time_t time_now = std::chrono::system_clock::to_time_t(now);
                    const std::tm now_tm = *std::localtime(&time_now);

                    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

                    std::cout << std::put_time(&now_tm, "%H:%M:%S.");
                    std::cout << std::setfill('0') << std::setw(3) << milliseconds.count() << " | ";
#endif
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
    MAKE_LOGGER_METHOD(critical, "crit", detail::colors::BRIGHT_WHITE, detail::colors::BRIGHT_RED)

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

    MAKE_LOGGER_OR_METHOD(info_or_warn, info, warn)
    MAKE_LOGGER_OR_METHOD(info_or_error, info, error)
    MAKE_LOGGER_OR_METHOD(info_or_critical, info, critical)

#undef MAKE_LOGGER_OR_METHOD
} // namespace logger

/// \brief Macro to log a FIXME message without arguments
/// \param indentation Indentation level
/// \param fmt Format string
#define FIXME_NO_ARG(indentation, fmt) logger::fixme<indentation>(fmt)

/// \brief Macro to log a TODO message without arguments
/// \param indentation Indentation level
/// \param fmt Format string
#define TODO_NO_ARG(indentation, fmt) logger::todo<indentation>(fmt)

/// \brief Macro to log a FIXME message with arguments
/// \param indentation Indentation level
/// \param fmt Format string
/// \param ... Variable arguments for formatting
#define FIXME(indentation, fmt, ...) logger::fixme<indentation>(fmt, __VA_ARGS__)

/// \brief Macro to log a TODO message with arguments
/// \param indentation Indentation level
/// \param fmt Format string
/// \param ... Variable arguments for formatting
#define TODO(indentation, fmt, ...) logger::todo<indentation>(fmt, __VA_ARGS__)
