#ifndef __MAPPERS_H__
#define __MAPPERS_H__

#include "mappers/mapper0.h"

#include "mappers/mapper1.h"

#include "mappers/mapper2.h"

#include "mappers/mapper3.h"

#include "mappers/mapper4.h"

#include "mappers/mapper7.h"

#include "mappers/mapper9.h"

#include "mappers/mapper66.h"

#include "mappers/mapper71.h"

#include "mappers/mapper87.h"

typedef struct mapper_t {
	char  supported;
	void* mapper_init;
	void* cpu_read;
	void* cpu_write;
	void* ppu_read;
	void* ppu_write;
} mapper_t;

static mapper_t mappers[256] = {
	[0] = {1, mapper0_init, mapper0_cpu_read, mapper0_cpu_write, mapper0_ppu_read, mapper0_ppu_write},
	[1] = {1, mapper1_init, mapper1_cpu_read, mapper1_cpu_write, mapper1_ppu_read, mapper1_ppu_write},
	[2] = {1, mapper2_init, mapper2_cpu_read, mapper2_cpu_write, mapper2_ppu_read, mapper2_ppu_write},
	[3] = {1, mapper3_init, mapper3_cpu_read, mapper3_cpu_write, mapper3_ppu_read, mapper3_ppu_write},
	[4] = {1, mapper4_init, mapper4_cpu_read, mapper4_cpu_write, mapper4_ppu_read, mapper4_ppu_write},
	[7] = {1, mapper7_init, mapper7_cpu_read, mapper7_cpu_write, mapper7_ppu_read, mapper7_ppu_write},
	[9] = {1, mapper9_init, mapper9_cpu_read, mapper9_cpu_write, mapper9_ppu_read, mapper9_ppu_write},
	[66] = {1, mapper66_init, mapper66_cpu_read, mapper66_cpu_write, mapper66_ppu_read, mapper66_ppu_write},
	[71] = {1, mapper71_init, mapper71_cpu_read, mapper71_cpu_write, mapper71_ppu_read, mapper71_ppu_write},
	[87] = {1, mapper87_init, mapper87_cpu_read, mapper87_cpu_write, mapper87_ppu_read, mapper87_ppu_write},
};

#endif
