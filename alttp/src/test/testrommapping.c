#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CuTest.h"
#include "rommapping.h"


typedef struct {
  int snes_addr;
  int pc_addr;
} addr_test_t;


addr_test_t tab_lorom[] = {
  {0x008200, 0x200},
  {0x2FA451, 0x17A451},
  {0x5B8001, 0x2D8001},
  {0x7D8001, 0x3E8001}
};

addr_test_t tab_lorom_mirror[] = {
  {0x5B0001, 0x2D8001},
  {0x808200, 0x200},
  {0xAFA451, 0x17A451},
  {0xDB8001, 0x2D8001},
  {0xDB0001, 0x2D8001},
  {0xFD8001, 0x3E8001}
};

void	testLoROMToPC(CuTest* tc)
{
  
  
    for (int i = 0; i < 4; i++)
    {
      char *tmp = NULL;
      asprintf(&tmp, "LoROM: Simple test %02X:%04X -> %X", tab_lorom[i].snes_addr >> 16, tab_lorom[i].snes_addr & 0x00FFFF, tab_lorom[i].pc_addr);
      CuAssertIntEquals_Msg(tc, tmp, tab_lorom[i].pc_addr, rommapping_snes_to_pc(tab_lorom[i].snes_addr, LoROM, false));
    }
    
    for (int i = 0; i < 6; i++)
    {
      char *tmp = NULL;
      asprintf(&tmp, "LoROM: Mirroring test %02X:%04X -> %X", tab_lorom_mirror[i].snes_addr >> 16, tab_lorom_mirror[i].snes_addr & 0x00FFFF, tab_lorom_mirror[i].pc_addr);
      CuAssertIntEquals_Msg(tc, tmp, tab_lorom_mirror[i].pc_addr, rommapping_snes_to_pc(tab_lorom_mirror[i].snes_addr, LoROM, false));
    }
    
    
    return;

    // Extra

    CuAssertIntEquals_Msg(tc, "LoROM: Special case FF:8001 -> 3F8001",
                          0x3F8001, rommapping_snes_to_pc(0xFF8001, LoROM, false));
    CuAssertIntEquals_Msg(tc, "LoROM: Special case FE:8001 -> 3F0001",
                          0x3F0001, rommapping_snes_to_pc(0xFE8001, LoROM, false));

}

void	testLoROMToPCErrors(CuTest *tc)
{
    CuAssertIntEquals_Msg(tc, "LoROM: Errors 01:4000 -> Reserved",
                          ROMMAPPING_LOCATION_SNES_RESERVED, rommapping_snes_to_pc(0x014000, LoROM, false));
    CuAssertIntEquals_Msg(tc, "LoROM: Errors 71:4000 -> SRAM",
                          ROMMAPPING_LOCATION_SRAM, rommapping_snes_to_pc(0x714000, LoROM, false));
    CuAssertIntEquals_Msg(tc, "LoROM: Errors FE:4000 -> SRAM",
                          ROMMAPPING_LOCATION_SRAM, rommapping_snes_to_pc(0xFE4000, LoROM, false));
    CuAssertIntEquals_Msg(tc, "LoROM: Errors 7E:8000 -> WRAM",
                          ROMMAPPING_LOCATION_WRAM, rommapping_snes_to_pc(0x7E8000, LoROM, false));
}

void	testPCToLoROM(CuTest *tc)
{
  for (int i = 0; i < 4; i++)
    {
      char *tmp = NULL;
      asprintf(&tmp, "Pc to LoROM: Simple test %X -> %02X:%04X", tab_lorom[i].pc_addr, tab_lorom[i].snes_addr >> 16, tab_lorom[i].snes_addr & 0x00FFFF);
      CuAssertIntEquals_Msg(tc, tmp, tab_lorom[i].snes_addr, rommapping_pc_to_snes(tab_lorom[i].pc_addr, LoROM, false));
    }
}

CuSuite* StrUtilGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, testLoROMToPC);
    SUITE_ADD_TEST(suite, testLoROMToPCErrors);
    SUITE_ADD_TEST(suite, testPCToLoROM);
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
