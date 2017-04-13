/*
 * Copyright 2016 - Sylvain "Skarsnik" Colinet
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "CuTest.h"
#include "tile.h"



const char testtiledatabpp2[] = {0, 1, 2, 3, 3, 2, 1, 0,
                       3, 2, 1, 0, 0, 1, 2, 3,
                       3, 2, 2, 1, 1, 2, 2, 3,
                       3, 2, 2, 1, 1, 2, 2, 3,
                       3, 2, 2, 1, 1, 2, 2, 3,
                       3, 2, 2, 1, 1, 2, 2, 3,
                       3, 2, 2, 1, 1, 2, 2, 3,
                       3, 3, 3, 3, 3, 3, 3, 3,
                      };

const char testtiledatabpp3[] = {0, 1, 2, 3, 4, 5, 6, 7,
                       7, 6, 5, 4, 3, 2, 1, 0,
                       2, 2, 2, 2, 2, 2, 2, 2,
                       2, 2, 7, 7, 7, 7, 2, 2,
                       2, 2, 2, 7, 7, 2, 2, 2,
                       2, 2, 2, 7, 7, 2, 2, 2,
                       2, 2, 2, 7, 7, 2, 2, 2,
                       2, 2, 2, 2, 2, 2, 2, 2,
                      };

const char testtiledatabpp4[] = {0, 1, 2, 3, 4, 5, 6, 7,
                       8, 9, 10, 11, 12, 13, 14, 15,
                       3, 3, 3, 3, 3, 3, 3, 3,
                       3, 9, 9, 3, 3, 12, 12, 3,
                       3, 9, 9, 3, 3, 12, 12, 3,
                       3, 9, 9, 3, 3, 12, 12, 3,
                       3, 9, 9, 3, 3, 12, 12, 3,
                       3, 3, 3, 3, 3, 3, 3, 3,
                      };


void	testUnpackBPP2Tile(CuTest* tc)
{
    char buffer[16];
    tile8	tile;

    int fd = open("testsnestilebpp2.tl", O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Can't open testsnestilebpp2.tl : %s\n", strerror(errno));
        return ;
    }
    read(fd, buffer, 16);
    tile = unpack_bpp2_tile(buffer, 0);

    CuAssertDataEquals_Msg(tc, "BPP2 simple test (using testsnestilebpp2.tl)",
                           testtiledatabpp2, 64, tile.data);
}

void testUnpackBPP3Tile(CuTest* tc)
{
    char buffer[24];
    tile8	tile;

    int fd = open("testsnestilebpp3.tl", O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Can't open testsnestilebpp3.tl : %s\n", strerror(errno));
        return ;
    }
    read(fd, buffer, 24);
    tile = unpack_bpp3_tile(buffer, 0);

    CuAssertDataEquals_Msg(tc, "BPP3 simple test (using testsnestilebpp3.tl)",
                           testtiledatabpp3, 64, tile.data);
}



void	testUnpackBPP4Tile(CuTest* tc)
{
    char buffer[32];
    tile8	tile;

    int fd = open("testsnestilebpp4.tl", O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Can't open testsnestilebpp4.tl : %s\n", strerror(errno));
        return ;
    }
    read(fd, buffer, 32);
    tile = unpack_bpp4_tile(buffer, 0);

    CuAssertDataEquals_Msg(tc, "BPP4 simple test (using testsnestilebpp4.tl)",
                           testtiledatabpp4, 64, tile.data);
}

void	testPackBPP2Tile(CuTest* tc)
{
    tile8	tile;
    char	buffer[16];
    memcpy(tile.data, testtiledatabpp2, 64);
    
    int fd = open("testsnestilebpp2.tl", O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Can't open testsnestilebpp2.tl : %s\n", strerror(errno));
        return ;
    }
    read(fd, buffer, 16);
    
    CuAssertDataEquals_Msg(tc, "packing BPP2 tile)",
                           buffer, 16, pack_bpp2_tile(tile));
}

void	testPackBPP3Tile(CuTest* tc)
{
    tile8	tile;
    char	buffer[24];
    memcpy(tile.data, testtiledatabpp3, 64);
    
    
    int fd = open("testsnestilebpp3.tl", O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Can't open testsnestilebpp3.tl : %s\n", strerror(errno));
        return ;
    }
    read(fd, buffer, 24);
    
    CuAssertDataEquals_Msg(tc, "packing BPP3 tile)",
                           buffer, 24, pack_bpp3_tile(tile));
}


void	testPackBPP4Tile(CuTest* tc)
{
    tile8	tile;
    char	buffer[32];
    memcpy(tile.data, testtiledatabpp4, 64);
    
    int fd = open("testsnestilebpp4.tl", O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Can't open testsnestilebpp4.tl : %s\n", strerror(errno));
        return ;
    }
    read(fd, buffer, 32);
    
    CuAssertDataEquals_Msg(tc, "packing BPP4 tile)",
                           buffer, 32, pack_bpp4_tile(tile));
}


CuSuite* StrUtilGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, testUnpackBPP2Tile);
    SUITE_ADD_TEST(suite, testUnpackBPP3Tile);
    SUITE_ADD_TEST(suite, testUnpackBPP4Tile);
    SUITE_ADD_TEST(suite, testPackBPP2Tile);
    SUITE_ADD_TEST(suite, testPackBPP3Tile);
    SUITE_ADD_TEST(suite, testPackBPP4Tile);
    return suite;
}


int	main(int ac, char* ag[])
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, StrUtilGetSuite());
    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return (suite->failCount != 0);
}
