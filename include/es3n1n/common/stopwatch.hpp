#pragma once
#include "base.hpp"
#include <chrono>
#include <format>
#include <string>

namespace stopwatch {
    /// \brief Custom elapsed time storage class
    class ElapsedTime {
    public:
        template <class Rep, class Period>
        explicit ElapsedTime(const std::chrono::duration<Rep, Period>& elapsed) {
            total_microseconds_ = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
            hours_ = extract_component<std::chrono::hours>();
            minutes_ = extract_component<std::chrono::minutes>();
            seconds_ = extract_component<std::chrono::seconds>();
            milliseconds_ = extract_component<std::chrono::milliseconds>();
        }

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
        template <typename DurationType>
        [[nodiscard]] DurationType extract_component() noexcept {
            const auto component = std::chrono::duration_cast<DurationType>(total_microseconds_);
            total_microseconds_ -= std::chrono::duration_cast<decltype(total_microseconds_)>(component);
            return component;
        }

        template <typename Ty, typename Period>
        static void append_if_non_zero(std::string& result, const std::chrono::duration<Ty, Period>& duration, const char* unit) {
            if (const auto count = duration.count(); count > 0) {
                result += std::format(" {} {}", count, unit);
            }
        }

        std::chrono::hours hours_{};
        std::chrono::minutes minutes_{};
        std::chrono::seconds seconds_{};
        std::chrono::milliseconds milliseconds_{};
        std::chrono::microseconds total_microseconds_{};
    };

    /// \brief A high-resolution stopwatch class for time measurements
    class Stopwatch : public base::NonCopyable {
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;

    public:
        Stopwatch() noexcept {
            reset();
        }

        void reset() {
            started_ = Clock::now();
        }

        [[nodiscard]] ElapsedTime elapsed() const noexcept {
            return ElapsedTime(Clock::now() - started_);
        }

    private:
        TimePoint started_{};
    };
} // namespace stopwatch

template <>
struct std::formatter<stopwatch::ElapsedTime> : std::formatter<std::string> {
    template <class FormatContext>
    constexpr auto format(const stopwatch::ElapsedTime& instance, FormatContext& ctx) const {
        return std::formatter<std::string>::format(instance.str(), ctx);
    }
};
