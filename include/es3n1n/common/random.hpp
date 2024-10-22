#pragma once
#include "logger.hpp"
#include "types.hpp"

#include <algorithm>
#include <climits>
#include <numeric>
#include <optional>
#include <random>

/// Intentionally named rnd and not random to avoid conflicts with the standard library (libc++, libstdc++)
namespace rnd {
    namespace detail {
        inline std::mt19937_64 prng;

        inline void seed(std::optional<std::uint64_t> seed = std::nullopt) {
            if (!seed.has_value()) {
                std::random_device device;
                seed = (static_cast<uint64_t>(device()) << (sizeof(uint32_t) * CHAR_BIT)) | device();
            }

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
            using UResultTy = std::make_unsigned_t<ResultTy>;

            explicit UniformIntDistribution(ResultTy min, ResultTy max = (std::numeric_limits<ResultTy>::max)()): min_(min), max_(max) { }

            template <typename Engine>
            ResultTy operator()(Engine& engine) {
                return eval(engine, min_, max_);
            }

        private:
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

    template <typename Ty = std::uint32_t, typename TyVal = std::remove_reference_t<Ty>, typename Limits = std::numeric_limits<TyVal>>
    [[nodiscard]] TyVal number(const TyVal min = Limits::min(), const TyVal max = Limits::max()) {
        static_assert(sizeof(Ty) <= sizeof(std::uint64_t));
        detail::UniformIntDistribution<TyVal> dist(min, max);
        return static_cast<TyVal>(dist(detail::prng));
    }

    inline void bytes(std::uint8_t* ptr, const std::size_t size) {
        std::ranges::generate_n(ptr, size, []() -> std::uint8_t { return number<std::uint8_t>(); });
    }

    [[nodiscard]] inline std::vector<std::uint8_t> bytes(const std::size_t size) {
        std::vector<std::uint8_t> result = {};
        result.resize(size);

        bytes(result.data(), result.size());
        return result;
    }

    [[nodiscard]] inline bool chance(const std::uint8_t chance) {
        return number<std::uint8_t>(0, 100) <= chance;
    }

    template <std::ranges::range Rng, typename RngValueT = std::ranges::range_value_t<Rng>>
    [[nodiscard]] const RngValueT& item(Rng&& range) {
        auto distance = std::ranges::distance(range);
        auto generated_number = rnd::number<>(std::size_t(0), static_cast<std::size_t>(distance - 1));
        auto it = std::ranges::next(std::ranges::begin(range), generated_number);
        return *it;
    }

    template <typename... TArgs>
    [[nodiscard]] auto or_(TArgs... args) {
        return item(types::to_array(std::forward<TArgs>(args)...));
    }
} // namespace rnd
