#include "SDL_MAINLOOP.h"
#include "nes.h"
#include "menu.h"

int emulation_speed = 1;
bool pause;
char* rom_path;
nes_t nes;

SDL_Window* win_chr;
SDL_Window* win_nametable;
SDL_Window* win_palette;
SDL_Window* win_apu;
SDL_Window* win_oam;

void setup(){
    if(getArgc() != 2){
        menu_open_rom();
    } else {
        rom_path = getArgv(1);
    }

    char* base_path = SDL_GetBasePath();
    char logo_path[FILENAME_MAX];
    strcpy(logo_path, base_path);
    strcat(logo_path, "data/logo.bmp");
    SDL_free(base_path);

    setScaleMode(NEAREST);
    loadWindowIcon(logo_path);
    size(SCREEN_WIDTH, SCREEN_HEIGHT);
    setTitle(u8"ファミくん");
    frameRate(REFRESH_RATE);

    nes_init(&nes, rom_path);

    ines_t* ines = &nes.cart;
    printf("mapper: %d\n", ines->mapper);
    printf("vram align: %d\n", ines->vram_align);
    printf("trainer size: %zu\n", ines->trainer_size);
    printf("chr size: %zu\n", ines->chr_size);
    printf("prg size: %zu\n", ines->prg_size);
    printf("prg ram size: %zu\n", ines->prg_ram_size);

    joypad_init();

    SDL_AudioSpec audioSpec;
    memset(&audioSpec, 0, sizeof(SDL_AudioSpec));
    audioSpec.freq = SAMPLE_RATE;
    audioSpec.channels = 1;
    audioSpec.format = AUDIO_F32;
    nes.apu.audioDev = SDL_OpenAudioDevice(0, 0, &audioSpec, &audioSpec, 0);

    SDL_PauseAudioDevice(nes.apu.audioDev, 0);

    menu_init();

    noRender();
}

void loop(){
    #ifndef __EMSCRIPTEN__
    menu_update_shortcut();
    #endif

    nes.apu.push_rate_reload = PUSH_RATE_RELOAD*emulation_speed;

    for(int i = 0; i < emulation_speed && !pause; i++)
        nes_run_frame(&nes);

    #ifndef __EMSCRIPTEN__
    nes_t tmp = nes;
    ppu_draw_chr(&tmp.ppu, &win_chr);
    ppu_draw_nametables(&tmp.ppu, &win_nametable);
    ppu_draw_palettes(&tmp.ppu, &win_palette);
    ppu_draw_oam(&tmp.ppu, &win_oam);
    apu_draw_waves(&nes.apu, &win_apu);
    #endif
}