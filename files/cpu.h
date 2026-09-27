#pragma once
#include "cpustate.h"
#include "rom.h"



class Cpu {
public:
    Cpu(const Rom& rom, CpuState cpuState);
    CpuState& get_CpuState();
    void step();
private:
    const Rom& m_rom;
    CpuState m_cpuState;

};
