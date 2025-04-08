#include <benchmark/benchmark.h>
#include <es3n1n/common/string_parser.hpp>

namespace {
    void bm_parse_int64(benchmark::State& state) {
        const std::string test_input = "-12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_int64(test_input);
        }
    }
    BENCHMARK(bm_parse_int64);

    void bm_parse_uint64(benchmark::State& state) {
        const std::string test_input = "12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_uint64(test_input);
        }
    }
    BENCHMARK(bm_parse_uint64);

    void bm_parse_int32(benchmark::State& state) {
        const std::string test_input = "-12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_int32(test_input);
        }
    }
    BENCHMARK(bm_parse_int32);

    void bm_parse_uint32(benchmark::State& state) {
        const std::string test_input = "12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_uint32(test_input);
        }
    }
    BENCHMARK(bm_parse_uint32);

    void bm_parse_int8(benchmark::State& state) {
        const std::string test_input = "123";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_int8(test_input);
        }
    }
    BENCHMARK(bm_parse_int8);

    void bm_parse_uint8(benchmark::State& state) {
        const std::string test_input = "123";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_uint8(test_input);
        }
    }
    BENCHMARK(bm_parse_uint8);

    //

    void bm_parse_int64_hex(benchmark::State& state) {
        const std::string test_input = "-0x12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_int64(test_input, 16);
        }
    }
    BENCHMARK(bm_parse_int64_hex);

    void bm_parse_uint64_hex(benchmark::State& state) {
        const std::string test_input = "0x12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_uint64(test_input, 16);
        }
    }
    BENCHMARK(bm_parse_uint64_hex);

    void bm_parse_int32_hex(benchmark::State& state) {
        const std::string test_input = "-0x12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_int32(test_input, 16);
        }
    }
    BENCHMARK(bm_parse_int32_hex);

    void bm_parse_uint32_hex(benchmark::State& state) {
        const std::string test_input = "0x12345678";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_uint32(test_input, 16);
        }
    }
    BENCHMARK(bm_parse_uint32_hex);

    void bm_parse_int8_hex(benchmark::State& state) {
        const std::string test_input = "0x12";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_int8(test_input, 16);
        }
    }
    BENCHMARK(bm_parse_int8_hex);

    void bm_parse_uint8_hex(benchmark::State& state) {
        const std::string test_input = "0x12";

        for (auto _ : state) {
            volatile auto result = string_parser::parse_uint8(test_input, 16);
        }
    }
    BENCHMARK(bm_parse_uint8_hex);
} // namespace
