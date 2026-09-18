#include "./header.h"


/**
 *
 * @param address_of_ram you have to pass the address (char* pointer) of the ram (1 byte long)
 * @return the size of the ram and the unit KiB as a string (owning); "-" means unused!
 */
std::string get_ram_size(uint8_t code) {

    switch (code) {
        case 0: return "0 KiB"; // NO RAM
        case 1: return "-"; // Unused
        case 2: return "8 KiB"; // 1 Bank
        case 3: return "32 KiB"; // 4 Banks of 8 KiB each
        case 4: return "128 KiB"; // 16 Banks of 8 KiB each
        case 5: return "64 KiB"; // 8 banks of 8 KiB each
        default: return "No Type Found!";
    }
}


/**
 *
 * @param address_of_cartridge_type you have to pass the address (char* pointer) of the cartridge type (1 byte long)
 * @return e.g.: "ROM ONLY", "MBC1", "MMM01"
 */
std::string get_cartridge_type(uint8_t code) {

    std::string cartridge_type {};
    switch (code) {
        case 0:   cartridge_type = "ROM ONLY"; break;
        case 1:   cartridge_type = "MBC1"; break;
        case 2:   cartridge_type = "MBC1+RAM"; break;
        case 3:   cartridge_type = "MBC1+RAM+BATTERY"; break;
        case 5:   cartridge_type = "MBC2"; break;
        case 6:   cartridge_type = "MBC2+BATTERY"; break;
        case 8:   cartridge_type = "ROM+RAM"; break;
        case 9:   cartridge_type = "ROM+RAM+BATTERY"; break;
        case 11:  cartridge_type = "MMM01"; break;
        case 12:  cartridge_type = "MMM01+RAM"; break;
        case 13:  cartridge_type = "MMM01+RAM+BATTERY"; break;
        case 15:  cartridge_type = "MBC3+TIMER+BATTERY"; break;
        case 16:  cartridge_type = "MBC3+TIMER+RAM+BATTERY"; break;
        case 17:  cartridge_type = "MBC3"; break;
        case 18:  cartridge_type = "MBC3+RAM"; break;
        case 19:  cartridge_type = "MBC3+RAM+BATTERY"; break;
        case 25:  cartridge_type = "MBC5"; break;
        case 26:  cartridge_type = "MBC5+RAM"; break;
        case 27:  cartridge_type = "MBC5+RAM+BATTERY"; break;
        case 28:  cartridge_type = "MBC5+RUMBLE"; break;
        case 29:  cartridge_type = "MBC5+RUMBLE+RAM"; break;
        case 30:  cartridge_type = "MBC5+RUMBLE+RAM+BATTERY"; break;
        case 32:  cartridge_type = "MBC6"; break;
        case 34:  cartridge_type = "MBC7+SENSOR+RUMBLE+RAM+BATTERY"; break;
        case 252: cartridge_type = "POCKET CAMERA"; break;
        case 253: cartridge_type = "BANDAI TAMA5"; break;
        case 254: cartridge_type = "HuC3"; break;
        case 255: cartridge_type = "HuC1+RAM+BATTERY"; break;
        default:  cartridge_type = "UNKNOWN"; break;
    }

    return cartridge_type;

}