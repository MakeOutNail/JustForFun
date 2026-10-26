#pragma once
#include "cpustate.h"
#include "memorybus.h"
#include "rom.h"



class Cpu {
public:
    Cpu(MemoryBus memoryBus, CpuState cpuState);
    CpuState& get_CpuState();
    void step();
private:
    MemoryBus m_memoryBus;
    CpuState m_cpuState;
    std::uint8_t readOperand(std::uint8_t opcode, std::uint8_t mask);
    void setOperand(std::uint8_t opcode);
    void arithmeticOperation(std::uint8_t opcode);
    void loadOperation(std::uint8_t opcode);

};
