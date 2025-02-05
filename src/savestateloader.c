
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <puff.h>
#include <sys/stat.h>
#include "savestateloader.h"
#include "_savestate.h"
#include "snes9x_152.h"

#define windowBits 15
#define ENABLE_ZLIB_GZIP 16
#define GZIP_CHUNK_SIZE 0x4000
#define MY_ALLOC_CHUNK 0x100000

char*               data_from_gzip(const char* path, size_t* out_size)
{
    struct stat mstat;
    FILE* file = fopen(path, "rb");

    stat(path, &mstat);
    char *gzfile_data = (char*) malloc(mstat.st_size);
    fread(gzfile_data, 1, mstat.st_size, file);
    fclose(file);
    unsigned long uncompress_size;
    int pret = 0;
    pret = puff((unsigned char *)0, &uncompress_size, gzfile_data, &(mstat.st_size));
    if (pret != 0)
    {
        fprintf(stderr, "Error with puff : %d\n", pret);
        return NULL;
    }
    char* data = (char*) malloc(uncompress_size);
    puff(data, &uncompress_size, gzfile_data, &(mstat.st_size));
    return data;
/*    Zlib implementation
 * z_stream strm = {0};
    unsigned char in[GZIP_CHUNK_SIZE];
    unsigned char out[GZIP_CHUNK_SIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = in;
    strm.avail_in = 0;
    if (!inflateInit2(&strm, windowBits + ENABLE_ZLIB_GZIP))
        return NULL;
    char* data = (char*) malloc(MY_ALLOC_CHUNK);
    size_t allocated_size = MY_ALLOC_CHUNK;
    size_t data_pos = 0;

    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Error with open\n");
        return NULL;
    }
    while (1) {
        int bytes_read;
        int zlib_status;
        bytes_read = fread(in, 1, GZIP_CHUNK_SIZE, file);
        strm.avail_in = bytes_read;
        strm.next_in = in;
        do {
            fprintf(stderr, "Do looop\n");
            strm.avail_out = GZIP_CHUNK_SIZE;
            strm.next_out = out;
            zlib_status = inflate(&strm, Z_NO_FLUSH);
            switch (zlib_status) {
                case Z_OK:
                case Z_STREAM_END:
                case Z_BUF_ERROR:
                    break;
                default:
                    inflateEnd (& strm);
                    fprintf (stderr, "Gzip error %d in '%s'.\n", zlib_status, path);
                    return NULL;
            }
            unsigned int len = GZIP_CHUNK_SIZE - strm.avail_out;
            if (data_pos + len > allocated_size)
            {
                data = (char*) realloc(data, allocated_size + MY_ALLOC_CHUNK);
                allocated_size += MY_ALLOC_CHUNK;
            }
            memcpy(data + data_pos, out, len);
            data_pos += len;
        } while(strm.avail_out == 0);

        if (feof(file)) {
            inflateEnd (&strm);
            break;
        }
    }
    fclose(path);
    *out_size = data_pos;
    return data;*/
}


bool                savestate_loader_init(const char* file_path, const char* source_data, size_t source_data_lenght, savestate_t* savestate_info)
{
    savestate_internal_t* internal = (savestate_internal_t*) malloc(sizeof(savestate_internal_t));
    savestate_info->__internals = (void *) internal;
    internal->source_pos = 0;
    internal->file = NULL;
    internal->source_data = NULL;
    internal->public = savestate_info;
    savestate_info->section_count = 0;
    char data_start[20];
    if (source_data == NULL)
    {
        internal->file = fopen(file_path, "rb");
        if (internal->file == NULL)
            return false;
        fread(&data_start, 1, 20, internal->file);
    }
    if (file_path == NULL)
    {
        internal->source_data_lenght = source_data_lenght;
        internal->source_data = source_data;
        memcpy(data_start, source_data, 20);
    }
    // gzip data, let uncompress everything in memory
    // Not great, but less annoying that trying to use it like a IO* file
    if (data_start[0] == (char)0x1f && data_start[1] == (char)0x8b)
    {
        if (source_data == NULL)
        {
            internal->source_data = data_from_gzip(file_path, &internal->source_data_lenght);
            memcpy(data_start, internal->source_data, 20);
        }
    }
    printf("testing for Snes9x 152 savestate\n");
    if (savestate_snes9x_152_is_valid(data_start, 20))
    {
        printf("Snes9x 152 savestate\n");
        savestate_snes9x_152_init(internal);
        internal->get_section = &savestate_snes9x_152_get_section;
    } else {
        return false;
    }
    return true;
}

bool                savestate_loader_get_header(savestate_t* savestate_info)
{
    return true;
}

bool                savestate_loader_get_all(savestate_t* savestate_info)
{
    return true;
}

char**              savestate_loader_list_section_name(savestate_t* savestate_info)
{
    char** toret = (char**) malloc(sizeof(char*) * (savestate_info->section_count + 1));
    savestate_internal_t* internal = (savestate_internal_t*) savestate_info->__internals;
    section_list_node_t* list = internal->sections;
    unsigned int i = 0;
    while (list != NULL)
    {
        char* name = (char*) malloc(strlen(list->section.name) + 1);
        strcpy(name, list->section.name);
        toret[i] = name;
        i++;
        list = list->next;
    }
    toret[i] = NULL;
    return toret;
}

savestate_section_t*    savestate_loader_get(savestate_section section, savestate_t* savestate_info)
{
    //savestate_section_t* toret = (savestate_section_t*) malloc(sizeof(savestate_section_t));
    savestate_internal_t* internal = (savestate_internal_t*) savestate_info->__internals;
    return internal->get_section(section, internal);
}
