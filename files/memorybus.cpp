#include "memorybus.h"

#include <format>
#include <stdexcept>

MemoryBus::MemoryBus(const Rom &rom, Ram &ram) : m_rom(rom), m_ram(ram) {
}

std::uint8_t MemoryBus::read(std::uint16_t address) const {

    // Rom
    if (address <= 0x7FFF) {
        return static_cast<unsigned int>(m_rom.get_byte(address));
    }
    // Ram
    if (0xC000 <= address && address <= 0xDFFF) {
        return m_ram.read(address);
    }

    throw std::runtime_error{std::format("Memory Bus doesn't recognize address: 0x{:04X}", address)};


}

void MemoryBus::write(std::uint16_t address, std::uint8_t value) {


    // Ram
    if (0xC000 <= address && address <= 0xDFFF) {
        return m_ram.write(address, value);
    }

    throw std::runtime_error{std::format("Memory Bus doesn't recognize address: 0x{:04X}", address)};


}

