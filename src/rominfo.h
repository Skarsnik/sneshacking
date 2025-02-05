#ifndef H_ROM_INFO_H
#define H_ROM_INFO_H
#include <stdint.h>
#include <stdbool.h>

#include "rommapping.h"

/*
https://en.wikibooks.org/wiki/Super_NES_Programming/SNES_memory_map
$xFC0	Game title.	21 bytes, usually uppercase, but there are no limitations to it. Basically every ASCII character between (not including) $1F and $7F is valid.
$xFD5	ROM makeup byte.	xxAAxxxB; AA==11 means FastROM ($30). If B is set, it's HiROM, otherwise it's LoROM.
$xFD6	ROM type.	ROM/RAM/SRAM/DSP1/FX
$xFD7	ROM size.	The size bits for the ROM, as the ROM sees itself. If you do $400<<(ROM size), you get the overall size in byte
$xFD8	SRAM size.	The size bits for the SRAM, RAM that is really fast and can be used by the cartridge for various tasks. Usually though, this memory is just used to save your progress. To get the size again use $400<<(RAM size).
$xFD9	Creator license ID code.
$xFDB	Version #.
$xFDC	Checksum complement.
$xFDE	Checksum.
*/

#ifdef __cplusplus
extern "C" {
#endif


struct rom_infos {
    enum rom_type   type;
    bool            fastrom;
    bool            make_sense;
    char            title[21];
    uint32_t        size;
    uint32_t        sram_size;
    uint16_t        creator_id;
    uint8_t         version;
    uint16_t        checksum_comp;
    uint16_t        checksum;


    uint16_t        native_cop;
    uint16_t        native_brk;
    uint16_t        native_abort;
    uint16_t        native_nmi;
    uint16_t        native_reset;
    uint16_t        native_irq;
    uint16_t        emulation_cop;
    uint16_t        emulation_abort;
    uint16_t        emulation_nmi;
    uint16_t        emulation_reset;
    uint16_t        emulation_irq;
};

struct rom_infos* get_rom_info(const uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif
