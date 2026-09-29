#include "ram.h"


std::uint8_t Ram::read(std::uint16_t address) const {
    return m_container.at(getIndex(address));
}

void Ram::write(std::uint16_t address, std::uint8_t value) {
    m_container.at(getIndex(address)) = value;
}

std::size_t Ram::getIndex(std::uint16_t address) {
    // The Range is 0xC000-0xDFFF
    return address-0xC000;
}
