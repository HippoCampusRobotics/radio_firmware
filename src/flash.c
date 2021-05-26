#include "flash.h"

#include "Si1000_defs.h"

#define FLASH_SCRATCHPAD_WRITE() PSCTL = 0x05
#define FLASH_DATA_READ() PSCTL = 0x00
#define FLASH_SCRATCHPAD_READ() PSCTL = 0x04
#define FLASH_SCRATCHPAD_DISABLE() PSCTL = 0x00
#define FLASH_SCRATCHPAD_ERASE() PSCTL = 0x07
#define FLASH_WRITE() PSCTL = 0x01

inline static void flash_unlock();

inline static void flash_unlock() {
    FLKEY = 0xA5;
    FLKEY = 0xF1;
}

void flash_scratchpad_erase() __critical {
    FLASH_SCRATCHPAD_ERASE();
    flash_unlock();
    *((uint8_t __xdata *)0x0000) = 0xFF;
    FLASH_SCRATCHPAD_DISABLE();
}

uint8_t flash_scratchpad_read(uint16_t address) __critical {
    uint8_t value;
    // select scratchpad
    FLASH_SCRATCHPAD_READ();
    value = *((uint8_t __code *)address);
    FLASH_SCRATCHPAD_DISABLE();
    return value;
}

void flash_scratchpad_write(uint16_t address, uint8_t value) __critical {
    FLASH_SCRATCHPAD_WRITE();
    flash_unlock();
    *((uint8_t __xdata *)address) = value;
    FLASH_SCRATCHPAD_DISABLE();
}

uint8_t flash_mcu_signature() __critical { return *((uint8_t __code *)0xFFFE); }

void flash_error_reset() __critical {
    FLASH_WRITE();
    flash_unlock();
    *((uint8_t __xdata *)FLASH_RESERVED_ADDRESS) = 0xFF;
    while (true)
        ;
}
