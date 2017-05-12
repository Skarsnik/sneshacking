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
    bool 			compression;
    unsigned int	max_lenght;
} s_location;


char* rom_file = "/home/skarsnik/Zelda - A Link to the Past.smc";
unsigned int header_offset = 0x200;

#define NB_POINTER_IN_TABLE 223

unsigned int gfx_table_pointers[NB_POINTER_IN_TABLE];
unsigned int gfx_table_pc_locations[NB_POINTER_IN_TABLE];

/* zcompress skip some data. Here are all the data :
* 00-DC are extracted from the pointer table
* 00-70  : 3bpp compressed (converted to 4bpp)
* 71, 72 : 2bpp compressed
* 73-7E  : 3bpp uncompressed (converted to 4bpp)
* 7F-D9  : 3bpp compressed (converted to 4bpp)
* DA-DC  : 2bpp compressed
* DD,DE  : 2bpp compressed, not present in zcompress

*/

s_location   gfx_locations[NB_POINTER_IN_TABLE];
s_location   zcompress_gfx_locations[NB_POINTER_IN_TABLE];

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

void	build_gfx_location()
{
    for (unsigned int i = 0; i < NB_POINTER_IN_TABLE; i++)
    {
        gfx_locations[i].address = gfx_table_pc_locations[i];
        gfx_locations[i].max_lenght = 0;
        if (i >= 0 && i <= 0x70)
        {
            gfx_locations[i].bpp = 3;
            gfx_locations[i].compression = true;
            if (i != 0x70)
                gfx_locations[i].max_lenght = gfx_table_pc_locations[i + 1] - gfx_locations[i].address;
            else
                gfx_locations[i].max_lenght = gfx_locations[i - 1].max_lenght;
        }
        if (i == 0x71 || i == 0x72)
        {
            gfx_locations[i].bpp = 2;
            gfx_locations[i].compression = true;
            if (i == 0x71)
                gfx_locations[i].max_lenght = gfx_table_pc_locations[i + 1] - gfx_locations[i].address;
            else
                gfx_locations[i].max_lenght = 2048;
        }
        if (i >= 0x73 && i <= 0x7E)
        {
            gfx_locations[i].bpp = 3;
            gfx_locations[i].compression = false;
            if (i != 0x7E)
                gfx_locations[i].max_lenght = gfx_table_pc_locations[i + 1] - gfx_locations[i].address;
            else
                gfx_locations[i].max_lenght = gfx_locations[i - 1].max_lenght;
        }
        if (i >= 0x7F && i <= 0xD9)
        {
            gfx_locations[i].bpp = 3;
            gfx_locations[i].compression = true;
            if (i != 0xD9)
                gfx_locations[i].max_lenght = gfx_table_pc_locations[i + 1] - gfx_locations[i].address;
            else
                gfx_locations[i].max_lenght = gfx_locations[i - 1].max_lenght;

        }
        if (i >= 0xDA && i <= 0xDE)
        {
            gfx_locations[i].bpp = 2;
            gfx_locations[i].compression = true;
            if (i != 0xDE)
                gfx_locations[i].max_lenght = gfx_table_pc_locations[i + 1] - gfx_locations[i].address;
            else
                gfx_locations[i].max_lenght = gfx_locations[i - 1].max_lenght;
        }
    }
}

void	build_zcompress_gfx_location()
{
    for (unsigned int i = 0; i < NB_POINTER_IN_TABLE; i++)
    {
        zcompress_gfx_locations[i].bpp = gfx_locations[i].bpp;
        zcompress_gfx_locations[i].compression = gfx_locations[i].compression;
        zcompress_gfx_locations[i].address = gfx_locations[i].address;
		zcompress_gfx_locations[i].max_lenght = gfx_locations[i].max_lenght;
    }
    zcompress_gfx_locations[0xDD].address = 0x70000;
    zcompress_gfx_locations[0xDD].bpp = 2;
    zcompress_gfx_locations[0xDD].compression = false;
    zcompress_gfx_locations[0xDD].max_lenght = 2048 * 2;
    zcompress_gfx_locations[0xDE].address = 0x80000;
    zcompress_gfx_locations[0xDE].bpp = 4;
    zcompress_gfx_locations[0xDE].compression = false;
    zcompress_gfx_locations[0xDE].max_lenght = 28672;
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

void	print_location(s_location loc)
{
  printf("0x%06X - %d bpp,  %s  - max lenght: %d\n", loc.address, loc.bpp,
               loc.compression ? "Compressed" : "Uncompressed", loc.max_lenght);
}


void	print_gfx_location()
{
    for (unsigned int i = 0; i < NB_POINTER_IN_TABLE; i++)
    {
        printf("%02X  ", i);
		print_location(gfx_locations[i]);
    }
}


void	extract_gfx(s_location* locations, bool single_file, const char* allfilename)
{
    char read_buffer[1000000]; //~ 1M should be enought x)
    char filename[20];
    FILE* rom_stream = my_fopen(rom_file, "r");
	FILE* gfx_file;
    unsigned int	compressed_lenght;
    unsigned int	decompressed_lenght;
    unsigned int	to_convert_lenght;
    unsigned int	converted_lenght;
    char			*data;
    unsigned int	read_lenght;
    char			*uncompressed;
    char			*to_write;
    unsigned int	write_lenght;
    s_location		location;
	
	if (single_file)
	  gfx_file = my_fopen(allfilename, "w");

    for (unsigned int i = 0; i < NB_POINTER_IN_TABLE; i++)
    {
        location = locations[i];
        unsigned int pc_location = location.address;

		printf("==%02X==\n", i);
		print_location(location);
        rom_fseek(rom_stream, pc_location);
        read_lenght = fread(read_buffer, 1, location.max_lenght, rom_stream);
        write_lenght = read_lenght;
        to_convert_lenght = (2048 / 32) * 24;
        data = read_buffer;
        if (location.compression)
        {
            printf("Compressed\n");
            uncompressed = alttp_decompress(read_buffer, 0, read_lenght, &decompressed_lenght, &compressed_lenght);
			printf("Compressed lenght : %d  - Decompressed lenght : %d\n", compressed_lenght, decompressed_lenght);
			if (read_lenght < compressed_lenght)
			  fprintf(stderr, "WARNING: compressed lenght read exceed the expected read lenght for : %02X\n", i);
            if (uncompressed == NULL)
            {
                fprintf(stderr, "Error while decompressing data for 0x%06X\n", pc_location);
                exit(1);
            }
            data = uncompressed;
            to_convert_lenght = decompressed_lenght;
        }
        if (location.bpp == 3)
        {
            char *converted = convert_3bpp_to_4bpp(data, to_convert_lenght, &converted_lenght);
            write_lenght = converted_lenght;
            to_write = converted;
        }
        if (location.bpp == 2)
        {
            if (location.compression)
            {
                to_write = (char*) malloc(2048);
                memcpy(to_write, data, decompressed_lenght);
				if (decompressed_lenght != 2048)
                  memset(to_write, 0, decompressed_lenght - 1);
                write_lenght = 2048;
            } else {
                write_lenght = location.max_lenght;
                to_write = (char*) malloc(write_lenght);
                memcpy(to_write, read_buffer, write_lenght);
            }
        }
        if (location.bpp == 4)
        {
            write_lenght = location.max_lenght;
            to_write = (char*) malloc(write_lenght);
            memcpy(to_write, read_buffer, write_lenght);
        }
        if (location.compression)
            free(uncompressed);

        if (single_file)
        {
			fwrite(to_write, 1, write_lenght, gfx_file);
        } else {
            sprintf(filename, "SGFX%02X.bin", i);
            gfx_file = my_fopen(filename, "w");
            fwrite(to_write, 1, write_lenght, gfx_file);
            fclose(gfx_file);
        }
        free(to_write);
    }
    if (single_file)
	  fclose(gfx_file);
	fclose(rom_stream);
}


int 	main(int ac, char *ag[])
{

    if (ac == 3 && ag[1][0] == '0')
    {
        set_header_offset();
        build_gfx_table_pointers();
        build_gfx_location();
        build_zcompress_gfx_location();
        extract_gfx(zcompress_gfx_locations, true, "alttpall.bin");
    }
    if (ac ==  3 && ag[1][0] == '2')
    {
        set_header_offset();
        list_table_compressed();
    }
    if (ac == 3 && ag[1][0] == '3')
    {
        set_header_offset();
        build_gfx_table_pointers();
        build_gfx_location();
        print_gfx_location();
    }
    return 0;
}

