#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cmath>
#include <format>
#include <cstdint>

#include "files/header.h"

namespace {






}

// User receives a hex code string literal
int main(int argc, char* argv[]) {

    // argc is always 1 (the program itself) it can not be zero
    // returns 0 or 1; 0 means END OF FILE (EOF)
    if (argc <= 1) {
        std::cerr << "No file was provided!";
        return -1;
    }


    std::ifstream input{argv[1], std::ios::binary};
    if (!input) {
        std::cerr << "Unable to open the file!";
        return -1;
    }

    if (std::filesystem::file_size(argv[1])<336) {
        std::cerr << "File is smaller than 336 bytes!";
        return -1;
    }




    // Header Checksum
    char checksum {};
    input.seekg(333);
    input.read(&checksum, 1);

    // ROM size
    char rom_size_location {};
    unsigned int rom_size_kib{};
    input.seekg(328);
    input.read(&rom_size_location, 1);


    const unsigned int rom_code =
    static_cast<unsigned int>(
        static_cast<unsigned char>(rom_size_location));

    // 0xFF (HEX) -> 255 (DECIMAL)
    if (rom_code > 8) {
        std::cerr << "Invalid Rom Size!";
        return -1;
    }

    rom_size_kib=32*(std::pow(2, rom_code));


    // RAM size
    char ram_size_location {};
    input.seekg(329);
    input.read(&ram_size_location, 1);

    // Cartridge type
    char cartridge_type_location {};
    input.seekg(327);
    input.read(&cartridge_type_location, 1);

    // Title
    char title_location[16];
    input.seekg(308);
    input.read(&title_location[0], 16);

    std::string title{};
    for (std::size_t i{};i<16;i++) {

        if (title_location[i]=='\0') {
            break;
        }

        title.push_back(title_location[i]);
    }


    // Header Checksum Stored
    char stored_header_checksum_location{};
    input.seekg(333);
    input.read(&stored_header_checksum_location, 1);


    // Header Checksum Calculated (332 - 308)
    char stored_header_checksum_values_location[25];
    std::uint8_t checksum_calculated{};
    input.seekg(308);
    input.read(&stored_header_checksum_values_location[0], 25);
    for (std::size_t i{};i<25;i++) {
        checksum_calculated=checksum_calculated-static_cast<unsigned int>(static_cast<unsigned char>(stored_header_checksum_values_location[i]))-1;
    }

    std::cout << "File: " << argv[1] << std::endl;
    std::cout << "Size: " << std::filesystem::file_size(argv[1]) << " bytes" << std::endl;
    std::cout << "Title: " << title << std::endl;
    std::cout << "Cartridge type: " << std::format("{}", get_cartridge_type(static_cast<unsigned int>(static_cast<unsigned char>(cartridge_type_location)))) << std::endl;

    std::cout << "ROM size: " << rom_size_kib << " KiB"  << std::endl;
    std::cout << "RAM size: " << get_ram_size(static_cast<unsigned int>(static_cast<unsigned char>(ram_size_location))) << std::endl;

    std::cout << "Header checksum stored: " << std::format("0x{:X}", static_cast<std::size_t>(static_cast<unsigned char>(stored_header_checksum_location))) << std::endl;
    std::cout << "Header checksum calculated: " << std::format("0x{:X}", checksum_calculated) << std::endl;
    std::cout << "Header checksum: " << ((checksum_calculated==static_cast<std::size_t>(static_cast<unsigned char>(stored_header_checksum_location))) ? "PASS" : "FAILED") << std::endl;
    return 0;
}
