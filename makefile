CC = gcc
CFLAGS = -lmingw32 -LSDL\lib -lSDL2main -lSDL2 -lSDL2_mixer -o

all: Emu.exe

Emu.exe: main.c Opcodes.c 
	$(CC) main.c Opcodes.c $(CFLAGS) Build\Emu.exe
