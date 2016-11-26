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
  
  char expected[] = {0, 1, 2, 3, 4, 5, 6, 7, 
		     7, 6, 5, 4, 3, 2, 1, 0,
		     2, 2, 2, 2, 2, 2, 2, 2,
		     2, 2, 7, 7, 7, 7, 2, 2,
		     2, 2, 2, 7, 7, 2, 2, 2,
		     2, 2, 2, 7, 7, 2, 2, 2,
		     2, 2, 2, 7, 7, 2, 2, 2,
		     2, 2, 2, 2, 2, 2, 2, 2,
  };
  CuAssertDataEquals_Msg(tc, "BPP3 simple test (using testsnestilebpp3.tl)",
			 expected, 64, tile.data);
}


CuSuite* StrUtilGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, testUnpackBPP3Tile);
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
