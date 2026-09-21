#pragma once
#include <cstdint>
#include <string_view>
#include <vector>


class Rom {
public:
    Rom(std::string_view file_path);
    // compiler warns if you ignore the returned value!
    [[nodiscard]] size_t get_rom_byte_size() const;

    /**
     *
     * @param offset should be in decimal (1 = 1 byte offset) format!
     * @return
     */
    [[nodiscard]] unsigned char get_byte(size_t offset) const;
private:
    std::vector<char> byte_collection;
};
