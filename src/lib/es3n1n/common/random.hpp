#pragma once
#include "logger.hpp"
#include "types.hpp"

#include <algorithm>
#include <numeric>
#include <optional>
#include <random>

namespace rnd {
    namespace detail {
        /// We are gonna use the mersenne twister prng because its pretty convenient
        /// and its already present in std
        inline std::mt19937_64 prng;

        /// \brief Set the MT seed
        /// \param seed seed to set
        inline void seed(std::optional<std::uint64_t> seed = std::nullopt) {
            /// Generate the random seed, if needed
            if (!seed.has_value()) {
                std::random_device device;
                seed = (static_cast<uint64_t>(device()) << (sizeof(uint32_t) * CHAR_BIT)) | device();
            }

            /// Set the seed
            logger::info("random: seed is {:#x}", *seed);
            prng.seed(*seed);
        }

        /// \brief A drop-in replacement for std::uniform_int_distribution.
        ///
        /// We are using a replacement because the uniform_int_distribution implementation is different in libc and libstdc++.
        /// This is important because otherwise we can't reproduce random values across multiple platforms.
        ///
        /// \tparam IntType The integer type to be used for the distribution. Defaults to int.
        template <typename IntType = int>
        class UniformIntDistribution {
        public:
            using ResultTy = IntType;

            explicit UniformIntDistribution(ResultTy min, ResultTy max = (std::numeric_limits<ResultTy>::max)()): min_(min), max_(max) { }

            /// \brief Generates a random integer within the distribution range.
            /// \engine The random engine instance to use.
            template <typename Engine>
            ResultTy operator()(Engine& engine) {
                return eval(engine, min_, max_);
            }

        private:
            using UResultTy = std::make_unsigned_t<ResultTy>;

            /// \brief Evaluates the distribution and generates a random integer.
            /// \param engine The random engine instance to use.
            /// \param min The minimum value of the distribution (inclusive).
            /// \param max The maximum value of the distribution (inclusive).
            /// \return A random integer within the specified range.
            template <typename Engine>
            ResultTy eval(Engine& engine, ResultTy min, ResultTy max) const {
                const auto u_min = static_cast<UResultTy>(min);
                const auto u_max = static_cast<UResultTy>(max);

                const UResultTy range = u_max - u_min + 1;

                const auto r_raw = engine();
                if (u_max - u_min == static_cast<UResultTy>(-1)) {
                    return static_cast<ResultTy>(r_raw);
                }

                // todo @es3n1n: https://arxiv.org/pdf/1805.10941
                return static_cast<ResultTy>(r_raw % range + u_min);
            }

            ResultTy min_;
            ResultTy max_;
        };
    } // namespace detail

    /// \brief Get random number in desired range
    /// \tparam Ty result type
    /// \param min minimal value, by default set to the min limit of the `Ty` type
    /// \param max maximal value, by default set to the max limit of the `Ty` type
    /// \return random number
    template <typename Ty = std::uint32_t, typename TyVal = std::remove_reference_t<Ty>, typename Limits = std::numeric_limits<TyVal>>
    [[nodiscard]] TyVal number(const TyVal min = Limits::min(), const TyVal max = Limits::max()) {
        static_assert(sizeof(Ty) <= sizeof(std::uint64_t));
        detail::UniformIntDistribution<TyVal> dist(min, max);
        return static_cast<TyVal>(dist(detail::prng));
    }

    /// \brief Generate a number of bytes
    /// \param ptr pointer where it should write these bytes to
    /// \param size size
    inline void bytes(std::uint8_t* ptr, const std::size_t size) {
        std::generate_n(ptr, size, []() -> std::uint8_t { return number<std::uint8_t>(); });
    }

    /// \brief Generate a number of bytes and return them as a vector
    /// \param size size
    /// \return vector filled with random bytes
    [[nodiscard]] inline std::vector<std::uint8_t> bytes(const std::size_t size) {
        std::vector<std::uint8_t> result = {};
        result.resize(size);

        bytes(result.data(), result.size());
        return result;
    }

    /// \brief
    /// \param chance (from 0 to 100)% chance
    /// \return true/false
    [[nodiscard]] inline bool chance(const std::uint8_t chance) {
        return number<std::uint8_t>(0, 100) <= chance;
    }

    /// \brief Get a random item from the container
    /// \tparam Rng Range
    /// \param range Range value (vector/array/anything)
    /// \return random value reference
    template <std::ranges::range Rng>
    [[nodiscard]] const std::ranges::range_value_t<Rng>& item(Rng&& range) {
        auto it = std::ranges::begin(range);
        std::advance(it, number<std::size_t>(static_cast<std::size_t>(0), //
                                             static_cast<std::size_t>(std::ranges::distance(range) - 1)));
        return *it;
    }

    /// \brief Select between values
    /// \tparam TArgs typename of the operands
    /// \param args variadic options
    /// \return random choosen result
    template <typename... TArgs>
    [[nodiscard]] auto or_(TArgs... args) {
        return item(types::to_array(std::forward<TArgs>(args)...));
    }
} // namespace rnd
