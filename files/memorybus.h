#pragma once
#include "ram.h"
#include "rom.h"

class MemoryBus {
public:
    MemoryBus(const Rom& rom, Ram& ram);
    [[nodiscard]] std::uint8_t read(std::uint16_t address) const;
    void write(std::uint16_t address, std::uint8_t);
private:
    const Rom& m_rom;
    Ram& m_ram;
};
