#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>

#include "tile.h"
#include "rommapping.h"
#include "compressions/alttpcompression.h"

typedef struct {
    unsigned int	address;
    unsigned char	bpp;
    bool 		compression;
} s_location;


char* rom_file = "/home/skarsnik/Zelda - A Link to the Past.smc";
unsigned int header_offset = 0x200;

#define NB_POINTER_IN_TABLE 222

unsigned int gfx_table_pointers[NB_POINTER_IN_TABLE];
unsigned int gfx_table_pc_locations[NB_POINTER_IN_TABLE];

/* Most data in the table are 3bpp, zcompress convert them to 4bpp
* some data are stored as 2bpp, zcompress extract them as 2bpp
*/

#define TABLE_INDEX2BPP_SIZE 6

unsigned char index_2bpp_compressed[TABLE_INDEX2BPP_SIZE] = {0x71, 0x72, 0xDA, 0xDB, 0xDC, 0xDD};

unsigned char index_2bpp_uncompressed[] = {0xDD};

void set_header_offset()
{
    struct stat st;
    stat(rom_file, &st);
    if (st.st_size & 0x200)
    {
        header_offset = 0x200;
        printf("Rom has header\n");
    }
    else
    {
        header_offset = 0;
        printf("Rom has no header\n");
    }
}

/* $4FF3-$528F - indexed (long) table to compressed graphics packets
 * $80000-$86FFF - 4bpp graphics sets for all of Link's animations

************************************

$87000-$8B7FF - 3bpp graphics sets (uncompressed)

 When reinserting, I highly recommend using address 0x87200 since
 that's the address of the original gfx. 0xC4200 (end)
 */
FILE* my_fopen(const char *file, const char *mode)
{
    FILE* toret = fopen(file, mode);
    if (toret == NULL)
    {
        fprintf(stderr, "Can't open %s file : %s\n", file, strerror(errno));
        exit(1);
    }
    return toret;
}

int	rom_fseek(FILE* stream, long offset)
{
    return fseek(stream, offset + header_offset, SEEK_SET);
}

static char*	hexString(const char* str, const unsigned int size)
{
    char* toret = malloc(size * 3 + 1);

    unsigned int i;
    for (i = 0; i < size; i++)
    {
        sprintf(toret + i * 3, "%02X ", (unsigned char) str[i]);
    }
    toret[size * 3] = 0;
    return toret;
}

// ROM_DATA[0x04F80 + 446 + i], ROM_DATA[0x04F80 + 223 + i], ROM_DATA[0x04F80 + i]

void	build_gfx_table_pointers()
{
    FILE* rom_stream = my_fopen(rom_file, "r");
    unsigned char e_addr[3];
    for (unsigned int i = 0; i < NB_POINTER_IN_TABLE; i++)
    {
        rom_fseek(rom_stream, 0x04F80 + i);
        fread(e_addr, 1, 1, rom_stream);
        rom_fseek(rom_stream, 0x04F80 + 223 + i);
        fread(e_addr + 1, 1, 1, rom_stream);
        rom_fseek(rom_stream, 0x04F80 + 446 + i);
        fread(e_addr + 2, 1, 1, rom_stream);

        /*char *hex_str = hexString((char*) e_addr, 3);
        printf("Address read : %s\n", hex_str);
        free(hex_str);*/
        gfx_table_pointers[i] = (e_addr[0] << 16) + (e_addr[1] << 8) + e_addr[2];
        char* mapping_error;
        int pc_addr = lorom_snes_to_pc(gfx_table_pointers[i], &mapping_error);
        if (pc_addr < 0)
        {
            fprintf(stderr, "Error while mapping a snes address to pc : %s\n", mapping_error);
            exit(1);
        }
        gfx_table_pc_locations[i] = (unsigned int) pc_addr;
    }
    fclose(rom_stream);
}


// Just list the table
void	list_table_compressed()
{
    set_header_offset();
    printf("Listing graphics address in $04F80 table\n");
    build_gfx_table_pointers();
    for (unsigned int i = 0; i < 20; i++)
    {
        printf("SNES location 0x%06X", gfx_table_pointers[i]);
        printf(" -> PC : 0x%06X\n", gfx_table_pc_locations[i]);
    }
}

char*	convert_3bpp_to_4bpp(char *tiles, unsigned int lenght, unsigned int* converted_lenght)
{
    unsigned int nb_tile = lenght / 24;
    *converted_lenght = nb_tile * 32;
    char* converted = (char*) malloc(*converted_lenght);

    for (unsigned int i = 0; i < nb_tile; i++)
    {
        tile8 tile = unpack_bpp3_tile(tiles, i * 24);
        char *bpp4_str = pack_bpp4_tile(tile);
        memcpy(converted + i * 32, bpp4_str, 32);
        free(bpp4_str);
    }
    return converted;
}

bool	is_index_2bpp_compressed(unsigned int index)
{
    for (unsigned int i = 0; i < TABLE_INDEX2BPP_SIZE; i++)
    {
        if (index_2bpp_compressed[i] == index)
            return true;
    }
    return false;
}

bool	is_index_2bpp_uncompressed(unsigned int index)
{
    for (unsigned int i = 0; i < sizeof(index_2bpp_uncompressed); i++)
    {
        if (index_2bpp_uncompressed[i] == index)
            return true;
    }
    return false;
}

// Extract all gfx in separate file
void	extract_gfx_separated()
{
    set_header_offset();
    build_gfx_table_pointers();
    char filename[20];
    char buffer[2048];
    FILE* rom_stream = my_fopen(rom_file, "r");
    unsigned int	compressed_lenght;
    unsigned int	decompressed_lenght;
    unsigned int	converted_lenght;
    bool		is_compressed = false;

    for (unsigned int i = 0; i < NB_POINTER_IN_TABLE; i++)
    {
        sprintf(filename, "SGFX%02X.bin", i);
        unsigned int pc_location = gfx_table_pc_locations[i];
        printf("Creating %s from SNES:%06X | PC:%06X\n", filename, gfx_table_pointers[i], gfx_table_pc_locations[i]);
        FILE* gfx_file = my_fopen(filename, "w");
        rom_fseek(rom_stream, pc_location);
        fread(buffer, 2048, 1, rom_stream);
        is_compressed = (! (pc_location >= 0x87000 && pc_location < 0x8B7FF) );// && !is_index_2bpp_uncompressed(i);
        if (is_compressed)
        {
            char* uncompressed = alttp_decompress(buffer, 0, 2048, &decompressed_lenght, &compressed_lenght);
            if (uncompressed == NULL)
            {
                fprintf(stderr, "Error while decompressing data for 0x%06X\n", pc_location);
                exit(1);
            }
            if (is_index_2bpp_compressed(i))
            {
	        printf("Compressed, 2bpp, no convertion.\n");
                fwrite(uncompressed, decompressed_lenght, 1, gfx_file);
                fseek(gfx_file, 2048, SEEK_SET);
            } else {
	        printf("Compressed, standard, converstion 3bpp to 4bpp.\n");
                char *converted = convert_3bpp_to_4bpp(uncompressed, decompressed_lenght, &converted_lenght);
                fwrite(converted, converted_lenght, 1, gfx_file);
                free(converted);
            }
            free(uncompressed);
        } else {
            if (is_index_2bpp_uncompressed(i))
            {
	        printf("No compression, 2bpp, no convertion\n");
		fwrite(buffer, 2048, 1, gfx_file);
            } else {
	        printf("No compression, conversion 3bpp to 4bpp\n");
                char *converted = convert_3bpp_to_4bpp(buffer, (2048 / 32) * 24, &converted_lenght);
                fwrite(converted, converted_lenght, 1, gfx_file);
                free(converted);
            }
        }
        fclose(gfx_file);
    }
    fclose(rom_stream);
}

int 	main(int ac, char *ag[])
{

    if (ac == 3 && ag[1][0] == '0')
        extract_gfx_separated();
    if (ac ==  3 && ag[1][0] == '2')
        list_table_compressed();
    return 0;
}
