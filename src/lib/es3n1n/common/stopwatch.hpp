#pragma once
#include "base.hpp"
#include <chrono>

namespace stopwatch {
    /// \brief Our custom elapsed time storage
    class ElapsedTime {
    public:
        /// \brief Explicit conversion from chrono duration to our own stuff
        /// \tparam Rep Duration rep
        /// \tparam Period Duration period
        /// \param elapsed Duration value
        template <class Rep, class Period>
        explicit ElapsedTime(const std::chrono::duration<Rep, Period> elapsed) {
            microseconds_ = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
            hours_ = remainder_microseconds_cast<decltype(hours_)>();
            minutes_ = remainder_microseconds_cast<decltype(minutes_)>();
            seconds_ = remainder_microseconds_cast<decltype(seconds_)>();
            milliseconds_ = remainder_microseconds_cast<decltype(milliseconds_)>();
        }

        /// \brief Convert elapsed time to string in format "{} hr {} min {} sec {} ms"
        /// \return String
        [[nodiscard]] std::string str() const {
            std::string result = {};
            /// Convert hours
            if (auto hr = hours_.count()) {
                result += std::format(" {} hr", hr);
            }

            /// Convert minutes
            if (auto min = minutes_.count()) {
                result += std::format(" {} min", min);
            }

            /// Convert seconds
            if (auto sec = seconds_.count()) {
                result += std::format(" {} sec", sec);
            }

            /// Convert milliseconds
            if (auto ms = milliseconds_.count()) {
                result += std::format(" {} ms", ms);
            }

            /// Convert microseconds only if others are empty
            if (result.empty()) {
                result += std::format(" {} microseconds", microseconds_.count());
            }

            /// Remove the leading space
            result = result.substr(1);

            return result;
        }

    private:
        /// \brief Get remainder from total microseconds for a given duration
        /// \tparam Dst Destination duration type
        /// \return Converted amount
        template <typename Dst>
        [[nodiscard]] Dst remainder_microseconds_cast() noexcept {
            const auto result = std::chrono::duration_cast<Dst>(microseconds_);
            microseconds_ -= std::chrono::duration_cast<decltype(microseconds_)>(result);
            return result;
        }

        /// \brief Elapsed hours
        std::chrono::hours hours_ = {};
        /// \brief Elapsed minutes
        std::chrono::minutes minutes_ = {};
        /// \brief Elapsed seconds
        std::chrono::seconds seconds_ = {};
        /// \brief Elapsed milliseconds
        std::chrono::milliseconds milliseconds_ = {};
        /// \brief Elapsed **total** microseconds
        std::chrono::microseconds microseconds_ = {};
    };

    /// \brief A stopwatch class that should be used for all the time elapsing stuff
    class Stopwatch : public base::NonCopyable {
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;

    public:
        /// \brief General constructor
        Stopwatch() noexcept {
            reset();
        }

        /// \brief Start time reset
        void reset() {
            started_ = Clock::now();
        }

        /// \brief Get the difference between current and start time
        /// \return ElapsedTime struct
        [[nodiscard]] auto elapsed() const noexcept {
            return ElapsedTime(Clock::now() - started_);
        }

    private:
        /// \brief Start time
        TimePoint started_ = {};
    };
} // namespace stopwatch

/// \brief Elapsed time formatter
template <>
struct std::formatter<stopwatch::ElapsedTime> : std::formatter<std::string> {
    template <class FormatContextTy>
    constexpr auto format(const stopwatch::ElapsedTime& instance, FormatContextTy& ctx) const {
        return std::formatter<std::string>::format(instance.str(), ctx);
    }
};
