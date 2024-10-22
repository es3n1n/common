#pragma once
#include "base.hpp"
#include "logger.hpp"
#include "stopwatch.hpp"

namespace progress {
    /// \brief Progress-bar object
    class Progress : public base::NonCopyable {
    public:
        explicit Progress(const std::string_view title, const std::size_t num_steps): title_(title), steps_(num_steps) {
            step();
        }

        void step() {
            ++step_;

            const auto percents = static_cast<float>(step_) / static_cast<float>(steps_);
            const auto percents_m = static_cast<std::size_t>(percents * 100);

            if (step_ == steps_) {
                logger::info("{}: {}% took {}", title_, percents_m, stopwatch_.elapsed());
                return;
            }
            logger::info("{}: {}%", title_, percents_m);
        }

    private:
        stopwatch::Stopwatch stopwatch_ = {};
        std::string title_ = {};
        std::size_t steps_ = {};
        std::ptrdiff_t step_ = -1;
    };
} // namespace progress
