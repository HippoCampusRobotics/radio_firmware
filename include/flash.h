#ifndef INCLUDE_FLASH
#define INCLUDE_FLASH

#include <stdint.h>

#define FLASH_SCRATCHPAD_ADDRESS 0x00
#define FLASH_SIG0 0x3d
#define FLASH_SIG1 0xc2
#define FLASH_INFO_PAGE 0xf800  // 1 page reserved for bootloader
#define FLASH_LOCK_BYTE 0xfbff
#define FLASH_RESERVED_ADDRESS 0xFFFF
#define FLASH_SIGNATURE_BYTES \
    (FLASH_INFO_PAGE - 2)   // Location of the flash signature

void flash_scratchpad_erase();
uint8_t flash_scratchpad_read(uint16_t address);
void flash_scratchpad_write(uint16_t address, uint8_t value);
uint8_t flash_mcu_signature();
void flash_error_reset();

#endif /* INCLUDE_FLASH */
