#include <iostream>
#include <SDL.h>
#include "CPU.h"
#include "KEY.h"
using namespace std;

Chip8 chip8;

int emu_keys[16] = {
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_q,
	SDLK_w,
	SDLK_e,
	SDLK_r,
	SDLK_a,
	SDLK_s,
	SDLK_d,
	SDLK_f,
	SDLK_z,
	SDLK_x,
	SDLK_c,
	SDLK_v
};

void checkKeyPressedEmu(int key) {
	for (int x = 0; x < 16; x++) {
		if (emu_keys[x] == key) {
			chip8.keys[x] = 1;
		}
	}
}

void DrawMethodOne(SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderClear(renderer);
	chip8.canDraw = false;
	uint32_t pixels[32*64];

	for (int x = 0; x < 32*64; x++) {
		if (chip8.gfx[x] == 1) {
			pixels[x] = 0xffffff;
		}
		else {
			pixels[x] = 0xff0000;
		}
	}

	SDL_UpdateTexture(texture, NULL, pixels,64*sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void DrawMethodTwo(SDL_Renderer* renderer) {
	chip8.canDraw = false;
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderClear(renderer);
	
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 24; y++) {
			//cout << (void*)(chip8.gfx[x*y]) << endl;
			SDL_Rect rect;
			rect.h = 8;
			rect.w = 8;
			rect.x = x;
			rect.x = y;
			if ((chip8.gfx[y * 64 + x]) == 0x01) {
				
				//cout << "A coincidence" << endl;
				SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
				SDL_RenderFillRect(renderer, &rect);
				SDL_RenderPresent(renderer);
				SDL_RenderClear(renderer);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderFillRect(renderer, &rect);
				SDL_RenderPresent(renderer);
				SDL_RenderClear(renderer);
			}
		}
	}
	//SDL_Delay(50);

	SDL_RenderPresent(renderer);
}

void RunMainApp() {
	SDL_Window* mainWindow = NULL;
	SDL_Renderer* renderer = NULL;
	int result = SDL_CreateWindowAndRenderer(640, 320, SDL_WINDOW_SHOWN, &mainWindow, &renderer);
	chip8.Initialize();
	chip8.loadGame("C:/Users/josel/Documents/ChipEight/CHIP-8 Emulator/GAMES/PONG1P.ch8");
	//mainWindow = SDL_CreateWindow("8-CHIP Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
	SDL_RenderSetLogicalSize(renderer, 640, 320);
	if (result == -1) {
		cout << "Window not created for a reason " << SDL_GetError() << endl;
	}
	else 
	{
		SDL_Event programEvent;
		SDL_Texture* texture = NULL;
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
		bool isRunning = true;
		while (isRunning) {
			chip8.emulateCPUCycles();
			SDL_Delay(10);
			if (chip8.canDraw){
				DrawMethodOne(renderer, texture);
				//DrawMethodTwo(renderer);
			}
			system("CLS");
			for (int a = 0; a < 16; a++)
			{
				cout << "Register V[" << a << "] value is: " << (void*)chip8.V[a] << hex << endl;
				cout << "Stack[" << a << "] value is: " << (void*)chip8.stack[a] << hex << endl;
			}
			while (SDL_PollEvent(&programEvent)) {
				switch (programEvent.type) {
					case SDL_QUIT:
						isRunning = false;
						break;
					case SDL_KEYDOWN:
						cout << "Key pressed: " << programEvent.key.keysym.sym << endl;
						checkKeyPressedEmu(programEvent.key.keysym.sym);
				}
			}
		}
	}
}

int main(int argc, char* args[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "SDL failed" << SDL_GetError() << endl;
	}
	else {
		cout << "SDL Initiated correctly" << endl;
		RunMainApp();
	}

	return 0;
}