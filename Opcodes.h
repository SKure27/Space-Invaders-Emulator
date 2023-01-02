#include "common.h"

typedef struct
{
    uint8_t z : 1;
    uint8_t s : 1;
    uint8_t p : 1;
    uint8_t cy : 1;
    uint8_t ac : 1;
    uint8_t fill : 3;
} flags;

struct intel8080
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    flags conCode;
    uint8_t interrupt_enabled;
    uint16_t shiftReg;

} intel8080;

void generateInterrupt(uint8_t *romBuf, uint16_t interruptNum);

void RunOpcode(uint8_t *romBuf);