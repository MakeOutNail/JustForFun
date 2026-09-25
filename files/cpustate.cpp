#include "cpustate.h"
#include <stdexcept>

CpuState::CpuState(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d, std::uint8_t e, std::uint8_t h, std::uint8_t l,  std::uint16_t sp, std::uint16_t pc, std::uint8_t f)
: m_a(a), m_b(b), m_c(c), m_d(d), m_e(e), m_h(h), m_l(l), m_sp(sp), m_pc(pc){
    set_f(f);
}

std::uint16_t CpuState::get_bc() const{
    // register1 * 2 ^ (8) + register2 (|: looks at every position)
    return (m_b << 8) | m_c;
}

std::uint16_t CpuState::get_de() const{
    // register1 * 2 ^ (8) + register2 (|: looks at every position)
    return (m_d << 8) | m_e;
}

std::uint16_t CpuState::get_hl() const{
    // register1 * 2 ^ (8) + register2 (|: looks at every position)
    return (m_h << 8) | m_l;
}

std::uint16_t CpuState::get_af() const {
    // register1 * 2 ^ (8) + register2 (|: looks at every position)
    return (m_a << 8) | m_f;
}


void CpuState::set_a(std::uint8_t a){
    m_a=a;
}

void CpuState::set_b(std::uint8_t b) {
    m_b=b;
}

void CpuState::set_c(std::uint8_t c) {
    m_c=c;
}

void CpuState::set_d(std::uint8_t d) {
    m_d=d;
}

void CpuState::set_e(std::uint8_t e) {
    m_e=e;
}

void CpuState::set_h(std::uint8_t h) {
    m_h=h;
}

void CpuState::set_l(std::uint8_t l) {
    m_l=l;
}

void CpuState::set_f(std::uint8_t f) {

    // Turn off the bit with a bit mask
    f&= ~0b00001111;
    m_f = f;
}

std::uint8_t CpuState::get_a() const{
    return m_a;
}

std::uint8_t CpuState::get_b() const{
    return m_b;
}

std::uint8_t CpuState::get_c() const{
    return m_c;
}

std::uint8_t CpuState::get_d() const{
    return m_d;
}

std::uint8_t CpuState::get_e() const{
    return m_e;
}

std::uint8_t CpuState::get_h() const{
    return m_h;
}

std::uint8_t CpuState::get_l() const{
    return m_l;
}

std::uint8_t CpuState::get_f() const {
    return m_f;
}

void CpuState::set_bc(std::uint16_t bc) {
    set_b(bc/256);
    set_c(bc%256);
}

void CpuState::set_de(std::uint16_t de) {
    set_d(de/256);
    set_e(de%256);
}

void CpuState::set_hl(std::uint16_t hl) {
    set_h(hl/256);
    set_l(hl%256);
}

void CpuState::set_af(std::uint16_t af) {
    set_a(af/256);
    set_f(af%256);
}

std::uint16_t CpuState::get_pc() const {
    return m_pc;
}

std::uint16_t CpuState::get_sp() const {
    return m_sp;
}


void CpuState::set_sp(std::uint16_t sp) {
    m_sp = sp;
}

void CpuState::set_pc(std::uint16_t pc) {
    m_pc = pc;
}

void CpuState::set_flag(FlagType position, bool status) {
    switch (position) {
        case FlagType::Z:
            status ? m_f|=0b10000000 : m_f&= ~0b10000000;
            break;
        case FlagType::N:
            status ? m_f|=0b01000000 : m_f&= ~0b01000000;
            break;
        case FlagType::H:
            status ? m_f|=0b00100000 : m_f&= ~0b00100000;
            break;
        case FlagType::C:
            status ? m_f|=0b00010000 : m_f&= ~0b00010000;
            break;
    }
}

bool CpuState::get_flag(FlagType position) const {
    switch (position) {
        case FlagType::Z:
            return static_cast<bool>(m_f & 0b10000000);
        case FlagType::N:
            return static_cast<bool>(m_f & 0b01000000);
        case FlagType::H:
            return static_cast<bool>(m_f & 0b00100000);
        case FlagType::C:
            return static_cast<bool>(m_f & 0b00010000);
        default:
            throw std::invalid_argument{"Unknown CPU flag"};
    }

}

