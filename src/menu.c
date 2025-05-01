#include "menu.h"
#include "SDL_MAINLOOP.h"
#include "nes.h"
#include "tinyfiledialogs.h"

extern nes_t nes;
extern SDL_Window* win_chr;
extern SDL_Window* win_nametable;
extern SDL_Window* win_palette;
extern SDL_Window* win_apu;
extern SDL_Window* win_oam;
extern int emulation_speed;
extern bool pause;
extern char* rom_path;

static buttonId btn_sound[5]; 
static buttonId btn_pause;
static int actual_speed = 1;
static const int MAX_SPEED = 8;

#define KEY_RELEASED(x) (prev_ks[x] && !ks[x])
#define MUTE_FUNC(x) static void cb_sound_ ## x () { nes.apu.mute[x] ^= 1; tickButton(btn_sound[x], !nes.apu.mute[x]); } 
#define OPEN_FUNC(name, title, x, y) static void cb_open_ ## name () { Uint32 id = SDL_GetWindowID(win_ ## name); if(!id) win_ ## name = createWindowWithIcon(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x, y, 0);}
#define SPEED_FUNC(x) static void cb_speed_ ## x () { actual_speed = x; }

SPEED_FUNC(1)
SPEED_FUNC(2)
SPEED_FUNC(4)
SPEED_FUNC(8)

MUTE_FUNC(0)
MUTE_FUNC(1)
MUTE_FUNC(2)
MUTE_FUNC(3)
MUTE_FUNC(4)

OPEN_FUNC(nametable, "NAMETABLE", 32*8*2, 30*8*2)
OPEN_FUNC(chr, "TILESET", 32*8, 16*8)
OPEN_FUNC(palette, "PALETTE", 4*16, 8*16)
OPEN_FUNC(apu, "APU", 256*2, 256*3)
OPEN_FUNC(oam, "SPRITES", 8*32, 8*32)

static void cb_pause(){ pause ^= 1; tickButton(btn_pause, pause); }
static void cb_reset(){ nes_reset(&nes); }

void menu_init(){
    menuId menu_emulation = addMenuTo(-1, L"Emulation", false);

    addButtonTo(menu_emulation, L"Open\tCtrl+O", menu_open_rom);
    menuId btn_pause = addButtonTo(menu_emulation, L"Pause\tCtrl+P", cb_pause);
    addButtonTo(menu_emulation, L"Reset\tCtrl+R", cb_reset);

    menuId menu_speed = addMenuTo(menu_emulation, L"Speed", true);
    buttonId btn_speed1 = addButtonTo(menu_speed, L"x1", cb_speed_1);
    addButtonTo(menu_speed, L"x2", cb_speed_2);
    addButtonTo(menu_speed, L"x4", cb_speed_4);
    addButtonTo(menu_speed, L"x8", cb_speed_8);
    checkRadioButton(btn_speed1);

    menuId menu_sound = addMenuTo(-1, L"Sound", false);
    btn_sound[0] = addButtonTo(menu_sound, L"Square 1", cb_sound_0);
    btn_sound[1] = addButtonTo(menu_sound, L"Square 2", cb_sound_1);
    btn_sound[2] = addButtonTo(menu_sound, L"Triangle", cb_sound_2);
    btn_sound[3] = addButtonTo(menu_sound, L"Noise", cb_sound_3);
    btn_sound[4] = addButtonTo(menu_sound, L"Dmc", cb_sound_4);
    tickButton(btn_sound[0], true);
    tickButton(btn_sound[1], true);
    tickButton(btn_sound[2], true);
    tickButton(btn_sound[3], true);
    tickButton(btn_sound[4], true);
    
    menuId viewMenu = addMenuTo(-1, L"View", false);
    addButtonTo(viewMenu, L"Nametable", cb_open_nametable);
    addButtonTo(viewMenu, L"Tileset", cb_open_chr);
    addButtonTo(viewMenu, L"Palette", cb_open_palette);
    addButtonTo(viewMenu, L"Waveform", cb_open_apu);
    addButtonTo(viewMenu, L"Sprites", cb_open_oam);
}

void menu_update_shortcut(){
    static Uint8 prev_ks[SDL_NUM_SCANCODES];
    const Uint8* ks = SDL_GetKeyboardState(NULL);

    if(ks[SDL_SCANCODE_LCTRL] || ks[SDL_SCANCODE_RCTRL]){
        if(KEY_RELEASED(SDL_SCANCODE_1))
            cb_sound_0();
        if(KEY_RELEASED(SDL_SCANCODE_2))
            cb_sound_1();
        if(KEY_RELEASED(SDL_SCANCODE_3))
            cb_sound_2();
        if(KEY_RELEASED(SDL_SCANCODE_4))
            cb_sound_3();
        if(KEY_RELEASED(SDL_SCANCODE_5))
            cb_sound_4();
        if(KEY_RELEASED(SDL_SCANCODE_P))
            cb_pause();
        if(KEY_RELEASED(SDL_SCANCODE_R))
            cb_reset();
        if(KEY_RELEASED(SDL_SCANCODE_O))
            menu_open_rom();
    }

    emulation_speed = ks[SDL_SCANCODE_TAB] ? MAX_SPEED : actual_speed;

    memcpy(prev_ks, ks, SDL_NUM_SCANCODES);
}

void menu_open_rom(){
    bool nes_is_empty = !nes.cart.prg;

    char const* file_type[1] = { "*.nes" };

    rom_path = tinyfd_openFileDialog(
        "Select ROM",
        NULL,
        1,
        file_type,
        NULL,
        0
    );

    if(nes_is_empty && !rom_path){
        tinyfd_messageBox(
			"Loading Error",
			"Can not open ROM, quitting...",
			"ok",
			"error",
            1
        );
        exit(EXIT_FAILURE);
    }

    if(rom_path && !nes_is_empty){
        free(nes.mapper);
        free(nes.ppu.vram);
        free(nes.cart.prg);
        free(nes.cart.chr);
        free(nes.cart.trainer);
        free(nes.cart.prg_ram);
        nes_init(&nes, rom_path);
    }
}