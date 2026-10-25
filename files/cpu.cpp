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
            // ADD A (Accumulator), B (Register)
        case 0x80:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_cpuState.get_b() & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_cpuState.get_b() > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_b());
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADD A (Accumulator), C (Register)
        case 0x81:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_cpuState.get_c() & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_cpuState.get_c() > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_c());
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADD A (Accumulator), D (Register)
        case 0x82:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_cpuState.get_d() & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_cpuState.get_d() > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_d());
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADD A (Accumulator), E (Register)
        case 0x83:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_cpuState.get_e() & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_cpuState.get_e() > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_e());
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADD A (Accumulator), H (Register)
        case 0x84:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_cpuState.get_h() & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_cpuState.get_h() > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_h());
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADD A (Accumulator), L (Register)
        case 0x85:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_cpuState.get_l() & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_cpuState.get_l() > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_l());
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADD A (Accumulator), HL (Address)
        case 0x86:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_memoryBus.read(m_cpuState.get_hl()) & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_memoryBus.read(m_cpuState.get_hl()) > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_memoryBus.read(m_cpuState.get_hl()));
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADD A (Accumulator), A (Accumulator)
        case 0x87:
            // Flag H
            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (m_cpuState.get_a() & 0x0F)) > 0x0F);

            // Flag C
            m_cpuState.get_a()+m_cpuState.get_a() > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_a());
            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
            // ADC A (Accumulator), B (Register)
        case 0x88:{

            // Add the old carry flag

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_cpuState.get_b() + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_cpuState.get_b() & 0x0F) + old_carry_value  > 0x0F);


            m_cpuState.set_a(m_cpuState.get_a()+m_cpuState.get_b()+old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // ADC A (Accumulator), C (Register)
        case 0x89:{
            // Add the old carry flag
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_cpuState.get_c() + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_cpuState.get_c() & 0x0F) + old_carry_value > 0x0F);

            m_cpuState.set_a(m_cpuState.get_a() + m_cpuState.get_c() + old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // ADC A (Accumulator), D (Register)
        case 0x8A:{
            // Add the old carry flag
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_cpuState.get_d() + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_cpuState.get_d() & 0x0F) + old_carry_value > 0x0F);

            m_cpuState.set_a(m_cpuState.get_a() + m_cpuState.get_d() + old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // ADC A (Accumulator), E (Register)
        case 0x8B:{
            // Add the old carry flag
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_cpuState.get_e() + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_cpuState.get_e() & 0x0F) + old_carry_value > 0x0F);

            m_cpuState.set_a(m_cpuState.get_a() + m_cpuState.get_e() + old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // ADC A (Accumulator), H (Register)
        case 0x8C:{
            // Add the old carry flag
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_cpuState.get_h() + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_cpuState.get_h() & 0x0F) + old_carry_value > 0x0F);

            m_cpuState.set_a(m_cpuState.get_a() + m_cpuState.get_h() + old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // ADC A (Accumulator), L (Register)
        case 0x8D:{
            // Add the old carry flag
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_cpuState.get_l() + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_cpuState.get_l() & 0x0F) + old_carry_value > 0x0F);

            m_cpuState.set_a(m_cpuState.get_a() + m_cpuState.get_l() + old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // ADC A (Accumulator), [HL] (Memory)
        case 0x8E:{
            // Add the old carry flag
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_memoryBus.read(m_cpuState.get_hl()) + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_memoryBus.read(m_cpuState.get_hl()) & 0x0F) + old_carry_value > 0x0F);

            m_cpuState.set_a(m_cpuState.get_a() + m_memoryBus.read(m_cpuState.get_hl()) + old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // ADC A (Accumulator), A (Accumulator)
        case 0x8F:{
            // Add the old carry flag
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag C
            m_cpuState.get_a() + m_cpuState.get_a() + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            // Flag H
            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (m_cpuState.get_a() & 0x0F) + old_carry_value > 0x0F);

            m_cpuState.set_a(m_cpuState.get_a() + m_cpuState.get_a() + old_carry_value);

            // Flag Z
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);

            // Flag N
            m_cpuState.set_flag(FlagType::N, false);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // SUB A (Accumulator), B (Register) (A-B and store in A again)
        case 0x90:


            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_b());

            // Flag check if the lower 4 bits of A are smaller than the lower bits of B
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_cpuState.get_b() & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_b());

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // SUB A (Accumulator), C (Register) (A-C and store in A again)
        case 0x91:

            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_c());

            // Flag check if the lower 4 bits of A are smaller than the lower bits of C
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_cpuState.get_c() & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_c());

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // SUB A (Accumulator), D (Register) (A-D and store in A again)
        case 0x92:

            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_d());

            // Flag check if the lower 4 bits of A are smaller than the lower bits of D
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_cpuState.get_d() & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_d());

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // SUB A (Accumulator), E (Register) (A-E and store in A again)
        case 0x93:

            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_e());

            // Flag check if the lower 4 bits of A are smaller than the lower bits of E
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_cpuState.get_e() & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_e());

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // SUB A (Accumulator), H (Register) (A-H and store in A again)
        case 0x94:

            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_h());

            // Flag check if the lower 4 bits of A are smaller than the lower bits of H
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_cpuState.get_h() & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_h());

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // SUB A (Accumulator), L (Register) (A-L and store in A again)
        case 0x95:

            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_l());

            // Flag check if the lower 4 bits of A are smaller than the lower bits of L
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_cpuState.get_l() & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_l());

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // SUB A (Accumulator), [HL] (Memory) (A-[HL] and store in A again)
        case 0x96:

            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_memoryBus.read(m_cpuState.get_hl()));

            // Flag check if the lower 4 bits of A are smaller than the lower bits of [HL]
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_memoryBus.read(m_cpuState.get_hl()) & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_memoryBus.read(m_cpuState.get_hl()));

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // SUB A (Accumulator), A (Accumulator) (A-A and store in A again)
        case 0x97:

            // Flag C (Run it before you change the register a)
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_a());

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of B plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (m_cpuState.get_a() & 0b00001111));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_a());

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        //  SBC A, B ..... A - B - old carry
        case 0x98: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of B plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_cpuState.get_b() & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_cpuState.get_b() + old_carry_value));


            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_b()-old_carry_value);


            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        //  SBC A, C ..... A - C - old carry
        case 0x99: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of C plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_cpuState.get_c() & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_cpuState.get_c() + old_carry_value));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_c()-old_carry_value);

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        //  SBC A, D ..... A - D - old carry
        case 0x9A: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of D plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_cpuState.get_d() & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_cpuState.get_d() + old_carry_value));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_d()-old_carry_value);

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        //  SBC A, E ..... A - E - old carry
        case 0x9B: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of E plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_cpuState.get_e() & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_cpuState.get_e() + old_carry_value));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_e()-old_carry_value);

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        //  SBC A, H ..... A - H - old carry
        case 0x9C: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of H plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_cpuState.get_h() & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_cpuState.get_h() + old_carry_value));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_h()-old_carry_value);

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        //  SBC A, L ..... A - L - old carry
        case 0x9D: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of L plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_cpuState.get_l() & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_cpuState.get_l() + old_carry_value));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_l()-old_carry_value);

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        //  SBC A, [HL] ..... A - [HL] - old carry
        case 0x9E: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of [HL] plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_memoryBus.read(m_cpuState.get_hl()) & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_memoryBus.read(m_cpuState.get_hl()) + old_carry_value));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_memoryBus.read(m_cpuState.get_hl())-old_carry_value);

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        //  SBC A, A ..... A - A - old carry
        case 0x9F: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            // Flag H check if the lower 4 bits of A are smaller than the lower bits of A plus old carry
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((m_cpuState.get_a() & 0b00001111)+old_carry_value));

            // Set C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (m_cpuState.get_a() + old_carry_value));

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()-m_cpuState.get_a()-old_carry_value);

            // Flag N
            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        // AND A, B (compare each bit with the and operator)
        case 0xA0:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_cpuState.get_b());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);

            break;
        // AND A, C (compare each bit with the and operator)
        case 0xA1:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_cpuState.get_c());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // AND A, D (compare each bit with the and operator)
        case 0xA2:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_cpuState.get_d());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // AND A, E (compare each bit with the and operator)
        case 0xA3:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_cpuState.get_e());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // AND A, H (compare each bit with the and operator)
        case 0xA4:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_cpuState.get_h());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // AND A, L (compare each bit with the and operator)
        case 0xA5:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_cpuState.get_l());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // AND A, [HL] (compare each bit with the and operator)
        case 0xA6:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_memoryBus.read(m_cpuState.get_hl()));

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // AND A, A (compare each bit with the and operator)
        case 0xA7:

            // Flag H (AND always sets H)
            m_cpuState.set_flag(FlagType::H, true);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()&m_cpuState.get_a());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;

        // XOR A, B
        case 0xA8:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_cpuState.get_b());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // XOR A, C
        case 0xA9:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_cpuState.get_c());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // XOR A, D
        case 0xAA:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_cpuState.get_d());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // XOR A, E
        case 0xAB:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_cpuState.get_e());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // XOR A, H
        case 0xAC:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_cpuState.get_h());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // XOR A, L
        case 0xAD:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_cpuState.get_l());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // XOR A, [HL]
        case 0xAE:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_memoryBus.read(m_cpuState.get_hl()));

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // XOR A, A
        case 0xAF:
            // Flag H (XOR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()^m_cpuState.get_a());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // OR A, B
        case 0xB0:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_cpuState.get_b());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;


        // OR A, C
        case 0xB1:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_cpuState.get_c());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // OR A, D
        case 0xB2:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_cpuState.get_d());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // OR A, E
        case 0xB3:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_cpuState.get_e());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // OR A, H
        case 0xB4:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_cpuState.get_h());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // OR A, L
        case 0xB5:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_cpuState.get_l());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // OR A, [HL]
        case 0xB6:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_memoryBus.read(m_cpuState.get_hl()));

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // OR A, A
        case 0xB7:
            // Flag H (OR clears H)
            m_cpuState.set_flag(FlagType::H, false);

            // No Addition or subtraction -> false
            m_cpuState.set_flag(FlagType::C, false);

            // Calculation
            m_cpuState.set_a(m_cpuState.get_a()|m_cpuState.get_a());

            // Flag N
            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            // Flag Z
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;

            // CP A, B (What happens if we subtract but we don't subtract)
        case 0xB8:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_cpuState.get_b()&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_b());


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_cpuState.get_b());

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
            // CP A, C (What happens if we subtract but we don't subtract)
        case 0xB9:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_cpuState.get_c()&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_c());


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_cpuState.get_c());

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
            // CP A, D (What happens if we subtract but we don't subtract)
        case 0xBA:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_cpuState.get_d()&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_d());


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_cpuState.get_d());

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
            // CP A, E (What happens if we subtract but we don't subtract)
        case 0xBB:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_cpuState.get_e()&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_e());


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_cpuState.get_e());

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
            // CP A, H (What happens if we subtract but we don't subtract)
        case 0xBC:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_cpuState.get_h()&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_h());


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_cpuState.get_h());

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
            // CP A, L (What happens if we subtract but we don't subtract)
        case 0xBD:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_cpuState.get_l()&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_l());


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_cpuState.get_l());

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
            // CP A, [HL] (What happens if we subtract but we don't subtract)
        case 0xBE:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_memoryBus.read(m_cpuState.get_hl())&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_memoryBus.read(m_cpuState.get_hl()));


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_memoryBus.read(m_cpuState.get_hl()));

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
            // CP A, A (What happens if we subtract but we don't subtract)
        case 0xBF:{

            // Flag H
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(m_cpuState.get_a()&0b00001111));

            // Flag C
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<m_cpuState.get_a());


            // Flag N (True because it is a subtraction)
            m_cpuState.set_flag(FlagType::N, true);

            // Flag Z (is the result zero)
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==m_cpuState.get_a());

            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        }
        default:
            throw std::runtime_error(std::format("Unsupported opcode 0x{:02X} at address 0x{:04X}", byte, m_cpuState.get_pc()));
    }
}
