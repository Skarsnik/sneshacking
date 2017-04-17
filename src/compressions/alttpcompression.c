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
#include <unistd.h>
#include "alttpcompression.h"

#define INITIAL_ALLOC_SIZE 512

#define D_CMD_COPY 0
#define D_CMD_BYTE_REPEAT 1
#define D_CMD_ALTERNATE_BYTE 2
#define D_CMD_BYTE_INC 3
#define D_CMD_COPY_EXISTING 4

#define D_MAX_NORMAL_LENGHT 32
#define D_MAX_LENGHT 1024

/*
 * The compression format follow a simple pattern:
 * first byte represente a header. The header represent a command and a lenght
 * The bytes after the header have meaning depending on the command
 * Then you have a new header byte and so on, until you hit a header with the value FF
 */

char*	decompress(const char *c_data, const unsigned int start, unsigned int* uncompressed_data_size)
{
    char*		u_data;
    unsigned char	header;
    unsigned int	c_data_pos;
    unsigned int	u_data_pos;
    unsigned int	allocated_memory;

    header = c_data[start];
    u_data = (char *) malloc(INITIAL_ALLOC_SIZE); // No way to know the final size, we will probably realloc if needed
    allocated_memory = INITIAL_ALLOC_SIZE;
    u_data_pos = 0;
    c_data_pos = start;
    while (header != 0xFF)
    {
        unsigned int lenght;
        char command;

        command = header >> 5; // 3 hightest bits are the command
        lenght = (header & 0x1F); // The rest is the lenght

        // Extended header, to allow for bigger lenght value than 32
        if (command == 7)
        {
            // 2 bits in the original header are the hight bit for the new lenght
            // the next byte is added to this lenght
            command = (header >> 2 ) & 7;
            lenght = ((int)((header & 3) << 8)) + (unsigned char) c_data[c_data_pos + 1];
            c_data_pos++;
        }

        //lenght value starts at 0, 0 is 1
        lenght++;

        //printf("header %2X - Command : %d , lenght : %d\n", header, command, lenght);

        if (u_data_pos + lenght > allocated_memory) // Adjust allocated memory
        {
            u_data = realloc(u_data, allocated_memory + INITIAL_ALLOC_SIZE);
            allocated_memory += INITIAL_ALLOC_SIZE;
        }
        switch (command)
        {
        case D_CMD_COPY: { // No compression, data are copied as
            memcpy(u_data + u_data_pos, c_data + c_data_pos + 1, lenght);
            c_data_pos += lenght + 1;
            break;
        }
        case D_CMD_BYTE_REPEAT: { // Copy the same byte lenght time
            memset(u_data + u_data_pos, c_data[c_data_pos + 1], lenght);
            c_data_pos += 2;
            break;
        }
        case D_CMD_ALTERNATE_BYTE: { // Next byte is A, the one after is B, copy the sequence AB lenght times
            char a = c_data[c_data_pos + 1];
            char b = c_data[c_data_pos + 2];
            for (int i = 0; i < lenght; i = i + 2)
            {
                u_data[u_data_pos + i] = a;
                if ((i + 1) < lenght)
                    u_data[u_data_pos + i + 1] = b;
            }
            c_data_pos += 3;
            break;
        }
        case D_CMD_BYTE_INC: { // Next byte is copied and incremented lenght time
            for (int i = 0; i < lenght; i++) {
                u_data[u_data_pos + i] = c_data[c_data_pos + 1] + i;
            }
            c_data_pos += 2;
            break;
        }
        case D_CMD_COPY_EXISTING: { // Next 2 bytes form an offset to pick data from the output (dafuq?)
            int	offset = c_data[c_data_pos + 1] | (c_data[c_data_pos + 2] << 8);
            memcpy(u_data + u_data_pos, u_data + offset, lenght);
            c_data_pos += 3;
            break;
        }
        default: {
            fprintf(stderr, "Invalid command in the header for decompression\n");
            return NULL;
        }

        }
        u_data_pos += lenght;
        header = c_data[c_data_pos];
    }
    *uncompressed_data_size = u_data_pos;
    return u_data;
}

#define MY_BUILD_HEADER(command, lenght) (command << 5) + ((lenght) - 1)

typedef struct s_compression_piece compression_piece;

struct s_compression_piece {
    char 			command;
    unsigned int 		lenght;
    char			*argument;
    unsigned int		argument_lenght;
    compression_piece	*next;
};


compression_piece*	new_compression_piece(const char command, const unsigned int lenght, const char* args, const unsigned int argument_lenght)
{
    compression_piece* toret = (compression_piece*) malloc(sizeof(compression_piece));
    toret->command = command;
    toret->lenght = lenght;
    toret->argument = (char*) malloc(argument_lenght);
    memcpy(toret->argument, args, argument_lenght);
    toret->argument_lenght = argument_lenght;
    toret->next = NULL;
    return toret;
}

// Merge consecutive copy if possible
compression_piece*	merge_copy(compression_piece* start)
{
  compression_piece* piece = start;
  
  while (piece != NULL)
  {
    if (piece->command == D_CMD_COPY && piece->next != NULL && piece->next->command == D_CMD_COPY)
    {
      if (piece->lenght + piece->next->lenght <= D_MAX_LENGHT)
      {
	unsigned int previous_lenght = piece->lenght;
	piece->lenght = piece->lenght + piece->next->lenght;
	piece->argument = realloc(piece->argument, piece->lenght);
	memcpy(piece->argument + previous_lenght, piece->next->argument, piece->next->argument_lenght);
	piece->next = piece->next->next;
	continue; // Next could be another copy
      }
    }
    piece = piece->next;
  }
  return start;
}

unsigned int	create_compression_string(compression_piece* start, char *output)
{
  unsigned int pos = 0;
  compression_piece*	piece = start;
  
  while (piece != NULL)
  {
    if (piece->lenght < D_MAX_NORMAL_LENGHT) // Normal header
    {
      output[pos++] = MY_BUILD_HEADER(piece->command, piece->lenght);
    } else {
      if (piece->lenght <= D_MAX_LENGHT)
      {
	output[pos++] = MY_BUILD_HEADER(7, (piece->lenght >> 8) & 3);
	output[pos++] = (char) (piece->lenght & 0x00FF);
      } else { //We need to split the command
      }
    }
    memcpy(output + pos, piece->argument, piece->argument_lenght);
    pos += piece->argument_lenght;
    piece = piece->next;
  }
  output[pos] = 0xFF;
  return pos + 1;
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

void	print_compression_piece(compression_piece* piece)
{
  printf("Command : %d\n", piece->command);
  printf("Lenght  : %d\n", piece->lenght);
  printf("Argument lenght : %d\n", piece->argument_lenght);
  printf("Argument :%s\n", hexString(piece->argument, piece->argument_lenght));
}

// TODO TEST compressed data border for each cmd

char*	compress(const char* u_data, const size_t start, const unsigned int lenght, unsigned int* compressed_size)
{
    // we will realloc later
    printf("==Compressing %s ==\n", hexString(u_data + start, lenght));
    char* compressed_data = (char*) malloc(lenght);
    compression_piece* compressed_chain = new_compression_piece(42, 42, "aa", 42);
    compression_piece* compressed_chain_start = compressed_chain;

    unsigned int u_data_pos = start;
    unsigned int max_pos = start + lenght;
    //unsigned int previous_start = start;
    unsigned int data_size_taken[5] = {0, 0, 0, 0, 0};
    unsigned int cmd_size[5] = {0, 1, 2, 1, 2};
    char cmd_args[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    unsigned int bytes_since_last_compression = 0; // Used when skipping using copy

    while (1) {
        memset(data_size_taken, 0, 10);
        memset(cmd_args, 0, 10);
	printf("Testing every command\n");

        /* We test every command to see the gain with current position */
        {   //BYTE REPEAT
	  printf("Testing byte repeat\n");
            unsigned int pos = u_data_pos;
            char byte_to_repeat = u_data[pos];
            while (u_data[pos] == byte_to_repeat && pos < max_pos)
	    {
                data_size_taken[D_CMD_BYTE_REPEAT]++;
		pos++;
	    }
            cmd_args[D_CMD_BYTE_REPEAT][0] = byte_to_repeat;
        }
        {   // ALTERNATING BYTE
	  printf("Testing alternating byte\n");
            unsigned int pos = u_data_pos;
            char byte1 = u_data[pos];
            char byte2 = u_data[++pos];
            data_size_taken[D_CMD_ALTERNATE_BYTE] = 2;
            while (pos < max_pos) {
                if (u_data[++pos] == byte1)
                    data_size_taken[D_CMD_ALTERNATE_BYTE]++;
                else
                    break;
                if (pos < max_pos && u_data[++pos] == byte2)
                    data_size_taken[D_CMD_ALTERNATE_BYTE]++;
                else
                    break;
            }
            cmd_args[D_CMD_ALTERNATE_BYTE][0] = byte1;
            cmd_args[D_CMD_ALTERNATE_BYTE][1] = byte2;
        }
        {   // INC BYTE
	  printf("Testing byte inc\n");
            unsigned int pos = u_data_pos;
            char byte = u_data[pos];
	    data_size_taken[D_CMD_BYTE_INC] = 1;
            while (pos < max_pos && ++byte == u_data[++pos])
                data_size_taken[D_CMD_BYTE_INC]++;
            cmd_args[D_CMD_BYTE_INC][0] = u_data[u_data_pos];
        }
        {   // INTRA CPY
	    printf("Testing intra copy\n");
            if (u_data_pos != start)
            {
                unsigned int start_pos = start;
                //unsigned int compressed_lenght = u_data_pos - start;
                unsigned int current_pos_u = u_data_pos;
                unsigned int copied_size = 0;
                unsigned int start_data = start;

                while (start_pos < u_data_pos)
                {
		    while (u_data[current_pos_u] != u_data[start_pos] && start_pos < u_data_pos)
                        start_pos++;
                    start_data = start_pos;
                    while (u_data[current_pos_u] == u_data[start_pos] && start_pos < u_data_pos)
                    {
                        copied_size++;
                        current_pos_u++;
                        start_pos++;
                    }
                    if (copied_size > data_size_taken[D_CMD_COPY_EXISTING])
                    {
		        printf("-Found repeat of %d\n", copied_size);
                        data_size_taken[D_CMD_COPY_EXISTING] = copied_size;
                        cmd_args[D_CMD_COPY_EXISTING][0] = start_data & 0xFF;
                        cmd_args[D_CMD_COPY_EXISTING][1] = start_data >> 8;
                    }
                    copied_size = 0;
                }

            }
        }
	printf("Finding the best gain\n");
        // We check if a command managed to pick up 2 or more bytes
        unsigned int max_win = 1;
        unsigned int cmd_with_max = D_CMD_COPY;
        for (unsigned int cmd_i = 1; cmd_i < 5; cmd_i++)
        {
            unsigned int cmd_size_taken = data_size_taken[cmd_i];
            if (cmd_size_taken > max_win && cmd_size_taken > cmd_size[cmd_i])
            {
	        printf("--C:%d / S:%d\n", cmd_i, cmd_size_taken);
                cmd_with_max = cmd_i;
                max_win = cmd_size_taken;
            }
        }
        if (cmd_with_max == D_CMD_COPY) // This is the worse case
        {
	    printf("- Best command is copy\n");
            // We just move through the next byte and don't 'compress' yet, maybe something is better after.
            u_data_pos++;
            bytes_since_last_compression++;
            if (bytes_since_last_compression == 32 || u_data_pos == max_pos) // Arbitraty choice to do a 32 bytes grouping
            {
                char buffer[32];
                memcpy(buffer, u_data + u_data_pos - bytes_since_last_compression, bytes_since_last_compression);
                compression_piece* new_comp_piece = new_compression_piece(D_CMD_COPY, bytes_since_last_compression, buffer, bytes_since_last_compression);
                compressed_chain->next = new_comp_piece;
                compressed_chain = new_comp_piece;
                bytes_since_last_compression = 0;
            }
        }
        else { // Yay we get something better
	    printf("- Ok we get a gain from %d\n", cmd_with_max);
            char buffer[2];
            buffer[0] = cmd_args[cmd_with_max][0];
            if (cmd_size[cmd_with_max] == 2)
                buffer[1] = cmd_args[cmd_with_max][1];
            compression_piece* new_comp_piece = new_compression_piece(cmd_with_max, max_win, buffer, cmd_size[cmd_with_max]);
            if (bytes_since_last_compression != 0) // If we let non compressed stuff, we need to add a copy chuck before
            {
                char* copy_buff = (char*) malloc(bytes_since_last_compression);
                memcpy(copy_buff, u_data + u_data_pos - bytes_since_last_compression, bytes_since_last_compression);
                compression_piece* copy_chuck = new_compression_piece(D_CMD_COPY, bytes_since_last_compression, copy_buff, bytes_since_last_compression);
                compressed_chain->next = copy_chuck;
                compressed_chain = copy_chuck;
            }
            compressed_chain->next = new_comp_piece;
            compressed_chain = new_comp_piece;
            u_data_pos += max_win;
            bytes_since_last_compression = 0;
        }
        if (u_data_pos >= max_pos)
	  break;
	//sleep(1);
    }
    compressed_chain = compressed_chain_start->next;
    while (compressed_chain != NULL)
    {
      printf("-Piece-\n");
      print_compression_piece(compressed_chain);
      compressed_chain = compressed_chain->next;
    }
    // First is a dumb place holder
    merge_copy(compressed_chain_start->next);
    *compressed_size = create_compression_string(compressed_chain_start->next, compressed_data);
    return compressed_data;
}
