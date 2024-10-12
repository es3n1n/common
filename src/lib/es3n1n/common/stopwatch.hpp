#pragma once
#include "base.hpp"
#include <chrono>
#include <format>
#include <string>

namespace stopwatch {
    /// \brief Custom elapsed time storage class
    class ElapsedTime {
    public:
        /// \brief Construct ElapsedTime from a chrono duration
        /// \tparam Rep The arithmetic type representing the number of ticks
        /// \tparam Period A std::ratio representing the tick period
        /// \param elapsed The duration to convert
        template <class Rep, class Period>
        explicit ElapsedTime(const std::chrono::duration<Rep, Period>& elapsed) {
            total_microseconds_ = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
            hours_ = extract_component<std::chrono::hours>();
            minutes_ = extract_component<std::chrono::minutes>();
            seconds_ = extract_component<std::chrono::seconds>();
            milliseconds_ = extract_component<std::chrono::milliseconds>();
        }

        /// \brief Convert elapsed time to a formatted string
        /// \return A string representation in the format "X hr Y min Z sec W ms" or "V microseconds"
        [[nodiscard]] std::string str() const {
            std::string result;
            append_if_non_zero(result, hours_, "hr");
            append_if_non_zero(result, minutes_, "min");
            append_if_non_zero(result, seconds_, "sec");
            append_if_non_zero(result, milliseconds_, "ms");

            if (result.empty()) {
                return std::format("{} microseconds", total_microseconds_.count());
            }

            result = result.substr(1); // Remove leading space
            return result;
        }

    private:
        /// \brief Extract a time component from the total microseconds
        /// \tparam DurationType The type of duration to extract
        /// \return The extracted duration component
        template <typename DurationType>
        [[nodiscard]] DurationType extract_component() noexcept {
            const auto component = std::chrono::duration_cast<DurationType>(total_microseconds_);
            total_microseconds_ -= std::chrono::duration_cast<decltype(total_microseconds_)>(component);
            return component;
        }

        /// \brief Append a non-zero time component to the result string
        /// \tparam Ty The arithmetic type representing the number of ticks
        /// \tparam Period A std::ratio representing the tick period of the duration
        /// \param result The string to append to
        /// \param duration The std::chrono::duration to check and potentially append
        /// \param unit The string representation of the time unit (e.g., "hr", "min", "sec")
        template <typename Ty, typename Period>
        static void append_if_non_zero(std::string& result, const std::chrono::duration<Ty, Period>& duration, const char* unit) {
            if (const auto count = duration.count(); count > 0) {
                result += std::format(" {} {}", count, unit);
            }
        }

        std::chrono::hours hours_{}; ///< Elapsed hours
        std::chrono::minutes minutes_{}; ///< Elapsed minutes
        std::chrono::seconds seconds_{}; ///< Elapsed seconds
        std::chrono::milliseconds milliseconds_{}; ///< Elapsed milliseconds
        std::chrono::microseconds total_microseconds_{}; ///< Total elapsed microseconds
    };

    /// \brief A high-resolution stopwatch class for time measurements
    class Stopwatch : public base::NonCopyable {
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;

    public:
        /// \brief Default constructor
        /// \note Automatically calls reset() to set the start time
        Stopwatch() noexcept {
            reset();
        }

        /// \brief Reset the stopwatch to the current time
        void reset() {
            started_ = Clock::now();
        }

        /// \brief Get the elapsed time since the last reset
        /// \return An ElapsedTime object representing the time difference
        [[nodiscard]] ElapsedTime elapsed() const noexcept {
            return ElapsedTime(Clock::now() - started_);
        }

    private:
        TimePoint started_{}; ///< The start time point
    };

} // namespace stopwatch

/// \brief Custom formatter for ElapsedTime to allow use with std::format
template <>
struct std::formatter<stopwatch::ElapsedTime> : std::formatter<std::string> {
    /// \brief Format the ElapsedTime object
    /// \tparam FormatContext The type of the format context
    /// \param instance The ElapsedTime instance to format
    /// \param ctx The format context
    /// \return An iterator past the last character written
    template <class FormatContext>
    constexpr auto format(const stopwatch::ElapsedTime& instance, FormatContext& ctx) const {
        return std::formatter<std::string>::format(instance.str(), ctx);
    }
};
