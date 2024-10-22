#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>

namespace files {
    /// \brief Calculates the size of a file.
    /// \param file A reference to an open input file stream.
    /// \return An optional containing the file size in bytes if successful,
    ///         or an empty optional if the file is not in a good state.
    inline std::optional<std::size_t> file_size(std::ifstream& file) {
        if (!file.good()) {
            return {};
        }

        file.seekg(0, std::ios::end);
        const auto file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        return static_cast<std::size_t>(file_size);
    }

    /// \brief Reads the contents of a file into a vector of bytes.
    /// \param path The filesystem path to the file to be read.
    /// \return A vector of unsigned 8-bit integers containing the file contents.
    ///         Returns std::nullopt if the file cannot be opened.
    inline std::optional<std::vector<std::uint8_t>> read_file(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.good()) {
            return {};
        }

        std::vector<std::uint8_t> buffer(file_size(file).value());
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
        return buffer;
    }

    /// \brief Writes a buffer of bytes to a file.
    /// \param path The filesystem path where the file should be written.
    /// \param raw_buffer Pointer to the buffer containing the data to be written.
    /// \param buffer_size The size of the buffer in bytes.
    inline void write_file(const std::filesystem::path& path, const std::uint8_t* raw_buffer, const std::size_t buffer_size) {
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(raw_buffer), static_cast<std::streamsize>(buffer_size));
    }
} // namespace files
