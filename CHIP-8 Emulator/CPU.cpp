#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
//#include "GPU.h"
using namespace std;

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

	//Variabe for checking if can draw the new image
	bool canDraw = false;

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

private:

	void printHex(const char* displayMessage, int val);

	string convertHex(int valueToConvert);

	void clearScreen();
};

void Chip8::Initialize(){
	//Program starts at 0x200 so pc needs to be in 0x200 postion
	pc = 0x200;
	cout << "Setting program counter to 0x200" << endl;
	//Clear opcode, index, and sp(stack pointer)
	I = 0;
	opcodes = 0;
	sp = 0;
	cout << "Cleaning opcode, Index and stack pointer" << endl;
	//Clear registers
	for (int v = 0; v < sizeof(v); v++) {
		V[v] = 0;
	}
	cout << "Cleaning registers..." << endl;


	//Clear display
	for (int g = 0; g < sizeof(gfx); g++) {
		gfx[g] = 0;
	}

	cout << "Cleaning display" << endl;


	//Clear stack
	for (int s = 0; s < sizeof(stack); s++) {
		stack[s] = 0;
	}

	cout << "Cleaning stacks levels" << endl;


	//Clear memory
	for (int m = 0; m < sizeof(memory); m++) {
		memory[m] = 0;
	}
	cout << "Cleaning memory to zero" << endl;

	//Setting to 0 timers
	sound_timer = 0;
	delay_timer = 0;
	cout << "Setting to zero, timers" << endl;

	//Load fontset into memory which is 80 bytes 
	for (int m = 0; m < sizeof(fontset); m++) {
		memory[m] = fontset[m];
	}
}

void Chip8::emulateCPUCycles() {
	//This piece of code will try to simulate a cicle of the CPU
	//Fetch the opcode
	opcodes = memory[pc] << 8 | memory[pc + 1];
	//Check opcode
	switch (opcodes & 0xF000)
	{
		case (0x0000):
			switch (opcodes & 0x000F) {
				case (0x0):
					clearScreen();
					printHex("Opcode implemented: ", opcodes);
					break;
				case (0xE):
					pc = stack[sp];
					sp--;
					printHex("Opcode implemented: ", opcodes);
					break;
				default:
					printHex("Opcode not implemented: ", opcodes);
					break;
			}

		case(0x1000):
			//Jumps to NNN address
			//Format is 0x1NNN
			pc = opcodes & 0x0fff;
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0x2000):
			//Calls a subroutine at NNN
			//Format is 0x2NNN
			stack[sp] = pc;
			sp++;
			pc = opcodes & 0x0fff;
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0x3000):
			//Condition == pseudo is if(Vx == NN)
			//format 0x3XNN
			if (V[(opcodes & 0x0f00) >> 8] == (opcodes & 0x00ff)) {
				pc += 4;
			}else {
				pc += 2;
			}
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0x4000):
			//Condition != pseudo is if(Vx != NN)
			//format 0x4XNN
			if (V[(opcodes & 0x0f00) >> 8] != (opcodes & 0x00ff)) {
				pc += 4;
			}else {
				pc += 2;
			}
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0x5000):
			//Condition != pseudo is if(Vx != Vy)
			//format 0x5XY0
			if (V[(opcodes & 0x0f00) >> 8] != V[(opcodes & 0x00f0) >> 4]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0x6000):
			//Sets Vx to NN
			//Format 0x6XNN
			V[(opcodes & 0x0f00) >> 8] = opcodes & 0x00ff;
			printHex("Opcode implemented: ", opcodes);
			pc += 2;
			break;
		case(0x7000):
			//Add NN to Vx, we don't take care of CF
			//Format 0x7XNN
			V[(opcodes & 0x0f00) >> 8] = opcodes & 0x00ff;
			printHex("Opcode implemented: ", opcodes);
			pc += 2;
			break;
		case(0x8000):
			//Add NN to Vx, check for carry flag also
			switch (opcodes & 0x000f)
			{
				case 0x0000:
					//Sets VX to VY value
					//Format 0x8XY0
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				case 0x0001:
					//Set Vx to VX 'or' VY (OR operation)
					//Format 0x8XY1
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] | V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break; 
				case 0x0002:
					//Set Vx to VX 'and' VY (AND operation)
					//Format 0x8XY2
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] & V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				case 0x0003:
					//Set Vx to VX 'xor' VY (XOR operation)
					//Format 0x8XY3
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] ^ V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				case 0x0004:
					//Adds VY to VX register, also takes care of carry flag in VF register
					//Format 0x8XY4
					if (V[(opcodes & 0x00F0) >> 4] > (0xFF - V[(opcodes & 0x0F00) >> 8]))
						V[0xF] = 1; //flag is on
					else
						V[0xF] = 0; //normal operation
					V[(opcodes & 0x0F00) >> 8] += V[(opcodes & 0x00F0) >> 4];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				case 0x0005:
					//Substracts VY to VX register, also takes care of borrow flag in VF register
					//Format 0x8XY4
					if (V[(opcodes & 0x00F0) >> 4] < (0xFF - V[(opcodes & 0x0F00) >> 8]))
						V[0xF] = 1; //flag is on
					else
						V[0xF] = 0; //normal operation
					V[(opcodes & 0x0F00) >> 8] -= V[(opcodes & 0x00F0) >> 4];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				case 0x0006:
					//Bitwise operation, stores the least significant bit of VX in VF and then shifts to the right one bit
					//Format 0x8XY6
					V[0xF] = V[(opcodes & 0x0f00) >> 8] & 0x0f;
					V[(opcodes & 0x0f00) >> 8] >> 1;
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				case 0x0007:
					//Sets VX to VY minus VX, set VF if there is a borrow
					//Format 0x8XY7
					if (V[(opcodes & 0x0f00) >> 8] - V[(opcodes & 0x00f0) >> 4] < 0) {
						V[0xf] = 1; //Enable borrow flag
					}
					else {
						V[0xf] = 0; //No flag is enabled
					}
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x00f0) >> 4] - V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				case 0x000E:
					//Stores the most significant bit from VX register in VF register and shift to left one position
					V[0xf] = V[(opcodes & 0x0f00) >> 8] && 0xf0;
					V[(opcodes & 0x0f00) >> 8] << 1;
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				default:
					printHex("Opcode not implemented: ", opcodes);
					break;
			}
			break;
		case(0x9000):
			//Condition jump if VX and VY are equal, pseudo if(Vx == Vy)
			//Format 0x9XY0
			if (V[(opcodes & 0x0f00) >> 8] == V[(opcodes & 0x00f0) >> 4]) {
				pc += 2;
			}
			printHex("Opcode implemented: ", opcodes);
			break;
		case (0xA000):
			//Sets Index to address NNN
			I = opcodes & 0x0FFF;
			pc += 2;
			printHex("Opcode implemented: ", opcodes);
			break;

		case(0xB000):
			//Jump to address NNN plus V0
			pc = V[0] + (opcodes & 0x0FFF);
			pc += 2;
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0xC000):
			//Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
			//Format is 0xCXNN
			V[opcodes & 0x0F00] = rand() & (opcodes & 0x00FF);
			pc += 2;
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0xD000): {
				//Draws a sprite at coordinate (VX, VY)
				//Format is 0xDXYN where x is x pos, y is y pos and N is the height, width is always 8
				//GPU.h
				unsigned short vx = V[(opcodes & 0x0f00) >> 8];
				unsigned short vy = V[(opcodes & 0x00f0) >> 4];
				unsigned short height = (opcodes & 0x000f);
				unsigned short width = 8;
				unsigned short pixel;

				V[0xF] = 0;

				for (int y = 0; y < height; y++) {
					pixel = memory[I + y];

					for (int x = 0; x < width; x++) {
						if ((pixel & (0x80 >> x)) != 0) {
							if (gfx[(vx + x) + ((vy + y) * 64)] == 1) {
								V[0xf] = 1;
							}
							gfx[(vx + x) + ((vy + y) * 64)] ^= 1;
						}
					}
				}
				canDraw = true;
				pc += 2;
				printHex("Opcode implemented: ", opcodes);
			}
			break;
		case(0xE000):
			switch (opcodes & 0x00ff) {
				case 0x009E:
					//Handles if the key stored in VX is pressed by using a condition, if(key() == Vx){}
					//Format 0xEX9E
					if (V[(opcodes & 0x0f00) >> 8] == 0) {
						pc += 4;
					}
					else {
						pc += 2;
					}
					break;
				case 0x00A1:
					//Handles if the key stored in VX is NOT pressed by using a condition, if(key() != Vx){}
					//Format 0xEXA1
					if (V[(opcodes & 0x0f00) >> 8] != 0) {
						pc += 4;
					}
					else {
						pc += 2;
					}
					break;
			}
			printHex("Opcode implemented: ", opcodes);
			break;
		case(0xF000):
			switch (opcodes & 0x00ff) {
				//Sets VX to delay_timer
				//Format 0xFX07
				case 0x0007:
					V[(opcodes & 0x0f00) >> 8] = delay_timer;
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				//Sets VX to key pressed, have to wait until
				//A key is pressed
				//Format 0xFX0A
				case 0x000A:
					{
						bool isKeyPressed = false;

						for (int j = 0; j < 16; j++) {
							if (keys[j] != 0) {
								V[(opcodes & 0x0f00) >> 8] = j;
								isKeyPressed = true;
							}
						}
						if (!isKeyPressed) {
							return;
						}
						pc += 2;
						printHex("Opcode implemented: ", opcodes);
					}
					break;
				//Sets delay_timer to VX
				//Format 0xFX15
				case 0x0015:
					delay_timer = V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				//Sets sound_timer to VX
				//Format 0xFX18
				case 0x0018:
					sound_timer = V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				//Adds VX to I
				//Format 0xFX1E
				case 0x001E:
					if (I + V[(opcodes & 0x0f00) >> 8] > 0xFF) {
						V[0xF] = 1; //Overflow
					}else{
						V[0xF] = 0; //Non-Overflow
					}
					I += V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				//Sets I to the location of the sprite in VX
				//This is stored in hex from 0x0 to 0xF 
				//Represented by a 4x5 font
				//Format 0xFX29
				case 0x0029:
					I = V[(opcodes & 0x0f00) >> 8] * 0x5;
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				//Sets delay_timer to VX
				//Format 0xfX33
				case 0x0033:
					memory[I] = V[(opcodes & 0x0f00) >> 8] % 1000 / 100;
					memory[I + 1] = V[(opcodes & 0x0f00) >> 8] % 100 / 10;
					memory[I + 2] = V[(opcodes & 0x0f00) >> 8] % 10;
					pc += 2;
					printHex("Opcode implemented: ", opcodes);
					break;
				//Sets delay_timer to VX
				//Format 0xfX07
				case 0x0055:

					printHex("This opcode is not implemented yet: ", opcodes);
					break;
				//Sets delay_timer to VX
				//Format 0xfX07
				case 0x0065:

					printHex("This opcode is not implemented yet: ", opcodes);
					break;
			}
			break;
		default:
			printHex("This opcode is not implemented yet: ", opcodes);
			break;
	}
	
	if(sound_timer > 0){
		if(sound_timer == 1){
			//BEEP A SOUND
		}
		sound_timer--;
	}
	
	if(delay_timer > 0){
		delay_timer--;
	}
}

void Chip8::loadGame(const char * pathGame)
{
	//Loads the file into memory
	cout << "Trying to load the game" << endl;

	//Open file in binary
	ifstream game(pathGame, ifstream::binary);
	cout << "Opening file in binary mode" << endl;
	
	//Getting length of the file
	game.seekg(0, ifstream::end);
	int gameSize = game.tellg();
	game.seekg(0, ifstream::beg);
	
	//Allocate new char which will contain game buffer
	char* gameBuffer = new char[gameSize];
	
	cout << "Length of the game is: " << gameSize << endl;
	cout << "Data in buffer" << endl;

	//Dumping the content to the char created previously
	game.read(gameBuffer, gameSize);
	cout << gameBuffer << endl;

	//Dumps game content into the CHIP-8 system memory
	cout << "Dumping game content to memory" << endl;
	for (int x = 0; x <= gameSize; x++) {

		cout << "Filling pos: " << x << endl;
		memory[x + 512] = gameBuffer[x];
	}
	cout << "Game dumped correctly" << endl;
}

void Chip8::printHex(const char* displayMessage, int val) {
	cout << displayMessage << hex << val << endl;
}

string Chip8::convertHex(int valueToConvert) {
	stringstream num;
	num << hex << valueToConvert;
	string converted(num.str());
	return converted;
}

void Chip8::clearScreen() {
	for (int i = 0; i < sizeof(gfx); i++) {
		gfx[i] = 0;
	}
}
