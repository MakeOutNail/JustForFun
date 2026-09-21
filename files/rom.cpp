#include "./rom.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>

Rom::Rom(const std::string_view file_path) {

    std::ifstream input{std::format("{}", file_path), std::ios::binary};

    if (!input) {
        throw std::runtime_error("Unable to open the file!");
    }


    const std::uintmax_t file_size = std::filesystem::file_size(std::format("{}", file_path));

    if (file_size<336) {
        throw std::runtime_error("File is smaller than 336 bytes!");
    }

    byte_collection.resize(std::filesystem::file_size(file_path));
    input.seekg(0);

    if (!input.read(&byte_collection[0], static_cast<long>(file_size))) {
        throw std::runtime_error(std::format("Couldn't read all the {} bytes!", file_size));
    }
}


size_t Rom::get_rom_byte_size() const {
    return byte_collection.size();
}

unsigned char Rom::get_byte(size_t offset) const{
    return static_cast<unsigned char>(byte_collection.at(offset));
}

