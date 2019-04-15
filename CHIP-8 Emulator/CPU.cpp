#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "CPU.h"

//#include "GPU.h"
using namespace std;

//Reset all vars
void Chip8::Initialize(){
	//Program starts at 0x200 so pc needs to be in 0x200 postion
	pc = 0x200;
	cout << "Setting program counter to 0x200" << endl;
	//Clear opcode, index, and sp(stack pointer)
	I = 0;
	opcodes = 0;
	sp = 0;
	cout << "Cleaning opcode, Index and stack pointer" << endl;
	cout << "Cleaning registers..." << endl;


	//Clear display,registers, and stack
	clearScreen();
	for (int g = 0; g < 16; g++) {
		stack[g] = 0;
		V[g] = 0;
	}

	cout << "Cleaning display" << endl;
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
	//printHex("Opcode 0x", opcodes);
	//Get first bit of opcode, then check
	//All the codes that start with that symbol
	switch (opcodes & 0xF000)
	{
		case (0x0000): 
			
			switch (opcodes & 0x00FF) {
				//0x00E0
				//Clears the screen, I used memset but you could use 
				//A traditional loop, your election
				case (0xE0):
					clearScreen();
					pc += 2;
					break;
				
				//0x00EE
				//Push data to the stack
				//Decrease stack pointer, then sets the program counter, PC
				//And increment PC in 2 positions
				case (0xEE):
					sp--;
					pc = stack[sp];
					pc += 2;
					break;
				default:
					printHex("Opcode not implemented: ", opcodes);
					pc += 2;
					break;
			}
			break;

		case(0x1000):
			//Jumps to NNN address
			//Format is 0x1NNN
			pc = opcodes & 0x0fff;
			break;
		case(0x2000):
			//Calls a subroutine at NNN
			//Format is 0x2NNN
			stack[sp] = pc;
			sp++;
			pc = opcodes & 0x0fff;
			break;
		case(0x3000):
			//Condition == pseudo is if(Vx == NN)
			//If Vx equal to NN skips next instruction
			//format 0x3XNN
			if (V[(opcodes & 0x0f00) >> 8] == (opcodes & 0x00ff)) {
				pc += 4;
			}else {
				pc += 2;
			}
			break;
		case(0x4000):
			//Condition != pseudo is if(Vx != NN)
			//If Vx not equal to NN skips next instruction
			//format 0x4XNN
			if (V[(opcodes & 0x0f00) >> 8] != (opcodes & 0x00ff)) {
				pc += 4;
			}else {
				pc += 2;
			}
			break;
		case(0x5000):
			//Condition != pseudo is if(Vx != Vy)
			//If Vx not equal to Vy skips next instruction
			//format 0x5XY0
			if (V[(opcodes & 0x0f00) >> 8] == V[(opcodes & 0x00f0) >> 4]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;
		case(0x6000):
			//Sets Vx to NN
			//Format 0x6XNN
			V[(opcodes & 0x0f00) >> 8] = (opcodes & 0x00ff);
			pc += 2;
			break;
		case(0x7000):
			//Add NN to Vx, we don't take care of CF
			//Format 0x7XNN
			V[(opcodes & 0x0f00) >> 8] += (opcodes & 0x00ff);
			pc += 2;
			break;
		case(0x8000):
			switch (opcodes & 0x000f)
			{
				case 0x0:
					//Sets VX to VY value
					//Format 0x8XY0
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					break;
				case 0x1:
					//Set Vx to VX 'or' VY (OR operation)
					//Format 0x8XY1
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] | V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					break; 
				case 0x2:
					//Set Vx to VX 'and' VY (AND operation)
					//Format 0x8XY2
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] & V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					break;
				case 0x3:
					//Set Vx to VX 'xor' VY (XOR operation)
					//Format 0x8XY3
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] ^ V[(opcodes & 0x00f0) >> 4];
					pc += 2;
					break;
				case 0x4:
					//Adds VY to VX register, also takes care of carry flag in VF register
					//Format 0x8XY4
					if (V[(opcodes & 0x00F0) >> 4] > (0xFF - V[(opcodes & 0x0F00) >> 8]))
						V[0xF] = 1; //flag is on
					else
						V[0xF] = 0; //normal operation
					V[(opcodes & 0x0F00) >> 8] += V[(opcodes & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x5:
					//Substracts VY to VX register, also takes care of borrow flag in VF register
					//Format 0x8XY4
					if (V[(opcodes & 0x0F00) >> 8] > V[(opcodes & 0x00F0) >> 4])
						V[0xF] = 1; //flag is on
					else
						V[0xF] = 0; //normal operation
					V[(opcodes & 0x0F00) >> 8] -= V[(opcodes & 0x00F0) >> 4];
					pc += 2;
					break;
				case 0x6:
					//Bitwise operation, stores the least significant bit of VX in VF and then shifts VX to the right one bit
					//Format 0x8XY6
					//If it is one then VF is set to 1 if not then is 0
					V[0xF] = V[(opcodes & 0x0f00) >> 8] & 0x1;
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] >> 1;
					pc += 2;
					break;
				case 0x7:
					//Sets VX to VY minus VX, set VF if there is a borrow
					//Format 0x8XY7
					if (V[(opcodes & 0x00F) >> 4] > V[(opcodes & 0x0F) >> 8]) {
						V[0xf] = 1; //Enable borrow flag
					}
					else {
						V[0xf] = 0; //No flag is enabled
					}
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x00f0) >> 4] - V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					break;
				case 0xE:
					//Stores the most significant bit from VX register in VF register and shift VX to left one position
					//Format 0x8XY6
					//If it is one then VF is set to 1 if not then is 0
					V[0xF] = V[(opcodes & 0x0f00) >> 8] >> 7;
					V[(opcodes & 0x0f00) >> 8] = V[(opcodes & 0x0f00) >> 8] << 1;
					pc += 2;
					break;
				default:
					printHex("Opcode not implemented: ", opcodes);
					pc += 2;
					break;
			}
			break;
		case(0x9000):
			//Condition jump if VX and VY are equal, pseudo if(Vx == Vy)
			//If VX is equal to VY then skip to the next instruction 
			//Format 0x9XY0
			if (V[(opcodes & 0x0f00) >> 8] != V[(opcodes & 0x00f0) >> 4]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;
		case (0xA000):
			//Sets Index to address NNN
			I = (opcodes & 0x0FFF);
			pc += 2;
			break;

		case(0xB000):
			//Jump to address NNN plus V0
			pc = V[0] + (opcodes & 0x0FFF);
			break;
		case(0xC000):
			//Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
			//Format is 0xCXNN
			V[(opcodes & 0x0F00) >> 8] = (rand() % 255 + 1) & (opcodes & 0x00FF);
			pc += 2;
			break;
		case(0xD000): {
				//Draws a sprite at coordinate (VX, VY)
				//Format is 0xDXYN where x is x pos, y is y pos and N is the height, width is always 8
				//GPU.h
				unsigned short vx = V[(opcodes & 0x0f00) >> 8];
				unsigned short vy = V[(opcodes & 0x00f0) >> 4];
				unsigned short height = opcodes & 0x000f;
				unsigned short width = 8;
				unsigned short pixel;

				V[0xF] = 0;

				for (int y = 0; y < height; y++) {
					pixel = memory[I + y];

					for (int x = 0; x < width; x++) {
						if ((pixel & (0x80 >> x)) != 0) {
							if (gfx[(vx + x + ((vy + y)*64))] == 1) {
								V[15] = 1;
							}
							gfx[vx + x + ((vy + y)*64)] ^= 1;
						}
					}
				}
				canDraw = true;
				pc += 2;
			}
			break;
		case(0xE000):
			switch (opcodes & 0x00ff) {
				case 0x9E:
					//Handles if the key stored in VX is pressed by using a condition, if(key() == Vx){}
					//If the key is equal to key stored in VX then skip to the next instruction 
					//Format 0xEX9E
					if (keys[V[(opcodes & 0x0f00) >> 8]] != 0) {
						pc += 4;
					}
					else {
						pc += 2;
					}
					break;
				case 0xA1:
					//Handles if the key stored in VX is NOT pressed by using a condition, if(key() != Vx){}
					//Format 0xEXA1
					//If the key is NOT equal to key stored in VX then skip to the next instruction
					if (keys[V[(opcodes & 0x0f00) >> 8]] == 0) {
						pc += 4;
					}
					else {
						pc += 2;
					}
					break;
				default:
					pc += 2;
					break;
			}
			break;
		case(0xF000):
			switch (opcodes & 0x00ff) {
				//Sets VX to delay_timer
				//Format 0xFX07
				case 0x07:
					V[(opcodes & 0x0f00) >> 8] = delay_timer;
					pc += 2;
					break;
				//Sets VX to key pressed, have to wait until
				//Wait for keypress
				//Format 0xFX0A
				case 0x0A:
					{
						cout << "Checking for a key press" << endl;
						bool isKeyPressed = false;
						for (int j = 0; j < 16; j++) {
							cout << "Checking key in location " << j << hex << endl;
							if (keys[j] == 1) {
								V[(opcodes & 0x0f00) >> 8] = j;
								isKeyPressed = true;
							}
						}						
						if (!isKeyPressed) {
							return;
						}

						pc += 2;
						break;
					}
				//Sets delay_timer to VX
				//Format 0xFX15
				case 0x15:
					delay_timer = V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					break;
				//Sets sound_timer to VX
				//Format 0xFX18
				case 0x18:
					sound_timer = V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					break;
				//Adds VX to I, and also checks for overflow
				//Format 0xFX1E
				case 0x1E:
					if (I + V[(opcodes & 0x0f00) >> 8] > 0xFFF) {
						V[0xF] = 1; //Overflow
					}else{
						V[0xF] = 0; //Non-Overflow
					}
					I += V[(opcodes & 0x0f00) >> 8];
					pc += 2;
					break;
				//Sets I to the location of the sprite in VX
				//This is stored in hex from 0x0 to 0xF 
				//Represented by a 4x5 font
				//Format 0xFX29
				case 0x29:
					I = V[(opcodes & 0x0f00) >> 8] * 0x5;
					pc += 2;
					break;
				//Sets delay_timer to VX
				//Format 0xfX33
				case 0x33:
					memory[I] = V[(opcodes & 0x0f00) >> 8] % 1000 / 100;
					memory[I + 1] = V[(opcodes & 0x0f00) >> 8] % 100 / 10;
					memory[I + 2] = V[(opcodes & 0x0f00) >> 8] % 10;
					pc += 2;
					break;
				//Sets delay_timer to VX
				//Format 0xfX55
				case 0x55:
					for (int q = 0; q <= ((opcodes & 0x0f00) >> 8); q++) {
						memory[I + q] = V[q];
					}
					I = I + ((opcodes & 0x0f00) >> 8) + 1;
					pc += 2;
					break;
				//Fills V0 to VX (including VX) with values from memory starting at address I. 
				//The offset from I is increased by 1 for each value written, but I itself is left unmodified.
				//Format 0xfX65
				case 0x65:
					for (auto q = 0; q <= ((opcodes & 0x0f00) >> 8); q++) {
						V[q] = memory[I+q];
					}
					I = I + ((opcodes & 0x0f00) >> 8) + 1;
					pc += 2;
					break;
			}
			break;
		default:
			pc += 2;
			printHex("This opcode is not implemented yet: ", opcodes);
			break;
	}
	
	if(sound_timer > 0){
		if (sound_timer == 1) {
			canPlaySound = true;
		}
		sound_timer--;
		canPlaySound = false;
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
	cout << hex << displayMessage << val << endl;
}

void Chip8::clearScreen() {
	memset(gfx, 0, sizeof(gfx));
	canDraw = true;
}
