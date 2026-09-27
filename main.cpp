#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <exception>
#include "files/header.h"
#include "files/rom.h"
#include <charconv>
#include <string_view>
#include <stdexcept>

#include "files/cpu.h"
#include "files/cpustate.h"

// User receives a hex code string literal
int main(int argc, char* argv[]) {

    try {

        // argc is always 1 (the program itself) it can not be zero
        // returns 0 or 1; 0 means END OF FILE (EOF)
        if (argc <= 1) {
            std::cerr << "No file was provided!";
            return -1;
        }

        if (std::string_view(argv[1])=="--cpu-state") {
            // Representative DMG state after boot with a nonzero cartridge header checksum.
            CpuState cpu_state{0x01, 0x00, 0x13, 0x00, 0xD8, 0x01, 0x4D, 0xFFFE, 0x0100, 0xB0};

            std::cout << std::format("A=0x{:02X} F=0x{:02X} AF=0x{:04X}", cpu_state.get_a(), cpu_state.get_f(), cpu_state.get_af()) << std::endl;
            std::cout << std::format("B=0x{:02X} C=0x{:02X} BC=0x{:04X}", cpu_state.get_b(), cpu_state.get_c(), cpu_state.get_bc()) << std::endl;
            std::cout << std::format("D=0x{:02X} E=0x{:02X} DE=0x{:04X}", cpu_state.get_d(), cpu_state.get_e(), cpu_state.get_de()) << std::endl;
            std::cout << std::format("H=0x{:02X} L=0x{:02X} HL=0x{:04X}", cpu_state.get_h(), cpu_state.get_l(), cpu_state.get_hl()) << std::endl;
            std::cout << std::format("PC=0x{:04X} SP=0x{:04X}", cpu_state.get_pc(), cpu_state.get_sp()) << std::endl;
            std::cout << std::format("Z={} N={} H={} C={}",
                static_cast<int>(cpu_state.get_flag(FlagType::Z)),
                static_cast<int>(cpu_state.get_flag(FlagType::N)),
                static_cast<int>(cpu_state.get_flag(FlagType::H)),
                static_cast<int>(cpu_state.get_flag(FlagType::C))) << std::endl;

            return 0;
        }
        
        if (argc > 3) {
            throw std::out_of_range(std::format("Invalid Argument: {}", argv[3]));
        }

        Rom rom {argv[1]};
        Cpu cpu{rom, CpuState{0x01, 0x00, 0x13, 0x00, 0xD8, 0x01, 0x4D, 0xFFFE, 0x0100, 0xB0}};
        cpu.step();


        // Byte-Reader-Mode
        // Example: ./ProjectX startup_time_test.gb 0x0100

        if (argc == 3) {
            std::string_view rom_address_to_read = argv[2];


            // Check the format
            if (rom_address_to_read.substr(0, 2) != "0x" ||
                rom_address_to_read.substr(2).length() > 4 || rom_address_to_read.substr(2).empty()) {
                throw std::invalid_argument{"Invalid ROM address format!"};
            }

            std::size_t hex_value{};
            auto result = std::from_chars(rom_address_to_read.substr(2).begin(), rom_address_to_read.substr(2).end(), hex_value, 16);

            if (result.ec != std::errc{} || result.ptr != rom_address_to_read.substr(2).end()) {
                throw std::invalid_argument("The conversion to a hex value went unsuccessful!");
            }


            // Check the range (the last byte is -1 byte...it is a indices)
            // 0x7FFF = 32767 is the valid game boy region
            if (hex_value >= rom.get_rom_byte_size() || hex_value>=32768) {
                throw std::out_of_range{"ROM address is outside the valid range!"};
            }


            std::cout << std::format("ROM[0x{:04X}] = 0x{:02X}", hex_value, rom.get_byte(hex_value));


            std::cout << std::endl;

        }


        // ---- ROM SIZE ----

        const unsigned int rom_code = rom.get_byte(328); // Implicit Conversion

        // 0xFF (HEX) -> 255 (DECIMAL)
        if (rom_code > 8) {
            std::cerr << "Invalid Rom Size!";
            return -1;
        }

        // 32 × 2^rom_code
        unsigned int rom_size_kib = 32u << rom_code;


        // ---- RAM size ----
        unsigned char ram_code = rom.get_byte(329);

        // ---- CARTRIDGE TYPE ----
        unsigned char cartridge_code = rom.get_byte(327);


        // --- TITLE ---
        std::string title{};
        for (std::size_t i{308};i<324;i++) {
            if (rom.get_byte(i)=='\0') {
                break;
            }

            title.push_back(static_cast<char>(rom.get_byte(i)));
        }


        // ---- HEADER CHECKSUM STORED ----
        unsigned char stored_header_checksum_location = rom.get_byte(333);

        // ---- HEADER CHECKSUM CALCULATED ----
        std::uint8_t checksum_calculated{};
        for (std::size_t i{308};i<333;i++) {
            checksum_calculated=checksum_calculated-static_cast<unsigned int>(rom.get_byte(i))-1;
        }

        std::cout << "File: " << argv[1] << std::endl;
        std::cout << "Size: " << std::filesystem::file_size(argv[1]) << " bytes" << std::endl;
        std::cout << "Title: " << title << std::endl;
        std::cout << "Cartridge type: " << std::format("{}", get_cartridge_type(cartridge_code)) << std::endl;

        std::cout << "ROM size: " << rom_size_kib << " KiB"  << std::endl;
        std::cout << "RAM size: " << get_ram_size(ram_code) << std::endl;

        std::cout << "Header checksum stored: " << std::format("0x{:X}", static_cast<std::size_t>(stored_header_checksum_location)) << std::endl;
        std::cout << "Header checksum calculated: " << std::format("0x{:X}", checksum_calculated) << std::endl;
        std::cout << "Header checksum: " << (checksum_calculated==static_cast<std::size_t>(stored_header_checksum_location) ? "PASS" : "FAILED") << std::endl;
        return 0;

    }
    catch (const std::exception& exception){
        std::cerr << exception.what() << '\n';
        return -1;
    }

}
