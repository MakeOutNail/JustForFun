#include "cpu.h"

#include <format>
#include <stdexcept>

Cpu::Cpu(MemoryBus memoryBus, CpuState cpuState) : m_memoryBus(memoryBus), m_cpuState(cpuState) {

}

CpuState& Cpu::get_CpuState() {
    return m_cpuState;
}

void Cpu::step() {



    unsigned char byte = m_memoryBus.read(m_cpuState.get_pc());



    // Checks opcode (specific operation)
    switch (static_cast<unsigned int>(byte)) {
        // 0x00 means NOP (do nothing; increment pc)
        case 0:
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;

        // JP a16: jump to the 16-bit address encoded after the opcode.
        // First operand byte is low; second operand byte is high (little-endian).
        case 195:
            m_cpuState.set_pc((m_memoryBus.read(m_cpuState.get_pc()+2)<<8)|m_memoryBus.read(m_cpuState.get_pc()+1));
            break;
        // 0x20: JR NZ, e8 (it gives a distance to move)
        case 32:{
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            auto offset = static_cast<std::int8_t>(m_memoryBus.read(m_cpuState.get_pc()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            if(m_cpuState.get_flag(FlagType::Z)==false)m_cpuState.set_pc(m_cpuState.get_pc()+offset);
            break;
        // 0x06 - LD B, n8 (load an 8-bit value into register B)
        }
        case 6:
            m_cpuState.set_b(m_memoryBus.read(m_cpuState.get_pc()+1));
            m_cpuState.set_pc(m_cpuState.get_pc()+2);
            break;
        // 0x0E - LD C,n8 (load an 8 bit value into register C)
        case 14:
            m_cpuState.set_c(m_memoryBus.read(m_cpuState.get_pc()+1));
            m_cpuState.set_pc(m_cpuState.get_pc()+2);
            break;
        // 0x04 - INC B (increment register B)
        case 4:
            // checks if the result is zero (256 -> 0) and changes the flag
            (m_cpuState.get_b() & 0b00001111) == 0b00001111 ? m_cpuState.set_flag(FlagType::H, true) : m_cpuState.set_flag(FlagType::H, false);
            m_cpuState.set_b(m_cpuState.get_b()+1);
            m_cpuState.get_b()==0 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);
            m_cpuState.set_flag(FlagType::N, false); // sets N (addition is false and substraction is true)
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // 0x0D - DEC C (decrease register C)
        case 13:
            (m_cpuState.get_c() & 0b00001111) == 0 ? m_cpuState.set_flag(FlagType::H, true) : m_cpuState.set_flag(FlagType::H, false);
            m_cpuState.set_c(m_cpuState.get_c()-1);
            m_cpuState.get_c()==0 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);
            m_cpuState.set_flag(FlagType::N, true); // sets N (addition is false and substraction is true)
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        default:
            throw std::runtime_error(std::format("Unsupported opcode 0x{:02X} at address 0x{:04X}", byte, m_cpuState.get_pc()));


    }

}
