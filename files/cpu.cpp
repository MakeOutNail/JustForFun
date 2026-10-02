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
        // LD B, B
        case 0x40:
            m_cpuState.set_b(m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD B, C
        case 0x41:
            m_cpuState.set_b(m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD B, D
        case 0x42:
            m_cpuState.set_b(m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD B, E
        case 0x43:
            m_cpuState.set_b(m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD B, H
        case 0x44:
            m_cpuState.set_b(m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD B, L
        case 0x45:
            m_cpuState.set_b(m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD B, [HL]
        case 0x46:
            m_cpuState.set_b(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD B, A
        case 0x47:
            m_cpuState.set_b(m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, B
        case 0x48:
            m_cpuState.set_c(m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, C
        case 0x49:
            m_cpuState.set_c(m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, D
        case 0x4A:
            m_cpuState.set_c(m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, E
        case 0x4B:
            m_cpuState.set_c(m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, H
        case 0x4C:
            m_cpuState.set_c(m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, L
        case 0x4D:
            m_cpuState.set_c(m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, [HL]
        case 0x4E:
            m_cpuState.set_c(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD C, A
        case 0x4F:
            m_cpuState.set_c(m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, B
        case 0x50:
            m_cpuState.set_d(m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, C
        case 0x51:
            m_cpuState.set_d(m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, D
        case 0x52:
            m_cpuState.set_d(m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, E
        case 0x53:
            m_cpuState.set_d(m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, H
        case 0x54:
            m_cpuState.set_d(m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, L
        case 0x55:
            m_cpuState.set_d(m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, [HL]
        case 0x56:
            m_cpuState.set_d(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD D, A
        case 0x57:
            m_cpuState.set_d(m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, B
        case 0x58:
            m_cpuState.set_e(m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, C
        case 0x59:
            m_cpuState.set_e(m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, D
        case 0x5A:
            m_cpuState.set_e(m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, E
        case 0x5B:
            m_cpuState.set_e(m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, H
        case 0x5C:
            m_cpuState.set_e(m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, L
        case 0x5D:
            m_cpuState.set_e(m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, [HL]
        case 0x5E:
            m_cpuState.set_e(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD E, A
        case 0x5F:
            m_cpuState.set_e(m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, B
        case 0x60:
            m_cpuState.set_h(m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, C
        case 0x61:
            m_cpuState.set_h(m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, D
        case 0x62:
            m_cpuState.set_h(m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, E
        case 0x63:
            m_cpuState.set_h(m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, H
        case 0x64:
            m_cpuState.set_h(m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, L
        case 0x65:
            m_cpuState.set_h(m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, [HL]
        case 0x66:
            m_cpuState.set_h(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD H, A
        case 0x67:
            m_cpuState.set_h(m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, B
        case 0x68:
            m_cpuState.set_l(m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, C
        case 0x69:
            m_cpuState.set_l(m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, D
        case 0x6A:
            m_cpuState.set_l(m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, E
        case 0x6B:
            m_cpuState.set_l(m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, H
        case 0x6C:
            m_cpuState.set_l(m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, L
        case 0x6D:
            m_cpuState.set_l(m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, [HL]
        case 0x6E:
            m_cpuState.set_l(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD L, A
        case 0x6F:
            m_cpuState.set_l(m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD [HL], B
        case 0x70:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD [HL], C
        case 0x71:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD [HL], D
        case 0x72:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD [HL], E
        case 0x73:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD [HL], H
        case 0x74:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD [HL], L
        case 0x75:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // 0x76 is HALT, not a load instruction.
        // LD [HL], A
        case 0x77:
            m_memoryBus.write(m_cpuState.get_hl(), m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, B
        case 0x78:
            m_cpuState.set_a(m_cpuState.get_b());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, C
        case 0x79:
            m_cpuState.set_a(m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, D
        case 0x7A:
            m_cpuState.set_a(m_cpuState.get_d());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, E
        case 0x7B:
            m_cpuState.set_a(m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, H
        case 0x7C:
            m_cpuState.set_a(m_cpuState.get_h());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, L
        case 0x7D:
            m_cpuState.set_a(m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, [HL]
        case 0x7E:
            m_cpuState.set_a(m_memoryBus.read(m_cpuState.get_hl()));
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // LD A, A
        case 0x7F:
            m_cpuState.set_a(m_cpuState.get_a());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        default:
            throw std::runtime_error(std::format("Unsupported opcode 0x{:02X} at address 0x{:04X}", byte, m_cpuState.get_pc()));


    }

}
