#include <time.h>
#include "main.h"
#include "Opcodes.h"

const uint8_t sizeMult = 2;      // Window Size multiplier
const uint16_t gameWidth = 224;  // Native screen width
const uint16_t gameHeight = 256; // Native screen height

// Contains the number of cycles used per instruction
const uint8_t cycleList8080[] = {
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,   // 0x00..0x0f
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,   // 0x10..0x1f
    4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, // etc
    4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, // 0x40..0x4f
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, // 0x80..8x4f
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, // 0xc0..0xcf
    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
    11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
    11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11};

// Assumes rom folder "invaders_rom" is located in the same folder as the executable
void readRom(char *basePath, uint8_t *romBuf)
{
    char *tempPath = malloc(strlen(basePath) + 24);
    char partExt = 'h'; // The space invaders rom consists of 4 parts "h, g, f, e"
    for (int partNum = 0; partNum < 4; partNum++)
    {
        sprintf(tempPath, "%s%s%c", basePath, "invaders_rom\\invaders.", partExt - partNum);
        FILE *rom = fopen(tempPath, "rb");
        if (rom == NULL)
        {
            printf("invaders.%c not found", partExt - partNum);
            exit(0);
        }

        fseek(rom, 0L, SEEK_END);
        int fsize = ftell(rom);
        fseek(rom, 0L, SEEK_SET);
        fread((void *)(romBuf + 0x0800 * partNum), fsize, 1, rom);
        fclose(rom);
    }
    free(tempPath);
}

Mix_Chunk *sampleList[9];

// Assumes "sample" folder is located in the same folder as the executable
void LoadAudio()
{
    char *basePath = SDL_GetBasePath();
    char *tempPath = malloc(strlen(basePath) + 16);
    Mix_OpenAudio(48000, AUDIO_S16, 1, 2048);
    for (int sampleNum = 0; sampleNum < 9; sampleNum++)
    {
        sprintf(tempPath, "%s%s%i%s", basePath, "samples\\", sampleNum, ".wav");
        sampleList[sampleNum] = Mix_LoadWAV(tempPath);
    }
    free(tempPath);
    /*
    sampleList[0] - UFO Repeating
    sampleList[1] - Player Shot
    sampleList[2] - Player Die
    sampleList[3] - Invader Die
    sampleList[4] - Alien movement
    sampleList[5] - Alien movement
    sampleList[6] - Alien movement
    sampleList[7] - Alien movement
    sampleList[8] - UFO Hit*/
    Mix_MasterVolume(2); // Adjust Volume
}

uint8_t shiftResultOffset = 0;

void processInputs(uint8_t port)
{
    switch (port)
    {
    case 1:
        intel8080.a = (!keyState[SDL_SCANCODE_1] |
                       keyState[SDL_SCANCODE_2] << 2 |
                       1 << 3 |
                       keyState[SDL_SCANCODE_Q] << 4 |
                       keyState[SDL_SCANCODE_LEFT] << 5 |
                       keyState[SDL_SCANCODE_RIGHT] << 6);
        break;
    case 2:
        intel8080.a = (keyState[SDL_SCANCODE_GRAVE] << 2);
        break;
    case 3:
        intel8080.a = (intel8080.shiftReg >> (8 - shiftResultOffset)) & 0x00FF;
        break;
    }
}

uint8_t audEnable[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

void processOutputs(uint8_t port)
{
    switch (port)
    {
    case 2:
        shiftResultOffset = intel8080.a & 0x07;
        break;
    case 3:
        if ((intel8080.a & 0x01) && Mix_Playing(0) == 0) // UFO Repeating
        {
            Mix_PlayChannel(0, sampleList[0], 0);
        }

        if (intel8080.a & 0x02) // Shot
        {
            if (audEnable[1] == 1)
            {
                Mix_PlayChannel(1, sampleList[1], 0);
                audEnable[1] = 0;
            }
        }
        else
        {
            audEnable[1] = 1;
        }

        if (intel8080.a & 0x04) // Player Die
        {
            if (audEnable[2] == 1)
            {
                Mix_PlayChannel(2, sampleList[2], 0);
                audEnable[2] = 0;
            }
        }
        else
        {
            audEnable[2] = 1;
        }

        if (intel8080.a & 0x08) // Invader Die
        {
            if (audEnable[3] == 1)
            {
                Mix_PlayChannel(3, sampleList[3], 0);
                audEnable[3] = 0;
            }
        }
        else
        {
            audEnable[3] = 1;
        }
        break;
    case 4:
        intel8080.shiftReg >>= 8;
        intel8080.shiftReg += ((uint16_t)intel8080.a << 8);
        break;
    case 5:
        if (intel8080.a & 0x01) // Invader Movement
        {
            Mix_PlayChannel(4, sampleList[4], 0);
        }
        if (intel8080.a & 0x02) // Invader Movement
        {
            Mix_PlayChannel(5, sampleList[5], 0);
        }
        if (intel8080.a & 0x04) // Invader Movement
        {
            Mix_PlayChannel(6, sampleList[6], 0);
        }
        if (intel8080.a & 0x08) // Invader Movement
        {
            Mix_PlayChannel(7, sampleList[7], 0);
        }
        if (intel8080.a & 0x10) // UFO Hit
        {
            if (audEnable[8] == 1)
            {
                Mix_PlayChannel(0, sampleList[8], 0);
                audEnable[8] = 0;
            }
        }
        else
        {
            audEnable[8] = 1;
        }
        break;
    }
}

uint32_t *rotateBuf;

int main(int argc, char **argv)
{
    SDL_Window *window;
    SDL_Renderer *render;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_CreateWindowAndRenderer(gameWidth * sizeMult, gameHeight * sizeMult, 0, &window, &render);
    SDL_SetWindowTitle(window, "Space Invaders");
    keyState = SDL_GetKeyboardState(NULL);
    rotateBuf = malloc(gameHeight * gameWidth * __SIZEOF_INT__);

    SDL_Rect *textureLoc; // Determines where the texture is copied to the screen
    textureLoc = malloc(sizeof(SDL_Rect));
    textureLoc->h = gameWidth * sizeMult;
    textureLoc->w = gameHeight * sizeMult;
    textureLoc->x = -16 * sizeMult; // 16 pixel adjustment to the left after rotating
    textureLoc->y = 16 * sizeMult;  // 16 pixel adjustment downward after rotating

    // Allocate for and read rom
    uint8_t *romBuf = calloc(1, 0x4000);
    readRom(SDL_GetBasePath(), romBuf);

    uint32_t cycleCounter = 0, interruptStage = 0; // 0 for half frame interrupt, 1 for start of VBLANK

    clock_t frameStart = clock();
    uint64_t instrStart = SDL_GetPerformanceCounter();

    SDL_Texture *frameBuf = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, gameHeight, gameWidth);
    LoadAudio(); // Initialize Audio

    while (SDL_QuitRequested() == SDL_FALSE)
    {
        if ((double)(SDL_GetPerformanceCounter() - instrStart) / SDL_GetPerformanceFrequency() >= (float)cycleList8080[romBuf[intel8080.pc]] / 2e6f)
        {

            cycleCounter += cycleList8080[romBuf[intel8080.pc]];

            RunOpcode(romBuf);

            instrStart = SDL_GetPerformanceCounter();

            if (cycleCounter > 16666) // 33332 clock cycles per frame
            {
                cycleCounter -= 16666;
                if (interruptStage == 1)
                {
                    generateInterrupt(romBuf, 0x10);
                    interruptStage = 0;
                }
                else
                {
                    interruptStage++;
                    generateInterrupt(romBuf, 0x08);
                }
            }
        }

        if ((float)(clock() - frameStart) / (float)CLOCKS_PER_SEC >= 1.0 / 60.0)
        {
            SDL_LockTexture(frameBuf, NULL, (void *)&rotateBuf, (int *)&frameStart);
            convertPixelData(romBuf + 0x2400);
            SDL_UnlockTexture(frameBuf);
            SDL_RenderCopyEx(render, frameBuf, NULL, textureLoc, -90, NULL, SDL_FLIP_NONE); // Rotate texture then copy to renderer
            SDL_RenderPresent(render);

            frameStart = clock();
        }
    }
    Mix_Quit();
    SDL_Quit();
    return 0;
}

void convertPixelData(uint8_t *vram)
{
    // Convert to SDL_PIXELFORMAT_ARGB8888

    for (int y = 0; y < gameWidth; y++) // Cycle through each row
    {
        for (int x = 0; x < 32; x++) // Cycle through each byte in each row
        {
            for (int b = 0; b < 8; b++) // Cycle through each bit in each byte
            {
                if ((vram[(32 * y) + x] >> b) & 0x01) // Shifts through each bit to check if active // (32 * y) skips rows of bytes
                {
                    rotateBuf[(y * gameHeight) + (x * 8) + b] = 0xFFFFFFFF; // Sets pixel on opposite side of the byte active // This flips all sprites on screen
                }
                else
                {
                    rotateBuf[(y * gameHeight) + (x * 8) + b] = 0xFF000000;
                }
            }
        }
    }
}
