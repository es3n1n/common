#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>

namespace files {
    inline std::optional<std::size_t> file_size(std::ifstream& file) {
        if (!file.good()) {
            return {};
        }

        file.seekg(0, std::ios::end);
        const auto file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        return static_cast<std::size_t>(file_size);
    }

    inline std::optional<std::vector<std::uint8_t>> read_file(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.good()) {
            return {};
        }

        std::vector<std::uint8_t> buffer(file_size(file).value());
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
        return buffer;
    }

    inline void write_file(const std::filesystem::path& path, const std::uint8_t* raw_buffer, const std::size_t buffer_size) {
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(raw_buffer), static_cast<std::streamsize>(buffer_size));
    }
} // namespace files
