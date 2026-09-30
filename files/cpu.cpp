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
        case 0x00:
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;

        // JP a16: jump to the 16-bit address encoded after the opcode.
        // First operand byte is low; second operand byte is high (little-endian).
        case 0XC3:
            m_cpuState.set_pc((m_memoryBus.read(m_cpuState.get_pc()+2)<<8)|m_memoryBus.read(m_cpuState.get_pc()+1));
            break;
        // 0x20: JR NZ, e8 (it gives a distance to move)
        case 0x20:{
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            auto offset = static_cast<std::int8_t>(m_memoryBus.read(m_cpuState.get_pc()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            if(m_cpuState.get_flag(FlagType::Z)==false)m_cpuState.set_pc(m_cpuState.get_pc()+offset);
            break;
        // 0x06 - LD B, n8 (load an 8-bit value into register B)
        }
        case 0x06:
            m_cpuState.set_b(m_memoryBus.read(m_cpuState.get_pc()+1));
            m_cpuState.set_pc(m_cpuState.get_pc()+2);
            break;
        // 0x0E - LD C,n8 (load an 8 bit value into register C)
        case 0x0E:
            m_cpuState.set_c(m_memoryBus.read(m_cpuState.get_pc()+1));
            m_cpuState.set_pc(m_cpuState.get_pc()+2);
            break;
        // 0x04 - INC B (increment register B)
        case 0x04:
            // checks if the result is zero (256 -> 0) and changes the flag
            (m_cpuState.get_b() & 0b00001111) == 0b00001111 ? m_cpuState.set_flag(FlagType::H, true) : m_cpuState.set_flag(FlagType::H, false);
            m_cpuState.set_b(m_cpuState.get_b()+1);
            m_cpuState.get_b()==0 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);
            m_cpuState.set_flag(FlagType::N, false); // sets N (addition is false and substraction is true)
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // 0x0D - DEC C (decrease register C)
        case 0x0D:
            (m_cpuState.get_c() & 0b00001111) == 0 ? m_cpuState.set_flag(FlagType::H, true) : m_cpuState.set_flag(FlagType::H, false);
            m_cpuState.set_c(m_cpuState.get_c()-1);
            m_cpuState.get_c()==0 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);
            m_cpuState.set_flag(FlagType::N, true); // sets N (addition is false and substraction is true)
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, n8; LOAD 8 BIT INTO REGISTER A
        case 0x3E:
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            m_cpuState.set_a(m_memoryBus.read(m_cpuState.get_pc()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD HL n16; LOAD 16 BIT INTO REGISTER HL
        case 0x21:
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            m_cpuState.set_hl((m_memoryBus.read(m_cpuState.get_pc()+1) << 8) | m_memoryBus.read(m_cpuState.get_pc()));
            m_cpuState.set_pc(m_cpuState.get_pc()+2);
            break;
        // LD [HL], A; Store A value at the address in HL
        case 0x77:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, [HL]; Read that value at address HL into A
        case 0x7E:
            m_cpuState.set_a(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;

        default:
            throw std::runtime_error(std::format("Unsupported opcode 0x{:02X} at address 0x{:04X}", byte, m_cpuState.get_pc()));


    }

}
