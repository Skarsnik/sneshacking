
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "snes9x_152.h"

#define MY_DEBUG 1
#ifdef MY_DEBUG
#define s_debug(...)  printf(__VA_ARGS__)
#else
#define s_debug(...)
#endif

#include <stdint.h>

typedef uint16_t uint16;
typedef int16_t int16;
typedef int8_t bool8;
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int64_t int64;
typedef intptr_t pint;
struct SOBJ
{
    int16	HPos;
    uint16	VPos;
    uint8	HFlip;
    uint8	VFlip;
    uint16	Name;
    uint8	Priority;
    uint8	Palette;
    uint8	Size;
};

struct SPPU
{
    struct
    {
        bool8	High;
        uint8	Increment;
        uint16	Address;
        uint16	Mask1;
        uint16	FullGraphicCount;
        uint16	Shift;
    }	VMA;

    uint32	WRAM;

    struct
    {
        uint16	SCBase;
        uint16	HOffset;
        uint16	VOffset;
        uint8	BGSize;
        uint16	NameBase;
        uint16	SCSize;
    }	BG[4];

    uint8	BGMode;
    uint8	BG3Priority;

    bool8	CGFLIP;
    uint8	CGFLIPRead;
    uint8	CGADD;
    uint8	CGSavedByte;
    uint16	CGDATA[256];

    struct SOBJ OBJ[128];
    bool8	OBJThroughMain;
    bool8	OBJThroughSub;
    bool8	OBJAddition;
    uint16	OBJNameBase;
    uint16	OBJNameSelect;
    uint8	OBJSizeSelect;

    uint16	OAMAddr;
    uint16	SavedOAMAddr;
    uint8	OAMPriorityRotation;
    uint8	OAMFlip;
    uint8	OAMReadFlip;
    uint16	OAMTileAddress;
    uint16	OAMWriteRegister;
    uint8	OAMData[512 + 32];

    uint8	FirstSprite;
    uint8	LastSprite;
    uint8	RangeTimeOver;

    bool8	HTimerEnabled;
    bool8	VTimerEnabled;
    short	HTimerPosition;
    short	VTimerPosition;
    uint16	IRQHBeamPos;
    uint16	IRQVBeamPos;

    uint8	HBeamFlip;
    uint8	VBeamFlip;
    uint16	HBeamPosLatched;
    uint16	VBeamPosLatched;
    uint16	GunHLatch;
    uint16	GunVLatch;
    uint8	HVBeamCounterLatched;

    bool8	Mode7HFlip;
    bool8	Mode7VFlip;
    uint8	Mode7Repeat;
    short	MatrixA;
    short	MatrixB;
    short	MatrixC;
    short	MatrixD;
    short	CentreX;
    short	CentreY;
    short	M7HOFS;
    short	M7VOFS;

    uint8	Mosaic;
    uint8	MosaicStart;
    bool8	BGMosaic[4];

    uint8	Window1Left;
    uint8	Window1Right;
    uint8	Window2Left;
    uint8	Window2Right;
    bool8	RecomputeClipWindows;
    uint8	ClipCounts[6];
    uint8	ClipWindowOverlapLogic[6];
    uint8	ClipWindow1Enable[6];
    uint8	ClipWindow2Enable[6];
    bool8	ClipWindow1Inside[6];
    bool8	ClipWindow2Inside[6];

    bool8	ForcedBlanking;

    uint8	FixedColourRed;
    uint8	FixedColourGreen;
    uint8	FixedColourBlue;
    uint8	Brightness;
    uint16	ScreenHeight;

    bool8	Need16x8Mulitply;
    uint8	BGnxOFSbyte;
    uint8	M7byte;

    uint8	HDMA;
    uint8	HDMAEnded;

    uint8	OpenBus1;
    uint8	OpenBus2;

    uint16	VRAMReadBuffer;
};

/* From doc/snapshots.txt
Currently defined blocks (in order) are:

    Essential parts:
      NAM - ROM filename, from Memory.ROMFilename. 0-terminated string.
      CPU - struct SCPUState, CPU internal state variables.
      REG - struct SRegisters, emulated CPU registers.
      PPU - struct SPPU, PPU internal variables. Note that IPPU is never saved.
      DMA - struct SDMA, DMA/HDMA state variables.
      VRA - Memory.VRAM, 0x10000 bytes.
      RAM - Memory.RAM, 0x20000 bytes (WRAM).
      SRA - Memory.SRAM, 0x20000 bytes.
      FIL - Memory.FillRAM, 0x8000 bytes (register backing store).
      SND - All of sound emulated registers and state valiables.
      CTL - struct SControlSnapshot, controller emulation.
      TIM - struct STimings, variables about timings between emulated events.

    Optional parts:
      SFX - struct FxRegs_s, Super FX.
      SA1 - struct SSA1, SA1 internal state variables.
      SAR - struct SSA1Registers, SA1 emulated registers.
      DP1 - struct SDSP1, DSP-1.
      DP2 - struct SDSP2, DSP-2.
      DP4 - struct SDSP4, DSP-4.
      CX4 - Memory.C4RAM, 0x2000 bytes.
      ST0 - struct SST010, ST-010.
      OBC - struct SOBC1, OBC1 internal state variables.
      OBM - Memory.OBC1RAM, 0x2000 byts.
      S71 - struct SSPC7110Snapshot, SPC7110.
      SRT - struct SSRTCSnapshot, S-RTC internal state variables.
      CLK - struct SRTCData, S-RTC emulated registers.
      BSX - struct SBSX, BS-X.
      SHO - rendered SNES screen.
      MOV - struct SnapshotMovieInfo.
      MID - Some block of data the movie subsystem.
*/

typedef struct {
    savestate_section   section;
    char*               block_str;
} map_block_name_entry_t;

static map_block_name_entry_t enums_and_block_tab[] = 
{
    {CPU, "CPU"},
    {REGISTER, "REG"},
    {PPU, "PPU"},
    {DMA, "DMA"},
    {VRAM, "VRA"},
    {WRAM, "RAM"},
    {SRAM, "SRA"},
    {FILLRAM, "FIL"},
    {CUSTOM, NULL}
};

static struct SPPU* SNES9xPPU = NULL;

static section_list_node_t* add_ppu_extra_section(char* ppu_data, section_list_node_t* section, unsigned int size, unsigned int version);

static savestate_section   block_name_to_enum(const char* name)
{
    unsigned int cpt = 0;
    while (enums_and_block_tab[cpt].section != CUSTOM)
    {
        if (strcmp(enums_and_block_tab[cpt].block_str, name) == 0)
            return enums_and_block_tab[cpt].section;
        cpt++;
    }
    return CUSTOM;
}

//  example block desc : NAM:000019:

bool    savestate_snes9x_152_is_valid(const char* start, unsigned int size)
{
    size = size;
    return (strncmp(start, "#!s9xsnp:", strlen("#!s9xsnp:")) == 0);
}

bool    savestate_snes9x_152_init(savestate_internal_t* savestate)
{
    char section_name[4];
    unsigned int    size = 0;
    unsigned int    version = 0;
    int             scanf_ret = 0;

    section_name[3] = 0;
    savestate_seek(savestate, 0, SEEK_SET);
    savestate_scanf(savestate, "#!s9xsnp:%04d\n", 14, &version);
    savestate->sections = NULL;
    section_list_node_t* prev_node = NULL;
    savestate->public->header.emulator_version = (char*) malloc(strlen("1.5x") + 1);
    strcpy(savestate->public->header.emulator_version, "1.5x");
    savestate->public->header.emulator_name = (char*) malloc(strlen("Snes9x") + 1);
    strcpy(savestate->public->header.emulator_name, "Snes9x");
    while ((scanf_ret = savestate_scanf(savestate, "%3c:%06d:", strlen("NAM:000019:"), &section_name, &size)) == 2)
    {
        s_debug("Section name : %s - Pos : %06X -- %d\n", section_name, savestate_get_current_position(savestate), size);
        if (strcmp(section_name, "NAM") == 0)
        {
            savestate->public->header.rom_file = (char*) malloc(size + 1);
            savestate_read(savestate, savestate->public->header.rom_file, size);
            continue;
        }
        section_list_node_t* current_node = (section_list_node_t*) malloc(sizeof(section_list_node_t));
        if (savestate->sections == NULL)
            savestate->sections = current_node;
        else
            prev_node->next = current_node;
        current_node->next = NULL;
        current_node->section.section = block_name_to_enum(section_name);
        savestate_section_internal_t* section = &current_node->section;
        if (section->section == CUSTOM)
        {
            section->name = (char*) malloc(4);
            strncpy(section->name, section_name, 4);
        } else {
            const char* name = savestate_internal_get_enum_name(section->section);
            section->name = (char*) malloc(strlen(name) + 1);
            strncpy(section->name, name, strlen(name) + 1);
        }
        section->start = savestate_get_current_position(savestate);
        section->size = size;
        prev_node = current_node;
        print_section(section);
        // OAM and CGRAM are in the PPU section
        if (section->section == PPU)
        {
            s_debug("PPU section\n");
            char* ppu_data = (char*) malloc(section->size);
            savestate_read(savestate, ppu_data, size);
            prev_node = add_ppu_extra_section(ppu_data, current_node, size, version);
            savestate->public->section_count += 2;
        } else {
            savestate_seek(savestate, size, SEEK_CUR);
        }
        savestate->public->section_count++;
    }
    s_debug("scanf ret : %d\n", scanf_ret);
    return true;
}

savestate_section_t*    savestate_snes9x_152_get_section(savestate_section section, savestate_internal_t* internal)
{
    for (unsigned int i = 0; enums_and_block_tab[i].section != CUSTOM; i++)
    {
        if (section == CGRAM)
        {
            savestate_section_t* cg_section = (savestate_section_t*) malloc(sizeof(savestate_section_t*));
            cg_section->name = (char*) malloc(6);
            strncpy(cg_section->name, "CGRAM", 5);
            cg_section->size = 512;
            cg_section->data = (char*) malloc(512);
            memcpy(cg_section->data, SNES9xPPU->CGDATA, 512);
            return cg_section;
        }
        if (section == enums_and_block_tab[i].section)
        {
            return generic_get_section(section, internal);
        }
    }
    return NULL;
}
 



/* Copied from snes9x snapshot.cpp
 * */


#define SNAPSHOT_VERSION 11

extern uint16				SignExtend[2];

typedef struct
{
        int			offset;
        int			offset2;
        int			size;
        int			type;
        uint16		debuted_in;
        uint16		deleted_in;
        const char	*name;
}	FreezeData;

enum
{
        INT_V,
        uint8_ARRAY_V,
        uint16_ARRAY_V,
        uint32_ARRAY_V,
        uint8_INDIR_ARRAY_V,
        uint16_INDIR_ARRAY_V,
        uint32_INDIR_ARRAY_V,
        POINTER_V
};

#define COUNT(ARRAY)				(sizeof(ARRAY) / sizeof(ARRAY[0]))
#define Offset(field, structure)	((int) (((char *) (&(((structure) NULL)->field))) - ((char *) NULL)))
#define OFFSET(f)					Offset(f, STRUCT *)
#define DUMMY(f)					Offset(f, struct Obsolete *)
#define DELETED(f)					(-1)

#define INT_ENTRY(save_version_introduced, field) \
{ \
        OFFSET(field), \
        0, \
        sizeof(((STRUCT *) NULL)->field), \
        INT_V, \
        save_version_introduced, \
        9999, \
        #field \
}

#define ARRAY_ENTRY(save_version_introduced, field, count, elemType) \
{ \
        OFFSET(field), \
        0, \
        count, \
        elemType, \
        save_version_introduced, \
        9999, \
        #field \
}

#define POINTER_ENTRY(save_version_introduced, field, relativeToField) \
{ \
        OFFSET(field), \
        OFFSET(relativeToField), \
        4, \
        POINTER_V, \
        save_version_introduced, \
        9999, \
        #field \
}

#define OBSOLETE_INT_ENTRY(save_version_introduced, save_version_removed, field) \
{ \
        DUMMY(field), \
        0, \
        sizeof(((struct Obsolete *) NULL)->field), \
        INT_V, \
        save_version_introduced, \
        save_version_removed, \
        #field \
}

#define OBSOLETE_ARRAY_ENTRY(save_version_introduced, save_version_removed, field, count, elemType) \
{ \
        DUMMY(field), \
        0, \
        count, \
        elemType, \
        save_version_introduced, \
        save_version_removed, \
        #field \
}

#define OBSOLETE_POINTER_ENTRY(save_version_introduced, save_version_removed, field, relativeToField) \
{ \
        DUMMY(field), \
        DUMMY(relativeToField), \
        4, \
        POINTER_V, \
        save_version_introduced, \
        save_version_removed, \
        #field \
}

#define DELETED_INT_ENTRY(save_version_introduced, save_version_removed, field, size) \
{ \
        DELETED(field), \
        0, \
        size, \
        INT_V, \
        save_version_introduced, \
        save_version_removed, \
        #field \
}

#define DELETED_ARRAY_ENTRY(save_version_introduced, save_version_removed, field, count, elemType) \
{ \
        DELETED(field), \
        0, \
        count, \
        elemType, \
        save_version_introduced, \
        save_version_removed, \
        #field \
}

#define DELETED_POINTER_ENTRY(save_version_introduced, save_version_removed, field, relativeToField) \
{ \
        DELETED(field), \
        DELETED(relativeToField), \
        4, \
        POINTER_V, \
        save_version_introduced, \
        save_version_removed, \
        #field \
}


#undef STRUCT
#define STRUCT	struct SPPU

static struct Obsolete
{
    uint8	CPU_IRQActive;
}	Obsolete;

static FreezeData	SnapPPU[] =
{
        INT_ENTRY(6, VMA.High),
        INT_ENTRY(6, VMA.Increment),
        INT_ENTRY(6, VMA.Address),
        INT_ENTRY(6, VMA.Mask1),
        INT_ENTRY(6, VMA.FullGraphicCount),
        INT_ENTRY(6, VMA.Shift),
        INT_ENTRY(6, WRAM),
#define O(N) \
        INT_ENTRY(6, BG[N].SCBase), \
        INT_ENTRY(6, BG[N].HOffset), \
        INT_ENTRY(6, BG[N].VOffset), \
        INT_ENTRY(6, BG[N].BGSize), \
        INT_ENTRY(6, BG[N].NameBase), \
        INT_ENTRY(6, BG[N].SCSize)
        O(0), O(1), O(2), O(3),
#undef O
        INT_ENTRY(6, BGMode),
        INT_ENTRY(6, BG3Priority),
        INT_ENTRY(6, CGFLIP),
        INT_ENTRY(6, CGFLIPRead),
        INT_ENTRY(6, CGADD),
        INT_ENTRY(11, CGSavedByte),
        ARRAY_ENTRY(6, CGDATA, 256, uint16_ARRAY_V),
#define O(N) \
        INT_ENTRY(6, OBJ[N].HPos), \
        INT_ENTRY(6, OBJ[N].VPos), \
        INT_ENTRY(6, OBJ[N].HFlip), \
        INT_ENTRY(6, OBJ[N].VFlip), \
        INT_ENTRY(6, OBJ[N].Name), \
        INT_ENTRY(6, OBJ[N].Priority), \
        INT_ENTRY(6, OBJ[N].Palette), \
        INT_ENTRY(6, OBJ[N].Size)
        O(  0), O(  1), O(  2), O(  3), O(  4), O(  5), O(  6), O(  7),
        O(  8), O(  9), O( 10), O( 11), O( 12), O( 13), O( 14), O( 15),
        O( 16), O( 17), O( 18), O( 19), O( 20), O( 21), O( 22), O( 23),
        O( 24), O( 25), O( 26), O( 27), O( 28), O( 29), O( 30), O( 31),
        O( 32), O( 33), O( 34), O( 35), O( 36), O( 37), O( 38), O( 39),
        O( 40), O( 41), O( 42), O( 43), O( 44), O( 45), O( 46), O( 47),
        O( 48), O( 49), O( 50), O( 51), O( 52), O( 53), O( 54), O( 55),
        O( 56), O( 57), O( 58), O( 59), O( 60), O( 61), O( 62), O( 63),
        O( 64), O( 65), O( 66), O( 67), O( 68), O( 69), O( 70), O( 71),
        O( 72), O( 73), O( 74), O( 75), O( 76), O( 77), O( 78), O( 79),
        O( 80), O( 81), O( 82), O( 83), O( 84), O( 85), O( 86), O( 87),
        O( 88), O( 89), O( 90), O( 91), O( 92), O( 93), O( 94), O( 95),
        O( 96), O( 97), O( 98), O( 99), O(100), O(101), O(102), O(103),
        O(104), O(105), O(106), O(107), O(108), O(109), O(110), O(111),
        O(112), O(113), O(114), O(115), O(116), O(117), O(118), O(119),
        O(120), O(121), O(122), O(123), O(124), O(125), O(126), O(127),
#undef O
        INT_ENTRY(6, OBJThroughMain),
        INT_ENTRY(6, OBJThroughSub),
        INT_ENTRY(6, OBJAddition),
        INT_ENTRY(6, OBJNameBase),
        INT_ENTRY(6, OBJNameSelect),
        INT_ENTRY(6, OBJSizeSelect),
        INT_ENTRY(6, OAMAddr),
        INT_ENTRY(6, SavedOAMAddr),
        INT_ENTRY(6, OAMPriorityRotation),
        INT_ENTRY(6, OAMFlip),
        INT_ENTRY(6, OAMReadFlip),
        INT_ENTRY(6, OAMTileAddress),
        INT_ENTRY(6, OAMWriteRegister),
        ARRAY_ENTRY(6, OAMData, 512 + 32, uint8_ARRAY_V),
        INT_ENTRY(6, FirstSprite),
        INT_ENTRY(6, LastSprite),
        INT_ENTRY(6, HTimerEnabled),
        INT_ENTRY(6, VTimerEnabled),
        INT_ENTRY(6, HTimerPosition),
        INT_ENTRY(6, VTimerPosition),
        INT_ENTRY(6, IRQHBeamPos),
        INT_ENTRY(6, IRQVBeamPos),
        INT_ENTRY(6, HBeamFlip),
        INT_ENTRY(6, VBeamFlip),
        INT_ENTRY(6, HBeamPosLatched),
        INT_ENTRY(6, VBeamPosLatched),
        INT_ENTRY(6, GunHLatch),
        INT_ENTRY(6, GunVLatch),
        INT_ENTRY(6, HVBeamCounterLatched),
        INT_ENTRY(6, Mode7HFlip),
        INT_ENTRY(6, Mode7VFlip),
        INT_ENTRY(6, Mode7Repeat),
        INT_ENTRY(6, MatrixA),
        INT_ENTRY(6, MatrixB),
        INT_ENTRY(6, MatrixC),
        INT_ENTRY(6, MatrixD),
        INT_ENTRY(6, CentreX),
        INT_ENTRY(6, CentreY),
        INT_ENTRY(6, M7HOFS),
        INT_ENTRY(6, M7VOFS),
        INT_ENTRY(6, Mosaic),
        INT_ENTRY(6, MosaicStart),
        ARRAY_ENTRY(6, BGMosaic, 4, uint8_ARRAY_V),
        INT_ENTRY(6, Window1Left),
        INT_ENTRY(6, Window1Right),
        INT_ENTRY(6, Window2Left),
        INT_ENTRY(6, Window2Right),
        INT_ENTRY(6, RecomputeClipWindows),
#define O(N) \
        INT_ENTRY(6, ClipCounts[N]), \
        INT_ENTRY(6, ClipWindowOverlapLogic[N]), \
        INT_ENTRY(6, ClipWindow1Enable[N]), \
        INT_ENTRY(6, ClipWindow2Enable[N]), \
        INT_ENTRY(6, ClipWindow1Inside[N]), \
        INT_ENTRY(6, ClipWindow2Inside[N])
        O(0), O(1), O(2), O(3), O(4), O(5),
#undef O
        INT_ENTRY(6, ForcedBlanking),
        INT_ENTRY(6, FixedColourRed),
        INT_ENTRY(6, FixedColourGreen),
        INT_ENTRY(6, FixedColourBlue),
        INT_ENTRY(6, Brightness),
        INT_ENTRY(6, ScreenHeight),
        INT_ENTRY(6, Need16x8Mulitply),
        INT_ENTRY(6, BGnxOFSbyte),
        INT_ENTRY(6, M7byte),
        INT_ENTRY(6, HDMA),
        INT_ENTRY(6, HDMAEnded),
        INT_ENTRY(6, OpenBus1),
        INT_ENTRY(6, OpenBus2),
        INT_ENTRY(11, VRAMReadBuffer)
};

#undef STRUCT
//UnfreezeStructFromCopy(&PPU, SnapPPU, COUNT(SnapPPU), local_ppu, version);
static void UnfreezeStructFromCopy (void *sbase, FreezeData *fields, int num_fields, uint8 *block, int version)
{
    uint8	*ptr = block;
    uint16	word;
    uint32	dword;
    int64	qaword;
    uint8	*addr;
    void	*base;
    int		relativeAddr;
    int		i, j;

    for (i = 0; i < num_fields; i++)
    {
        if (version < fields[i].debuted_in || version >= fields[i].deleted_in)
            continue;

        base = (SNAPSHOT_VERSION >= fields[i].deleted_in) ? ((void *) &Obsolete) : sbase;
        addr = (uint8 *) base + fields[i].offset;

        if (fields[i].type == uint8_INDIR_ARRAY_V || fields[i].type == uint16_INDIR_ARRAY_V || fields[i].type == uint32_INDIR_ARRAY_V)
            addr = (uint8 *) (*((pint *) addr));

        switch (fields[i].type)
        {
            case INT_V:
            case POINTER_V:
                switch (fields[i].size)
                {
                    case 1:
                        if (fields[i].offset < 0)
                        {
                            ptr++;
                            break;
                        }

                        *(addr) = *ptr++;
                        break;

                    case 2:
                        if (fields[i].offset < 0)
                        {
                            ptr += 2;
                            break;
                        }

                        word  = *ptr++ << 8;
                        word |= *ptr++;
                        *((uint16 *) (addr)) = word;
                        break;

                    case 4:
                        if (fields[i].offset < 0)
                        {
                            ptr += 4;
                            break;
                        }

                        dword  = *ptr++ << 24;
                        dword |= *ptr++ << 16;
                        dword |= *ptr++ << 8;
                        dword |= *ptr++;
                        *((uint32 *) (addr)) = dword;
                        break;

                    case 8:
                        if (fields[i].offset < 0)
                        {
                            ptr += 8;
                            break;
                        }

                        qaword  = (int64) *ptr++ << 56;
                        qaword |= (int64) *ptr++ << 48;
                        qaword |= (int64) *ptr++ << 40;
                        qaword |= (int64) *ptr++ << 32;
                        qaword |= (int64) *ptr++ << 24;
                        qaword |= (int64) *ptr++ << 16;
                        qaword |= (int64) *ptr++ << 8;
                        qaword |= (int64) *ptr++;
                        *((int64 *) (addr)) = qaword;
                        break;

                    default:
                        assert(0);
                        break;
                }

                break;

            case uint8_ARRAY_V:
            case uint8_INDIR_ARRAY_V:
                if (fields[i].offset >= 0)
                    memmove(addr, ptr, fields[i].size);
                ptr += fields[i].size;

                break;

            case uint16_ARRAY_V:
            case uint16_INDIR_ARRAY_V:
                if (fields[i].offset < 0)
                {
                    ptr += fields[i].size * 2;
                    break;
                }

                for (j = 0; j < fields[i].size; j++)
                {
                    word  = *ptr++ << 8;
                    word |= *ptr++;
                    *((uint16 *) (addr + j * 2)) = word;
                }

                break;

            case uint32_ARRAY_V:
            case uint32_INDIR_ARRAY_V:
                if (fields[i].offset < 0)
                {
                    ptr += fields[i].size * 4;
                    break;
                }

                for (j = 0; j < fields[i].size; j++)
                {
                    dword  = *ptr++ << 24;
                    dword |= *ptr++ << 16;
                    dword |= *ptr++ << 8;
                    dword |= *ptr++;
                    *((uint32 *) (addr + j * 4)) = dword;
                }

                break;
        }

        if (fields[i].type == POINTER_V)
        {
            relativeAddr = (int) *((pint *) ((uint8 *) base + fields[i].offset));
            uint8	*relativeTo = (uint8 *) *((pint *) ((uint8 *) base + fields[i].offset2));
            *((pint *) (addr)) = (pint) (relativeTo + relativeAddr);
        }
    }
}

static section_list_node_t* add_ppu_extra_section(char* ppu_data, section_list_node_t* section, unsigned int size, unsigned int version)
{
    SNES9xPPU = (struct SPPU*) malloc(sizeof(struct SPPU));
    UnfreezeStructFromCopy(SNES9xPPU, SnapPPU, COUNT(SnapPPU), ppu_data, version);
    savestate_section_internal_t cgram_section;
    cgram_section.section = CGRAM;
    cgram_section.name = (char*) malloc(6);
    strncpy(cgram_section.name, "CGRAM", 5);
    cgram_section.size = 0;
    cgram_section.start = 0;
    savestate_section_internal_t oam_section;
    oam_section.section = CUSTOM;
    oam_section.name = (char*) malloc(4);
    strncpy(oam_section.name, "OAM", 3);
    oam_section.size = 0;
    oam_section.start = 0;
    section_list_node_t* cgram_node = (section_list_node_t*) malloc(sizeof(section_list_node_t));
    section_list_node_t* oam_node = (section_list_node_t*) malloc(sizeof(section_list_node_t));
    cgram_node->section = cgram_section;
    oam_node->section = oam_section;
    section->next = cgram_node;
    cgram_node->next = oam_node;
    return oam_node;
}
