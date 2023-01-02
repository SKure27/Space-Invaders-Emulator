#include "main.h"
#include "Opcodes.h"

uint8_t parityCheck(uint8_t num)
{
    uint8_t count = 0;
    while (num != 0)
    {
        count += (num & 0x01);
        num = num >> 1;
    }
    return !(count % 2);
}

void int8CondSet(uint16_t ans)
{
    intel8080.conCode.z = ((ans & 0xff) == 0); // Checks if answer is equal to 0
    intel8080.conCode.s = ((ans & 0x80) != 0); // Checks if most significant bit is a 1
    intel8080.conCode.cy = (ans > 0xff);       // Checks if there is a carry (if the value is greater then 8 bits)
    intel8080.conCode.p = parityCheck(ans);    // Checks the parity of the value (if the number of 1 bits are even or odd)
}

uint8_t addint8(uint16_t ans)
{
    intel8080.conCode.cy = (ans > 0xff);
    return ans & 0xff;
}

uint16_t temp;
uint8_t psw;

void generateInterrupt(uint8_t *romBuf, uint16_t interruptNum)
{
    if (intel8080.interrupt_enabled == 1)
    {
        romBuf[intel8080.sp - 1] = (intel8080.pc & 0xff00) >> 8;
        romBuf[intel8080.sp - 2] = intel8080.pc & 0x00ff;
        intel8080.sp -= 2;

        intel8080.pc = interruptNum;
        intel8080.interrupt_enabled = 0;
    }
}

void RunOpcode(uint8_t *romBuf)
{
    switch (romBuf[intel8080.pc])
    {
    case 0x00:
        intel8080.pc++;
        break;
    case 0x01:
        intel8080.b = romBuf[intel8080.pc + 2];
        intel8080.c = romBuf[intel8080.pc + 1];
        intel8080.pc += 3;
        break;
    case 0x03:
        temp = ((uint16_t)intel8080.b << 8) + intel8080.c;
        temp++;
        intel8080.b = (temp & 0xFF00) >> 8;
        intel8080.c = temp & 0x00FF;
        intel8080.pc++;
        break;
    case 0x04:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.b + 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.b++;
        intel8080.pc++;
        break;
    case 0x05:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.b - 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.b--;
        intel8080.pc++;
        break;
    case 0x06:
        intel8080.b = romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0x07:
        intel8080.conCode.cy = (intel8080.a & 0x80) >> 7;
        intel8080.a = (intel8080.a << 1);
        intel8080.a += intel8080.conCode.cy;
        intel8080.pc += 1;
        break;
    case 0x09:
        intel8080.l = addint8(intel8080.l + intel8080.c);
        intel8080.h = addint8(intel8080.h + intel8080.b + intel8080.conCode.cy);
        intel8080.pc += 1;
        break;
    case 0x0a:
        intel8080.a = *(romBuf + ((uint16_t)intel8080.b << 8) + intel8080.c);
        intel8080.pc += 1;
        break;
    case 0x0c:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.c + 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.c++;
        intel8080.pc += 1;
        break;
    case 0x0d:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.c - 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.c--;
        intel8080.pc += 1;
        break;
    case 0x0e:
        intel8080.c = romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0x0f:
        intel8080.conCode.cy = intel8080.a & 0x01;
        intel8080.a = (intel8080.a >> 1);
        intel8080.a += (intel8080.conCode.cy << 7);
        intel8080.pc += 1;
        break;
    case 0x11:
        intel8080.d = romBuf[intel8080.pc + 2];
        intel8080.e = romBuf[intel8080.pc + 1];
        intel8080.pc += 3;
        break;
    case 0x12:
        *(romBuf + ((uint16_t)intel8080.d << 8) + intel8080.e) = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x13:
        intel8080.conCode.fill = intel8080.conCode.cy;
        intel8080.e = addint8(intel8080.e + 1);
        intel8080.d = addint8(intel8080.d + intel8080.conCode.cy);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.pc += 1;
        break;
    case 0x14:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.d + 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.d++;
        intel8080.pc++;
        break;
    case 0x15:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.d - 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.d--;
        intel8080.pc++;
        break;
    case 0x16:
        intel8080.d = romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0x19:
        intel8080.l = addint8(intel8080.l + intel8080.e);
        intel8080.h = addint8(intel8080.h + intel8080.d + intel8080.conCode.cy);
        intel8080.pc += 1;
        break;
    case 0x1a:
        intel8080.a = *(romBuf + ((uint16_t)intel8080.d << 8) + intel8080.e);
        intel8080.pc += 1;
        break;
    case 0x1b:
        temp = ((uint16_t)intel8080.d << 8) + intel8080.e;
        temp--;
        intel8080.d = (temp & 0xFF00) >> 8;
        intel8080.e = temp & 0x00FF;
        intel8080.pc += 1;
        break;
    case 0x1f:
        temp = intel8080.conCode.cy;
        intel8080.conCode.cy = intel8080.a & 0x01;
        intel8080.a >>= 1;
        intel8080.a |= temp << 7;
        intel8080.pc += 1;
        break;
    case 0x21:
        intel8080.h = romBuf[intel8080.pc + 2];
        intel8080.l = romBuf[intel8080.pc + 1];
        intel8080.pc += 3;
        break;
    case 0x22:
        *(romBuf + ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1]) = intel8080.l;
        *(romBuf + ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1] + 1) = intel8080.h;
        intel8080.pc += 3;
        break;
    case 0x23:
        intel8080.conCode.fill = intel8080.conCode.cy;
        intel8080.l = addint8(intel8080.l + 1);
        intel8080.h = addint8(intel8080.h + intel8080.conCode.cy);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.pc += 1;
        break;
    case 0x24:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.h + 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.h++;
        intel8080.pc += 1;
        break;
    case 0x26:
        intel8080.h = romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0x27:
        if ((intel8080.a & 0xf) > 9)
            intel8080.a += 6;
        if ((intel8080.a & 0xf0) > 0x90)
        {
            temp = (uint16_t)intel8080.a + 0x60;
            intel8080.a = temp & 0xff;
            int8CondSet(temp);
        }
        intel8080.pc += 1;
        break;
    case 0x29:
        intel8080.l = addint8(intel8080.l + intel8080.l);
        intel8080.h = addint8(intel8080.h + intel8080.h + intel8080.conCode.cy);
        intel8080.pc += 1;
        break;
    case 0x2a:
        intel8080.l = *(romBuf + ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1]);
        intel8080.h = *(romBuf + ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1] + 1);
        intel8080.pc += 3;
        break;
    case 0x2b:
        temp = ((uint16_t)intel8080.h << 8) + intel8080.l;
        temp--;
        intel8080.h = (temp & 0xFF00) >> 8;
        intel8080.l = temp & 0x00FF;
        intel8080.pc += 1;
        break;
    case 0x2c:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.l + 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.l++;
        intel8080.pc += 1;
        break;
    case 0x2e:
        intel8080.l = romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0x2f:
        intel8080.a = ~intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x31:
        intel8080.sp = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        intel8080.pc += 3;
        break;
    case 0x32:
        *(romBuf + ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1]) = intel8080.a;
        intel8080.pc += 3;
        break;
    case 0x34:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(*(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) + 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) += 1;
        intel8080.pc += 1;
        break;
    case 0x35:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(*(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) - 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) -= 1;
        intel8080.pc += 1;
        break;
    case 0x36:
        *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) = romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0x37:
        intel8080.conCode.cy = 1;
        intel8080.pc += 1;
        break;
    case 0x3a:
        intel8080.a = *(romBuf + ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1]);
        intel8080.pc += 3;
        break;
    case 0x3c:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.a + 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.a++;
        intel8080.pc += 1;
        break;
    case 0x3d:
        intel8080.conCode.fill = intel8080.conCode.cy;
        int8CondSet(intel8080.a - 1);
        intel8080.conCode.cy = intel8080.conCode.fill;
        intel8080.a--;
        intel8080.pc += 1;
        break;
    case 0x3e:
        intel8080.a = romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0x41:
        intel8080.b = intel8080.c;
        intel8080.pc += 1;
        break;
    case 0x46:
        intel8080.b = *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        intel8080.pc += 1;
        break;
    case 0x47:
        intel8080.b = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x48:
        intel8080.c = intel8080.b;
        intel8080.pc += 1;
        break;
    case 0x4e:
        intel8080.c = *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        intel8080.pc += 1;
        break;
    case 0x4f:
        intel8080.c = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x56:
        intel8080.d = *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        intel8080.pc += 1;
        break;
    case 0x57:
        intel8080.d = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x5e:
        intel8080.e = *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        intel8080.pc += 1;
        break;
    case 0x5f:
        intel8080.e = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x61:
        intel8080.h = intel8080.c;
        intel8080.pc += 1;
        break;
    case 0x65:
        intel8080.h = intel8080.l;
        intel8080.pc += 1;
        break;
    case 0x66:
        intel8080.h = *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        intel8080.pc += 1;
        break;
    case 0x67:
        intel8080.h = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x68:
        intel8080.l = intel8080.b;
        intel8080.pc += 1;
        break;
    case 0x69:
        intel8080.l = intel8080.c;
        intel8080.pc += 1;
        break;
    case 0x6f:
        intel8080.l = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x70:
        *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) = intel8080.b;
        intel8080.pc += 1;
        break;
    case 0x71:
        *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) = intel8080.c;
        intel8080.pc += 1;
        break;
    case 0x77:
        *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l) = intel8080.a;
        intel8080.pc += 1;
        break;
    case 0x78:
        intel8080.a = intel8080.b;
        intel8080.pc += 1;
        break;
    case 0x79:
        intel8080.a = intel8080.c;
        intel8080.pc += 1;
        break;
    case 0x7a:
        intel8080.a = intel8080.d;
        intel8080.pc += 1;
        break;
    case 0x7b:
        intel8080.a = intel8080.e;
        intel8080.pc += 1;
        break;
    case 0x7c:
        intel8080.a = intel8080.h;
        intel8080.pc += 1;
        break;
    case 0x7d:
        intel8080.a = intel8080.l;
        intel8080.pc += 1;
        break;
    case 0x7e:
        intel8080.a = *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        intel8080.pc += 1;
        break;
    case 0x80:
        int8CondSet(intel8080.a + intel8080.b);
        intel8080.a += intel8080.b;
        intel8080.pc += 1;
        break;
    case 0x81:
        int8CondSet(intel8080.a + intel8080.c);
        intel8080.a += intel8080.c;
        intel8080.pc += 1;
        break;
    case 0x83:
        int8CondSet((uint16_t)intel8080.a + (uint16_t)intel8080.e);
        intel8080.a += intel8080.e;
        intel8080.pc += 1;
        break;
    case 0x85:
        int8CondSet(intel8080.a + intel8080.l);
        intel8080.a += intel8080.l;
        intel8080.pc += 1;
        break;
    case 0x86:
        int8CondSet(intel8080.a + *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l));
        intel8080.a += *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        intel8080.pc += 1;
        break;
    case 0x8a:
        temp = (uint16_t)intel8080.a + (uint16_t)intel8080.d + intel8080.conCode.cy;
        int8CondSet(temp);
        intel8080.a = (temp & 0xff);
        intel8080.pc += 1;
        break;
    case 0x97:
        int8CondSet(0x00);
        intel8080.a = 0x00;
        intel8080.pc += 1;
        break;
    case 0xa0:
        intel8080.a &= intel8080.b;
        int8CondSet(intel8080.a);
        intel8080.pc += 1;
        break;
    case 0xa6:
        intel8080.a &= *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        int8CondSet(intel8080.a);
        intel8080.pc += 1;
        break;
    case 0xa7:
        int8CondSet(intel8080.a);
        intel8080.pc += 1;
        break;
    case 0xa8:
        intel8080.a ^= intel8080.b;
        int8CondSet(intel8080.a);
        intel8080.pc += 1;
        break;
    case 0xaf:
        int8CondSet(0x00);
        intel8080.a = 0x00;
        intel8080.pc += 1;
        break;
    case 0xb0:
        intel8080.a |= intel8080.b;
        int8CondSet(intel8080.a);
        intel8080.pc += 1;
        break;
    case 0xb4:
        intel8080.a |= intel8080.h;
        int8CondSet(intel8080.a);
        intel8080.pc += 1;
        break;
    case 0xb6:
        intel8080.a |= *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l);
        int8CondSet(intel8080.a);
        intel8080.pc += 1;
        break;
    case 0xb8:
        int8CondSet(intel8080.a - intel8080.b);
        intel8080.pc++;
        break;
    case 0xbc:
        int8CondSet(intel8080.a - intel8080.h);
        intel8080.pc++;
        break;
    case 0xbe:
        int8CondSet(intel8080.a - *(romBuf + ((uint16_t)intel8080.h << 8) + intel8080.l));
        intel8080.pc++;
        break;
    case 0xc0:
        if (!intel8080.conCode.z)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.sp + 1] << 8) + romBuf[intel8080.sp];
            intel8080.sp += 2;
        }
        else
        {
            intel8080.pc++;
        }
        break;
    case 0xc1:
        intel8080.b = romBuf[intel8080.sp + 1];
        intel8080.c = romBuf[intel8080.sp];
        intel8080.sp += 2;
        intel8080.pc += 1;
        break;
    case 0xc2:
        if (!intel8080.conCode.z)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xc3:
        intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        break;
    case 0xc4:
        if (!intel8080.conCode.z)
        {
            temp = intel8080.pc + 3;
            romBuf[intel8080.sp - 1] = (temp & 0xff00) >> 8;
            romBuf[intel8080.sp - 2] = temp & 0x00ff;
            intel8080.sp -= 2;
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xc5:
        romBuf[intel8080.sp - 1] = intel8080.b;
        romBuf[intel8080.sp - 2] = intel8080.c;
        intel8080.sp -= 2;
        intel8080.pc += 1;
        break;
    case 0xc6:
        int8CondSet(intel8080.a + romBuf[intel8080.pc + 1]);
        intel8080.a += romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0xc8:
        if (intel8080.conCode.z)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.sp + 1] << 8) + romBuf[intel8080.sp];
            intel8080.sp += 2;
        }
        else
        {
            intel8080.pc += 1;
        }
        break;
    case 0xc9: // RET
        intel8080.pc = ((uint16_t)romBuf[intel8080.sp + 1] << 8) + romBuf[intel8080.sp];
        intel8080.sp += 2;
        break;
    case 0xca:
        if (intel8080.conCode.z)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xcc:
        if (intel8080.conCode.z)
        {
            temp = intel8080.pc + 3;
            romBuf[intel8080.sp - 1] = (temp & 0xff00) >> 8;
            romBuf[intel8080.sp - 2] = temp & 0x00ff;
            intel8080.sp -= 2;
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xcd:                   // CALL
        temp = intel8080.pc + 3; // Location to return to after call.

        romBuf[intel8080.sp - 1] = (temp & 0xff00) >> 8;
        romBuf[intel8080.sp - 2] = temp & 0x00ff;
        intel8080.sp -= 2;
        intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        break;
    case 0xd0:
        if (!intel8080.conCode.cy)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.sp + 1] << 8) + romBuf[intel8080.sp];
            intel8080.sp += 2;
        }
        else
        {
            intel8080.pc++;
        }
        break;
    case 0xd1:
        intel8080.d = romBuf[intel8080.sp + 1];
        intel8080.e = romBuf[intel8080.sp];
        intel8080.sp += 2;
        intel8080.pc += 1;
        break;
    case 0xd2:
        if (!intel8080.conCode.cy)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xd3:
        processOutputs(romBuf[intel8080.pc + 1]);
        intel8080.pc += 2;
        break;
    case 0xd4:
        if (!intel8080.conCode.cy)
        {
            temp = intel8080.pc + 3;
            romBuf[intel8080.sp - 1] = (temp & 0xff00) >> 8;
            romBuf[intel8080.sp - 2] = temp & 0x00ff;
            intel8080.sp -= 2;
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xd5:
        romBuf[intel8080.sp - 1] = intel8080.d;
        romBuf[intel8080.sp - 2] = intel8080.e;
        intel8080.sp -= 2;
        intel8080.pc += 1;
        break;
    case 0xd6:
        int8CondSet(intel8080.a - romBuf[intel8080.pc + 1]);
        intel8080.a -= romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0xd8:
        if (intel8080.conCode.cy)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.sp + 1] << 8) + romBuf[intel8080.sp];
            intel8080.sp += 2;
        }
        else
        {
            intel8080.pc++;
        }
        break;
    case 0xda:
        if (intel8080.conCode.cy)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xdb:
        processInputs(romBuf[intel8080.pc + 1]);
        intel8080.pc += 2;
        break;
    case 0xde:
        int8CondSet(intel8080.a - romBuf[intel8080.pc + 1] - intel8080.conCode.cy);
        intel8080.a -= (romBuf[intel8080.pc + 1] + intel8080.conCode.cy);
        intel8080.pc += 2;
        break;
    case 0xe1:
        intel8080.h = romBuf[intel8080.sp + 1];
        intel8080.l = romBuf[intel8080.sp];
        intel8080.sp += 2;
        intel8080.pc += 1;
        break;
    case 0xe3:
        temp = romBuf[intel8080.sp + 1];
        romBuf[intel8080.sp + 1] = intel8080.h;
        intel8080.h = temp;

        temp = romBuf[intel8080.sp];
        romBuf[intel8080.sp] = intel8080.l;
        intel8080.l = temp;

        intel8080.pc += 1;
        break;
    case 0xe5:
        romBuf[intel8080.sp - 1] = intel8080.h;
        romBuf[intel8080.sp - 2] = intel8080.l;
        intel8080.sp -= 2;
        intel8080.pc += 1;
        break;
    case 0xe6:
        int8CondSet(intel8080.a & romBuf[intel8080.pc + 1]);
        intel8080.a = intel8080.a & romBuf[intel8080.pc + 1];
        intel8080.pc += 2;
        break;
    case 0xe9:
        intel8080.pc = ((uint16_t)intel8080.h << 8) + intel8080.l;
        break;
    case 0xeb:
        temp = intel8080.h;
        intel8080.h = intel8080.d;
        intel8080.d = temp;

        temp = intel8080.l;
        intel8080.l = intel8080.e;
        intel8080.e = temp;
        intel8080.pc += 1;
        break;
    case 0xf1:
        intel8080.a = romBuf[intel8080.sp + 1];
        psw = romBuf[intel8080.sp];

        intel8080.conCode.z = psw & 0x01;
        intel8080.conCode.s = (psw & 0x02) >> 1;
        intel8080.conCode.p = (psw & 0x04) >> 2;
        intel8080.conCode.cy = (psw & 0x08) >> 3;
        intel8080.conCode.ac = (psw & 0x10) >> 4;
        intel8080.sp += 2;
        intel8080.pc += 1;
        break;
    case 0xf5:
        romBuf[intel8080.sp - 1] = intel8080.a;

        psw = (intel8080.conCode.z |
               intel8080.conCode.s << 1 |
               intel8080.conCode.p << 2 |
               intel8080.conCode.cy << 3 |
               intel8080.conCode.ac << 4);

        romBuf[intel8080.sp - 2] = psw;
        intel8080.sp -= 2;
        intel8080.pc += 1;
        break;
    case 0xf6:
        intel8080.a |= romBuf[intel8080.pc + 1];
        int8CondSet(intel8080.a);
        intel8080.pc += 2;
        break;
    case 0xfa:
        if (intel8080.conCode.s)
        {
            intel8080.pc = ((uint16_t)romBuf[intel8080.pc + 2] << 8) + romBuf[intel8080.pc + 1];
        }
        else
        {
            intel8080.pc += 3;
        }
        break;
    case 0xfb:
        intel8080.interrupt_enabled = 1;
        intel8080.pc += 1;
        break;
    case 0xfe:
        int8CondSet(intel8080.a - romBuf[intel8080.pc + 1]);
        intel8080.pc += 2;
        break;
    default:
        printf("Unimplemented Instruction: 0x%x\n", romBuf[intel8080.pc]);
        printf("PC: %x\n", intel8080.pc);
        exit(0);
        break;
    }
}
