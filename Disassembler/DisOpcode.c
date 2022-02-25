int DisassembleOpcode(uint8_t *buffer, int pc)
{
    uint8_t *code = &buffer[pc];
    uint8_t opbytes = 1;
    printf("%04x ", pc);
    switch (code[0])
    {
    case 0x00:
    case 0x08:
    case 0x10:
    case 0x18:
    case 0x20:
    case 0x28:
    case 0x38:
    case 0xcb:
    case 0xd9:
    case 0xdd:
    case 0xed:
    case 0xfd:
        printf("NOP");
        break;
    case 0x01:
        printf("LXI    B,#$%02x%02x", code[2], code[1]); // Load immediately into extended register B
        opbytes = 3;
        break;
    case 0x02:
        printf("STAX   B"); // Store A into extended register B
        break;
    case 0x03:
        printf("INX    B"); // Increment extended register B
        break;
    case 0x04:
        printf("INR    B"); // Increment register B
        break;
    case 0x05:
        printf("DCR    B"); // Decrement register B
        break;
    case 0x06:
        printf("MVI    B,#$%02x", code[1]); // Immediately move value at address into register B
        opbytes = 2;
        break;
    case 0x07:
        printf("RLC"); // Rotate Accumulator Left
        break;
    case 0x09:
        printf("DAD    B"); // Add extended register B with extended register H and store in extended register H
        break;
    case 0x0a:
        printf("LDAX   B"); // Load extended register B into register A
        break;
    case 0x0b:
        printf("DCX    B"); // Decrement extended register B
        break;
    case 0x0c:
        printf("INR    C"); // Increment register C
        break;
    case 0x0d:
        printf("DCR    C"); // Decrement register C
        break;
    case 0x0e:
        printf("MVI    C,#$%02x", code[1]); // Immediately move value at address into register C
        opbytes = 2;
        break;
    case 0x0f:
        printf("RRC"); // Rotate Accumulator Right
        break;
    case 0x11:
        printf("LXI    D,#$%02x%02x", code[2], code[1]); // Load immediately into extended register D
        opbytes = 3;
        break;
    case 0x12:
        printf("STAX   D"); // Store A into extended register D
        break;
    case 0x13:
        printf("INX    D"); // Increment extended register D
        break;
    case 0x14:
        printf("INR    D"); // Increment register D
        break;
    case 0x15:
        printf("DCR    D"); // Decrement register D
        break;
    case 0x16:
        printf("MVI    D,#$%02x", code[1]); // Immediately move value at address into register D
        opbytes = 2;
        break;
    case 0x17:
        printf("RAL"); // Rotate Accumulator Left
        break;
    case 0x19:
        printf("DAD    D"); // Add extended register D with extended register H and store in extended register H
        break;
    case 0x1a:
        printf("LDAX   D"); // Load extended register D into register A
        break;
    case 0x1b:
        printf("DCX    D"); // Decrement extended register D
        break;
    case 0x1c:
        printf("INR    E"); // Increment register E
        break;
    case 0x1d:
        printf("DCR    E"); // Decrement register E
        break;
    case 0x1e:
        printf("MVI    E,#$%02x", code[1]); // Immediately move value at address into register E
        opbytes = 2;
        break;
    case 0x1f:
        printf("RAR"); // Rotate Accumulator Right
        break;
    case 0x21:
        printf("LXI    H,#$%02x%02x", code[2], code[1]); // Load immediately into extended register H
        opbytes = 3;
        break;
    case 0x22:
        printf("SHLD   #$%02x%02x", code[2], code[1]); // Store extended register H at address
        opbytes = 3;
        break;
    case 0x23:
        printf("INX    H"); // Increment extended register H
        break;
    case 0x24:
        printf("INR    H"); // Increment register H
        break;
    case 0x25:
        printf("DCR    H"); // Decrement register H
        break;
    case 0x26:
        printf("MVI    H,#$%02x", code[1]); // Immediately move value at address into register H
        opbytes = 2;
        break;
    case 0x27:
        printf("DAA"); // Adds numbers in Binary-coded decimal, see online reference
        break;
    case 0x29:
        printf("DAD    H"); // Add extended register H with extended register H and store in extended register H
        break;
    case 0x2a:
        printf("LHLD   #$%02x%02x", code[2], code[1]); // Load from address to extended register H
        opbytes = 3;
        break;
    case 0x2b:
        printf("DCX    H"); // Decrement extended register H
        break;
    case 0x2c:
        printf("INR    L"); // Increment register L
        break;
    case 0x2d:
        printf("DCR    L"); // Decrement register L
        break;
    case 0x2e:
        printf("MVI    L,#$%02x", code[1]); // Immediately move value at address into register L
        opbytes = 2;
        break;
    case 0x2f:
        printf("CMA"); // Performs 1's complement operation on the Accumulator
        break;
    case 0xc3:
        printf("JMP    $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    }
    printf("\n");
    return opbytes;
}