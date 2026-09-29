#pragma once
#include <array>
#include <cstdint>

class Ram{
public:
    void write(std::uint16_t address, std::uint8_t value);
    [[nodiscard]] std::uint8_t read(std::uint16_t address) const;
private:
    std::array<std::uint8_t, 8192> m_container{};
    // static: no this, so cannot access object members
    static std::size_t getIndex(std::uint16_t address) ;
};
