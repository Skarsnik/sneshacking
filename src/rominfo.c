#include "rominfo.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static char*	hexString(const char* str, const unsigned int size)
{
    char* toret = (char*) malloc(size * 3 + 1);

    unsigned int i;
    for (i = 0; i < size; i++)
    {
        sprintf(toret + i * 3, "%02X ", (unsigned char)str[i]);
    }
    toret[size * 3] = 0;
    return toret;
}


struct rom_infos*    get_rom_info(const uint8_t* data)
{
    struct rom_infos* toret = (struct rom_infos*) malloc(sizeof(struct rom_infos));
    memcpy(toret->title, data, 21);
    toret->type = LoROM;
    toret->fastrom = (data[21] & 0b00110000) == 0b00110000;
    if (data[21] & 1)
        toret->type = HiROM;
    if ((data[21] & 0b00000111) == 0b00000111)
        toret->type = ExHiROM;
    //printf("%s\n", hexString(data + 36, 20));
    toret->size = 0x400 << data[23];
    toret->sram_size = 0x400 << data[24];
    toret->creator_id =  (data[26] << 8) | data[25];
    toret->version = data[27];
    toret->checksum_comp = (data[29] << 8) | data[28];
    toret->checksum = (data[31] << 8) | data[30];
    toret->make_sense = false;
    if ((toret->checksum ^ toret->checksum_comp) == 0xFFFF)
        toret->make_sense = true;

    const uint8_t vector_start = 36;
    toret->native_cop = (data[vector_start + 1] << 8) | data[vector_start];
    toret->native_brk = (data[vector_start + 3] << 8) | data[vector_start + 2];
    toret->native_abort = (data[vector_start + 5] << 8) | data[vector_start + 4];
    toret->native_nmi = (data[vector_start + 7] << 8) | data[vector_start + 6];
    toret->native_reset = (data[vector_start + 9] << 8) | data[vector_start + 8];
    toret->native_irq = (data[vector_start + 11] << 8) | data[vector_start + 10];

    toret->emulation_cop = (data[vector_start + 17] << 8) | data[vector_start + 16];
    toret->emulation_abort = (data[vector_start + 21] << 8) | data[vector_start + 20];
    toret->emulation_nmi = (data[vector_start + 23] << 8) | data[vector_start + 22];
    toret->emulation_reset = (data[vector_start + 25] << 8) | data[vector_start + 24];
    toret->emulation_irq = (data[vector_start + 27] << 8) | data[vector_start + 26];
    return toret;
}
