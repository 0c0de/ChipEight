#include <iostream>
#include <SDL.h>
#include "CPU.h"
using namespace std;

SDL_Surface* loadImage(const char* path) {
	return SDL_LoadBMP(path);
}

void RunMainApp() {
	SDL_Window* mainWindow = NULL;
	SDL_Surface* surfaceWindow = NULL;
	SDL_Surface* helloWorldImage = loadImage("C:/Users/josel/Pictures/hell1.bmp");
	mainWindow = SDL_CreateWindow("8-CHIP Emulator", 500, 500, 500, 500, SDL_WINDOW_SHOWN);

	if (mainWindow == NULL) {
		cout << "Window not created for a reason " << SDL_GetError() << endl;
	}
	else 
	{
		Chip8 chip8;
		chip8.Initialize();
		chip8.loadGame("C:/Users/josel/source/repos/CHIP-8 Emulator/CHIP-8 Emulator/GAMES/MAZE");
		SDL_Event programEvent;
		bool isRunning = true;
		while (isRunning) {
			chip8.emulateCPUCycles();
			while (SDL_PollEvent(&programEvent)) {
				switch (programEvent.type) {
					case SDL_QUIT:
						//SDL_Quit();
						isRunning = false;
						break;
					case SDL_KEYDOWN:
						cout << "Key pressed: " << programEvent.key.keysym.sym << endl;
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