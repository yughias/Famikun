#ifndef __NES_H__
#define __NES_H__

#include "types.h"
#include "ines.h"
#include "m6502.h"
#include "ppu.h"
#include "memory.h"
#include "joypad.h"
#include "dma.h"
#include "apu.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define REFRESH_RATE 60.0988

#define RENDER_SCANLINES 240
#define SCANLINES 262
#define PPU_CYCLES_PER_SCANLINE 341
#define PPU_CYCLES_PER_FRAME (SCANLINES*PPU_CYCLES_PER_SCANLINE)

typedef struct nes_t {
    m6502_t cpu;
    ppu_t ppu;
    apu_t apu;
    ines_t cart;
    joypad_t joypad;
    dma_t dma;
    u8 ram[RAM_SIZE];
    bool cart_irq;
    void* mapper;
    size_t mapper_size;
} nes_t;

void nes_init(nes_t* nes, const char* filename);
void nes_reset(nes_t* nes);
void nes_run_frame(nes_t* nes);
void nes_sync(nes_t* nes);

#endif