#include "cpu.h"

#include <format>
#include <stdexcept>

Cpu::Cpu(MemoryBus memoryBus, CpuState cpuState) : m_memoryBus(memoryBus), m_cpuState(cpuState) {

}

CpuState& Cpu::get_CpuState() {
    return m_cpuState;
}

/**
 *
 * @param opcode the whole instruction
 * @param mask the certain part you want to read
 * @details this function is for arithmetic operation to extract the source
 * @return value of the register
*/
std::uint8_t Cpu::readOperand(std::uint8_t opcode, std::uint8_t mask) const{
    switch (opcode & mask) {
        case 0: return m_cpuState.get_b();
        case 1: return m_cpuState.get_c();
        case 2: return m_cpuState.get_d();
        case 3: return m_cpuState.get_e();
        case 4: return m_cpuState.get_h();
        case 5: return m_cpuState.get_l();
        case 6: return m_memoryBus.read(m_cpuState.get_hl());
        case 7: return m_cpuState.get_a();
        default:
            throw std::runtime_error(std::format("Unsupported operand 0x{:04X}!", opcode));

    }
}

void Cpu::setOperand(std::uint8_t opcode) {

    constexpr std::uint8_t source_mask = 0b00000111;

    // Shift the three lower bits to the left and then take it!
    switch ((opcode >> 3) & 0b111) {
        case 0: m_cpuState.set_b(readOperand(opcode, source_mask)); break;
        case 1: m_cpuState.set_c(readOperand(opcode, source_mask)); break;
        case 2: m_cpuState.set_d(readOperand(opcode, source_mask)); break;
        case 3: m_cpuState.set_e(readOperand(opcode, source_mask)); break;
        case 4: m_cpuState.set_h(readOperand(opcode, source_mask)); break;
        case 5: m_cpuState.set_l(readOperand(opcode, source_mask)); break;
        case 6: m_memoryBus.write(m_cpuState.get_hl(), readOperand(opcode, source_mask)); break;
        case 7: m_cpuState.set_a(readOperand(opcode, source_mask)); break;
        default:
            throw std::runtime_error(std::format("Unsupported operand 0x{:04X}!", opcode));

    }
}



void Cpu::arithmeticOperation(std::uint8_t opcode) {

    std::uint8_t operand = readOperand(opcode, 0b00000111);

    switch (opcode & 0b00111000) {
        // ADD A, ...
        case 0b00000000: {

            m_cpuState.set_flag(FlagType::H,((m_cpuState.get_a() & 0x0F) + (operand & 0x0F)) > 0x0F);
            m_cpuState.get_a()+operand > 255 ? m_cpuState.set_flag(FlagType::C, true) : m_cpuState.set_flag(FlagType::C, false);
            m_cpuState.set_a(m_cpuState.get_a()+operand);
            m_cpuState.get_a() == 0x00 ? m_cpuState.set_flag(FlagType::Z, true) : m_cpuState.set_flag(FlagType::Z, false);
            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction
            break;
        }
        // ADC A, ...
        case 0b00001000: {
            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            m_cpuState.get_a() + operand + old_carry_value > 255
                ? m_cpuState.set_flag(FlagType::C, true)
                : m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_flag(
                FlagType::H,
                (m_cpuState.get_a() & 0x0F) + (operand & 0x0F) + old_carry_value  > 0x0F);


            m_cpuState.set_a(m_cpuState.get_a()+operand+old_carry_value);


            m_cpuState.get_a() == 0x00
                ? m_cpuState.set_flag(FlagType::Z, true)
                : m_cpuState.set_flag(FlagType::Z, false);

            m_cpuState.set_flag(FlagType::N, false); // false because it is not a subtraction
            break;
        }
        // SUB A, ...
        case 0b00010000: {

            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<operand);
            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < (operand & 0b00001111));

            m_cpuState.set_a(m_cpuState.get_a()-operand);

            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);
            break;
        }
        // SBC A, ...
        case 0b00011000: {

            std::uint8_t old_carry_value = m_cpuState.get_flag(FlagType::C) ? 1 : 0;

            m_cpuState.set_flag(FlagType::H,(m_cpuState.get_a() & 0b00001111) < ((operand & 0b00001111)+old_carry_value));
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a() < (operand + old_carry_value));

            m_cpuState.set_a(m_cpuState.get_a()-operand-old_carry_value);

            m_cpuState.set_flag(FlagType::N, true); // it is true, because it is a subtraction
            m_cpuState.set_flag(FlagType::Z, m_cpuState.get_a()==0x00);
            break;
        }
        // AND A, ...
        case 0b00100000: {

            m_cpuState.set_flag(FlagType::H, true);
            m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()&operand);

            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);
            break;
        }
        // XOR A, ...
        case 0b00101000: {

            m_cpuState.set_flag(FlagType::H, false);
            m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()^operand);

            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);
            break;
        }
        // OR A, ...
        case 0b00110000: {

            m_cpuState.set_flag(FlagType::H, false);
            m_cpuState.set_flag(FlagType::C, false);

            m_cpuState.set_a(m_cpuState.get_a()|operand);

            m_cpuState.set_flag(FlagType::N, false); // it is false, because it is not subtraction
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==0x00);
            break;
        }
        // CP A, ... (What happens if we subtract but we don't subtract)
        case 0b00111000: {
            m_cpuState.set_flag(FlagType::H, (m_cpuState.get_a()&0b00001111)<(operand&0b00001111));
            m_cpuState.set_flag(FlagType::C, m_cpuState.get_a()<operand);
            m_cpuState.set_flag(FlagType::N, true);
            m_cpuState.set_flag(FlagType::Z,m_cpuState.get_a()==operand);
            break;
        }
        default:
            throw std::runtime_error(std::format("Unsupported arithmetic Operator 0x{:04X}!", opcode & 0b00000111));

    }

    m_cpuState.set_pc(m_cpuState.get_pc()+1);
}

void Cpu::loadOperation(std::uint8_t opcode) {

    setOperand(opcode);
    m_cpuState.set_pc(m_cpuState.get_pc()+1);
}

void Cpu::step() {



    unsigned char byte = m_memoryBus.read(m_cpuState.get_pc());

    // Checks opcode (specific operation) (family cases)
    switch (static_cast<unsigned int>(byte) & 0b11000000) {
        case 0b01000000: {
            // Halt Instruction to pause
            if (static_cast<unsigned int>(byte)==0x76) {
                throw std::runtime_error(std::format("Unsupported opcode 0x76 at address 0x{:04X}", m_cpuState.get_pc()));
            }

            loadOperation(byte);
            return;
        }
        case 0b10000000:
            arithmeticOperation(byte);
            return;
    }

    // Checks opcode (specific operation) (special cases)
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

        // LD SP, N16 (Load the next two bytes after 0x31)
        case 0x31:
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            // shift the upper byte to the left and attach them with the or operator
            m_cpuState.set_sp((m_memoryBus.read(m_cpuState.get_pc()+1)<<8) | m_memoryBus.read(m_cpuState.get_pc()));
            m_cpuState.set_pc(m_cpuState.get_pc()+2);
            break;
        // push BC to the stack
        case 0xC5:
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_b());
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_c());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // push DE to the stack
        case 0xD5:
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_d());
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_e());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // push HL to the stack
        case 0xE5:
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_h());
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_l());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        // push AF to the stack
        case 0xF5:
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_a());
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), m_cpuState.get_f());
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        //  pop BC from the stack
        case 0xC1:
            m_cpuState.set_c(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_b(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        //  pop DE from the stack
        case 0xD1:
            m_cpuState.set_e(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_d(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        //  pop HL from the stack
        case 0xE1:
            m_cpuState.set_l(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_h(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;
        //  pop AF from the stack
        case 0xF1:
            m_cpuState.set_f(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_a(m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+1);
            m_cpuState.set_pc(m_cpuState.get_pc()+1);
            break;

        // CALL (Subroutine)
        case 0xCD: {


            const std::uint16_t return_address = m_cpuState.get_pc()+3;

            m_cpuState.set_pc((
                m_memoryBus.read(m_cpuState.get_pc()+2)<<8)|m_memoryBus.read(m_cpuState.get_pc()+1));

            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            // High Byte (shift 8 bytes right because during conversion the low bytes still remain)
            m_memoryBus.write(m_cpuState.get_sp(), return_address>>8);
            m_cpuState.set_sp(m_cpuState.get_sp()-1);
            m_memoryBus.write(m_cpuState.get_sp(), return_address&0x00FF); // Low Byte
            break;
        }

        // RET (return after Subroutine)
        case 0xC9: {
            m_cpuState.set_pc((m_memoryBus.read(m_cpuState.get_sp()+1)<<8)|m_memoryBus.read(m_cpuState.get_sp()));
            m_cpuState.set_sp(m_cpuState.get_sp()+2);
            break;
        }
        default:
            throw std::runtime_error(std::format("Unsupported opcode 0x{:02X} at address 0x{:04X}", byte, m_cpuState.get_pc()));
    }
}
