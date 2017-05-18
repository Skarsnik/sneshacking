#ifndef H_SCOMPRESS_H
#define H_SCOMPRESS_H

#define NB_POINTER_IN_TABLE 223

#define NORMAL_START 0x87000
#define NORMAL_MAX_END 0xC4200

#define POINTER_TABLE_START 0x04F80

extern char* rom_file;
extern unsigned int header_offset;

typedef struct {
    unsigned int    address;
    unsigned char   bpp;
    bool            compression;
    unsigned int    max_lenght;
    unsigned int    index;
} s_location;

FILE* my_fopen(const char *file, const char *mode);
int rom_fseek(FILE* stream, long offset);
unsigned int    compare_data(const char* data1, const char* data2, const unsigned int lenght);

char*   convert_3bpp_to_4bpp(char *tiles, unsigned int lenght, unsigned int* converted_lenght);
char*   convert_4bpp_to_3bpp(const char* to_convert, size_t lenght, unsigned int* converted_lenght);
void    print_location(s_location loc);

void    copy_locations(s_location* dest, s_location *src);
void    sort_locations(s_location* tosort, s_location *output);

#endif