/*
Copyright 2016 Sylvain "Skarsnik" Colinet

 This file is part of the SkarAlttp project.

    SkarAlttp is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SkarAlttp.  If not, see <http://www.gnu.org/licenses/>
    */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "alttpcompression.h"

char* alttp_decompression_error = NULL;
char* alttp_compression_error = NULL;

static char*   my_asprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t needed = vsnprintf(NULL, 0, fmt, args);
    char  *buffer = malloc(needed + 1);
    vsnprintf(buffer, needed + 1, fmt, args);
    return buffer;
}

char*	alttp_decompress_gfx(const char *c_data, const size_t start, unsigned int max_lenght, unsigned int* uncompressed_data_size, unsigned int* compressed_lenght)
{
    char *toret = std_nintendo_decompress(c_data, start, max_lenght, uncompressed_data_size, compressed_lenght, D_NINTENDO_C_MODE2);
    alttp_decompression_error = std_nintendo_decompression_error;
    return toret;
}


char*	alttp_decompress_overworld(const char *c_data, const size_t start, unsigned int max_lenght, unsigned int* uncompressed_data_size, unsigned int* compressed_lenght)
{
    char* toret = std_nintendo_compress(c_data, start, max_lenght, uncompressed_data_size, compressed_lenght, D_NINTENDO_C_MODE1);
    alttp_decompression_error = std_nintendo_decompression_error;
    return toret;
}


char*	alttp_compress_gfx(const char* u_data, const size_t start, const unsigned int lenght, unsigned int* compressed_size)
{
    return std_nintendo_compress(u_data, start, lenght, compressed_size, D_NINTENDO_C_MODE2);
}

char*	alttp_compress_overworld(const char* u_data, const size_t start, const unsigned int lenght, unsigned int* compressed_size)
{
    return std_nintendo_compress(u_data, start, lenght, compressed_size, D_NINTENDO_C_MODE1);
}
