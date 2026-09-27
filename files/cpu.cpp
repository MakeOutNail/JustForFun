#include "cpu.h"

#include <format>
#include <iostream>
#include <stdexcept>

Cpu::Cpu(const Rom &rom, CpuState cpuState) : m_rom(rom), m_cpuState(cpuState) {

}

CpuState& Cpu::get_CpuState() {
    return m_cpuState;
}

void Cpu::step() {



    unsigned char byte = m_rom.get_byte(m_cpuState.get_pc());



    // Checks opcode (specific operation)
    switch (static_cast<unsigned int>(byte)) {
        // 0x00 means NOP (do nothing; increment pc)
        case 0:
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;

        // JP a16: jump to the 16-bit address encoded after the opcode.
        // First operand byte is low; second operand byte is high (little-endian).
        case 195:
            m_cpuState.set_pc((m_rom.get_byte(m_cpuState.get_pc()+2)<<8)|m_rom.get_byte(m_cpuState.get_pc()+1));
            break;
        // 0x06 - LD B, n8 (load an 8-bit value into register B)
        case 6:
            m_cpuState.set_b(m_rom.get_byte(m_cpuState.get_pc()+1));
            m_cpuState.set_pc(m_cpuState.get_pc()+2);
            break;
        default:
            throw std::runtime_error(std::format("Unsupported opcode 0x{:02X} at address 0x{:04X}", byte, m_cpuState.get_pc()));


    }

}
