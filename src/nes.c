#include "nes.h"
#include "memory.h"

#include "mappers.h"

#include <string.h>

typedef void (*mapper_init_func)(nes_t*);

#define GET_IRQ(n) (n->cart_irq || n->apu.frame_irq || n->apu.dmc.irq)

void nes_init(nes_t* nes, const char* filename){
    SDL_AudioDeviceID apu_dev = nes->apu.audioDev;
    memset(nes, 0, sizeof(nes_t));
    nes->apu.audioDev = apu_dev;
    ines_load(&nes->cart, filename);
    nes->ppu.vram_size = nes->cart.vram_align == VRAM_4 ? EXTENDED_VRAM_SIZE : BASIC_VRAM_SIZE;
    nes->ppu.vram = malloc(nes->ppu.vram_size);
    m6502_t* cpu = &nes->cpu;
    m6502_init(cpu);
    cpu->ctx = (void*)nes;
    ppu_t* ppu = &nes->ppu;
    ppu->ctx = (void*)nes;
    apu_t* apu = &nes->apu;
    apu->sequencer_mode = 4;
    apu->noise.lfsr = 1;
    apu->dmc.silence_flag = true;
    apu->dmc.empty = true;
    apu->ctx = (void*)nes;

    mapper_t* mapper = &mappers[nes->cart.mapper];
    if(!mapper->supported){
        printf("mapper %d not supported\n", nes->cart.mapper);
        exit(EXIT_FAILURE);
    }

    mapper_init_func mapper_init = (mapper_init_func)mapper->mapper_init;
    (*mapper_init)(nes);
    cpu->read = (m6502_read_func)mapper->cpu_read;
    cpu->write = (m6502_write_func)mapper->cpu_write;
    ppu->read = (ppu_read_func)mapper->ppu_read;
    ppu->write = (ppu_write_func)mapper->ppu_write;

    m6502_reset(cpu);
}

void nes_reset(nes_t* nes){
    m6502_init(&nes->cpu);
    m6502_reset(&nes->cpu);

    if(nes->mapper){
        free(nes->mapper);
        mapper_t* mapper = &mappers[nes->cart.mapper];
        mapper_init_func mapper_init = (mapper_init_func)mapper->mapper_init;
        (*mapper_init)(nes);
    }

    ppu_t* ppu = &nes->ppu;
    ppu_read_func ppu_read = ppu->read;
    ppu_write_func ppu_write = ppu->write;  
    u8* vram = ppu->vram;
    memset(ppu, 0, sizeof(ppu_t));
    ppu->vram = vram;
    ppu->vram_size = nes->cart.vram_align == VRAM_4 ? EXTENDED_VRAM_SIZE : BASIC_VRAM_SIZE;
    ppu->read = ppu_read;
    ppu->write = ppu_write;
    ppu->ctx = (void*)nes;
    apu_t* apu = &nes->apu;
    SDL_AudioDeviceID apu_dev = apu->audioDev;
    memset(apu, 0, sizeof(apu_t));
    apu->audioDev = apu_dev;
    apu->sequencer_mode = 4;
    apu->noise.lfsr = 1;
    apu->dmc.silence_flag = true;
    apu->dmc.empty = true;
    apu->ctx = (void*)nes;
}

void nes_run_frame(nes_t* nes){
    m6502_t* cpu = &nes->cpu;
    ppu_t* ppu = &nes->ppu;
    
    while(!ppu->end_of_frame){
        if(nes->dma.enabled){
            dma_t* dma = &nes->dma;
            if(dma->read_cycle){
                u16 addr = (dma->addr_hi << 8) | dma->addr_lo;
                dma->data = cpu->read(cpu, addr);
                cpu->cycles += 1;
                dma->read_cycle = false;
                dma->addr_lo += 1;
            } else {
                cpu->write(cpu, 0x2004, dma->data);
                cpu->cycles += 1;
                dma->read_cycle = true;
                dma->enabled =dma->addr_lo;
            }
        } else {
            if(ppu->nmi_pin){
                ppu->nmi_pin = false;
                m6502_nmi(cpu);
            } else if(GET_IRQ(nes) && m6502_interrupt_enabled(cpu)){
                m6502_irq(cpu);
            } else {
                m6502_step(cpu);
            }
        }
    }

    ppu->end_of_frame = false;
}

void nes_sync(nes_t* nes){
    apu_sync(&nes->apu);
    ppu_sync(&nes->ppu);
}