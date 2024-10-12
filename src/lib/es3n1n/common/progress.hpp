#pragma once
#include "base.hpp"
#include "logger.hpp"
#include "stopwatch.hpp"

namespace progress {
    /// \brief Progress-bar object
    class Progress : public base::NonCopyable {
    public:
        /// \brief
        /// \param title Title like "Obfuscating function main"
        /// \param num_steps Number of total steps for a progress
        explicit Progress(const std::string_view title, const std::size_t num_steps): title_(title), steps_(num_steps) {
            step();
        }

        /// \brief Do a step and update the progress msg
        void step() {
            /// Increment the step
            ++step_;

            /// Calculate current percentage
            const auto percents = static_cast<float>(step_) / static_cast<float>(steps_);
            const auto percents_m = static_cast<std::size_t>(percents * 100);

            /// Format msg with elapsed time if needed
            if (step_ == steps_) {
                logger::info("{}: {}% took {}", title_, percents_m, stopwatch_.elapsed());
                return;
            }
            logger::info("{}: {}%", title_, percents_m);
        }

    private:
        stopwatch::Stopwatch stopwatch_ = {}; ///< Stopwatch for elapsed time
        std::string title_ = {}; ///< Title of the progress
        std::size_t steps_ = {}; ///< Total steps
        std::ptrdiff_t step_ = -1; ///< Current step. We start at -1, and it will automatically increment it to 0
    };
} // namespace progress
