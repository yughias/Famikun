#ifndef __JOYPAD_H__
#define __JOYPAD_H__

#include "types.h"

typedef struct joypad_t {
    bool strobe;
    u8 controller_1_shifter;
} joypad_t;

u8 joypad_read_1(joypad_t* joypad);
u8 joypad_read_2(joypad_t* joypad);
void joypad_write(joypad_t* joypad, u8 byte);
void joypad_init();

#endif