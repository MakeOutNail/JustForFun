#pragma once
#include <cstdint>

enum class FlagType{
    Z,
    N,
    H,
    C
};

class CpuState{
public:
    CpuState(std::uint8_t a=0, std::uint8_t b=0, std::uint8_t c=0, std::uint8_t d=0, std::uint8_t e=0, std::uint8_t h=0, std::uint8_t l=0, std::uint16_t sp=0, std::uint16_t pc=0, std::uint8_t f=0);

    [[nodiscard]] std::uint16_t get_bc() const;
    [[nodiscard]] std::uint16_t get_de() const;
    [[nodiscard]] std::uint16_t get_hl() const;
    [[nodiscard]] std::uint16_t get_af() const;

    void set_bc(std::uint16_t bc);
    void set_de(std::uint16_t de);
    void set_hl(std::uint16_t hl);
    void set_af(std::uint16_t af);

    void set_a(std::uint8_t a);
    void set_b(std::uint8_t b);
    void set_c(std::uint8_t c);
    void set_d(std::uint8_t d);
    void set_e(std::uint8_t e);
    void set_h(std::uint8_t h);
    void set_l(std::uint8_t l);
    void set_f(std::uint8_t f);

    [[nodiscard]] std::uint8_t get_a() const;
    [[nodiscard]] std::uint8_t get_b() const;
    [[nodiscard]] std::uint8_t get_c() const;
    [[nodiscard]] std::uint8_t get_d() const;
    [[nodiscard]] std::uint8_t get_e() const;
    [[nodiscard]] std::uint8_t get_h() const;
    [[nodiscard]] std::uint8_t get_l() const;
    [[nodiscard]] std::uint8_t get_f() const;


    [[nodiscard]] std::uint16_t get_sp() const;
    [[nodiscard]] std::uint16_t get_pc() const;
    void set_sp(std::uint16_t sp);
    void set_pc(std::uint16_t pc);

    /**
     *
     * @param position
     * @param status accepts true (1) or false (0)
     */
    void set_flag(FlagType position, bool status);
    [[nodiscard]] bool get_flag(FlagType position) const;

private:

    // --- General-Purpose Registers ---
    std::uint8_t m_a; // accumulator (SPECIAL General-Purpose Register)
    std::uint8_t m_b, m_c, m_d, m_e, m_h, m_l;

    std::uint16_t m_sp; // Stack Pointer
    std::uint16_t m_pc; // Program Counter/Pointer
    std::uint8_t m_f{}; // Flag
};
