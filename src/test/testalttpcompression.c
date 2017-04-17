/*
 * Copyright 2016 - Sylvain "Skarsnik" Colinet
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CuTest.h"

#include "compressions/alttpcompression.h"


#define BUILD_HEADER(command, lenght) (command << 5) + (lenght - 1)


void	testValidCommandDecompress(CuTest* tc)
{
    unsigned int size;

    char simplecopy_i[4] = {BUILD_HEADER(0, 2), 42, 69, 0xFF};
    char simplecopy_o[2] = {42, 69};
    CuAssertDataEquals_Msg(tc, "Simple copy",
		       simplecopy_o, 2, decompress(simplecopy_i, 0, &size));
    CuAssertIntEquals(tc, 2, size);

    char simpleset_i[4] = {BUILD_HEADER(1, 2), 42, 0xFF};
    char simpleset_o[2] = {42, 42};
    CuAssertDataEquals_Msg(tc, "Simple Set",
		       simpleset_o, 2, decompress(simpleset_i, 0, &size));
    CuAssertIntEquals(tc, 2, size);

    //Command 2
    char simplecmd2_i[4] = {BUILD_HEADER(2, 6), 42, 69, 0xFF};
    char simplecmd2_o[6] = {42, 69, 42, 69, 42, 69};
    CuAssertDataEquals_Msg(tc, "Simple command 2 (ABAB..)",
		       simplecmd2_o, 6, decompress(simplecmd2_i, 0, &size));
    CuAssertIntEquals(tc, 6, size);

    // Command 2 with uneven size
    char simplecmd2_p_i[4] = {BUILD_HEADER(2, 7), 42, 69, 0xFF};
    char simplecmd2_p_o[7] = {42, 69, 42, 69, 42, 69, 42};
    CuAssertDataEquals_Msg(tc, "Simple command 2 uneven size",
		       simplecmd2_p_o, 7, decompress(simplecmd2_p_i, 0, &size));
    CuAssertIntEquals(tc, 7, size);

    //Command 3
    char simplecmd3_i[3] = {BUILD_HEADER(3, 5), 42, 0xFF};
    char simplecmd3_o[7] = {42, 43, 44, 45, 46};
    CuAssertDataEquals_Msg(tc, "Simple command 3 inc (A4->ABCD)",
		       simplecmd3_o, 5, decompress(simplecmd3_i, 0, &size));
    CuAssertIntEquals(tc, 5, size);

    //Command 4
    char simplecmd4_i[9] = {BUILD_HEADER(0, 4), 1, 2, 42, 69, BUILD_HEADER(4, 3), 01, 00, 0xFF};
    char simplecmd4_o[7] = {1, 2, 42, 69, 2, 42, 69};
    CuAssertDataEquals_Msg(tc, "Simple command 4, recopy data from u data",
			   simplecmd4_o, 7, decompress(simplecmd4_i, 0, &size));
    CuAssertIntEquals(tc, 7, size);
}


void	testMixingCommand(CuTest* tc)
{
    unsigned int size;
    
    char random1_i[11] = {BUILD_HEADER(1, 3), 42, BUILD_HEADER(0, 4), 1, 2, 3, 4, BUILD_HEADER(2, 3), 11, 22, 0xFF};
    char random1_o[10] = {42, 42, 42, 1, 2 , 3, 4, 11, 22, 11};
    CuAssertDataEquals_Msg(tc, "Mixing command (0, 1, 2)",
			   random1_o, 10, decompress(random1_i, 0, &size));
    CuAssertIntEquals(tc, 10, size);
}

void	testExtendedHeaderDecompress(CuTest* tc)
{
    unsigned int size;
    // Set 200 bytes to 42
    char	extendedcmd_i[4] = {0b11100100, 199, 42, 0xFF};
    char  *extendedcmd_o = malloc(200);
    for (int i = 0; i < 200; i++) {
        extendedcmd_o[i] = 42;
    }
    CuAssertDataEquals_Msg(tc, "Extended header test, test set size 200",
			   extendedcmd_o, 200, decompress(extendedcmd_i, 0, &size));
    CuAssertIntEquals(tc, 200, size);
    free(extendedcmd_o);

    char extendedcmd2_i[] = {0b11100101, 0x90, 42, 0xFF};
    extendedcmd_o = malloc(400);
    for (int i = 0; i < 400; i++) {
      extendedcmd_o[i] = 42;
    }
    CuAssertDataEquals_Msg(tc, "Extended header test, test set size 400",
			   extendedcmd_o, 400, decompress(extendedcmd2_i, 0, &size));
    
}


void	testCompressionSingle(CuTest* tc)
{
  unsigned int compress_size;
  char single_set[5] = {42, 42, 42, 42, 42};
  char single_set_expected[3] = {BUILD_HEADER(1, 5), 42, 0xFF};
  
  CuAssertDataEquals_Msg(tc, "Single compression, single set",
			    single_set_expected, 3, compress(single_set, 0, 5, &compress_size));
  CuAssertIntEquals(tc, 3, compress_size);

  char single_alternate[7] = {42, 1, 42, 1, 42, 1, 42};
  char single_alternate_expected[4] = {BUILD_HEADER(2, 7), 42, 1, 0xFF};
  CuAssertDataEquals_Msg(tc, "Single compression, alternating byte",
			    single_alternate_expected, 4, compress(single_alternate, 0, 7, &compress_size));
  
  char single_inc[3] = {1, 2, 3};
  char single_inc_expected[3] = {BUILD_HEADER(3, 3), 1, 0xFF};
  CuAssertDataEquals_Msg(tc, "Single compression, increasing byte",
			    single_inc_expected, 3, compress(single_inc, 0, 3, &compress_size));
  
  char single_copy[4] = {3, 10, 7, 20};
  char single_copy_expected[6] = {BUILD_HEADER(0, 4), 3, 10, 7, 20, 0xFF};
  CuAssertDataEquals_Msg(tc, "Single compression, direct copy",
			    single_copy_expected, 6, compress(single_copy, 0, 4, &compress_size));
  
  char single_copy_repeat[8] = {3, 10, 7, 20, 3, 10, 7, 20};
  char single_copy_repeat_expected[9] = {BUILD_HEADER(0, 4), 3, 10, 7, 20, BUILD_HEADER(4, 4), 0, 0, 0xFF};
  CuAssertDataEquals_Msg(tc, "Single compression, direct copy",
			    single_copy_repeat_expected, 9, compress(single_copy_repeat, 0, 8, &compress_size));
  
 
}

void	testSimpleMixCompression(CuTest* tc)
{
  unsigned int compress_size;
  char to_compress_string[] = {5, 5, 5, 5, 6, 7, 8, 9, 10, 11, 5, 2, 5, 2, 5, 2, 10, 11, 5, 2, 5, 2, 5, 2, 8, 10, 0, 5};
  
  char repeat_and_inc_copy_expected[] = {BUILD_HEADER(1, 4), 5, BUILD_HEADER(3, 6), 6, BUILD_HEADER(0, 1), 5, 0xFF};
  CuAssertDataEquals_Msg(tc, "Mixing, repeat, inc, trailing copy",
			    repeat_and_inc_copy_expected, 7, compress(to_compress_string, 0, 11, &compress_size));
  char inc_alternate_intra_copy_expected[] = {BUILD_HEADER(3, 7), 5, BUILD_HEADER(2, 6), 5, 2, BUILD_HEADER(4, 8), 06, 00, 0xFF};
  CuAssertDataEquals_Msg(tc, "Mixing, inc, alternate, intra copy",
			    inc_alternate_intra_copy_expected, 7, compress(to_compress_string, 3, 21, &compress_size));
  
  
}

CuSuite* StrUtilGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, testValidCommandDecompress);
    SUITE_ADD_TEST(suite, testExtendedHeaderDecompress);
    SUITE_ADD_TEST(suite, testMixingCommand);
    SUITE_ADD_TEST(suite, testCompressionSingle);
    SUITE_ADD_TEST(suite, testSimpleMixCompression);
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
