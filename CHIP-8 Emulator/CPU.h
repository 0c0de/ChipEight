#pragma once
class Chip8 {
public:
	//Fontsets for rendering
	const unsigned char fontset[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
		0x20, 0x60, 0x20, 0x20, 0x70,		// 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
		0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
		0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
		0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
		0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
		0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
		0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
		0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
		0xF0, 0x80, 0xF0, 0x80, 0x80		// F
	};


	//Handles the opcodes of the cpu, which are two bytes long so we use "unsigned short"
	unsigned short opcodes;

	//Emulates the memory size of CHIP-8 which is 4K, so 4096 bytes
	unsigned char memory[4096];

	//The CHIP-8 has sixteen registers
	//V1-V15 are 8 bits or 1 byte and for general purposes
	//V16 register is for the carry flag, when is under or overflow
	unsigned char V[16];

	//Index register
	unsigned short I;

	//Program Counter
	unsigned short pc;

	//Handle pixel screen state, 1 or 0
	//Resolution of CHIP-8 is about 64 by 24 or 2048 pixels
	//so we use an array to store this info
	unsigned char gfx[64 * 24];

	//Delay timer count at a frequency of 60Hz, when reaches more than 0
	//Counts down
	unsigned char delay_timer;

	//Buzzer register
	unsigned char sound_timer;

	//Handling stack of the system, has 16 levels
	unsigned char stack[16];

	//Create a stack pointer(sp) for remembering last jump
	unsigned char sp;

	//Handles the controller, in hex [0x0-0xF]
	unsigned char keys[16];

	//Handles all init stuff like clean all
	void Initialize();

	//Emulates every cicle of the CPU
	void emulateCPUCycles();

	//Load the game once specified the path
	void loadGame(const char* pathGame);
};