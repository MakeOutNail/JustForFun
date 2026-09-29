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

};
