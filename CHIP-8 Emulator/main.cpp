#include <iostream>
#include <SDL.h>
#include "CPU.h"
#include "SDL_thread.h"
#include "SDL_mixer.h"
#include "SDL_audio.h"
#include <cstdlib>
#include <cstdio>
#include "tinyfiledialogs.h"
using namespace std;

//Instancing Chip8 class
Chip8 chip8;

//File dialog name
const char* openFileTitle = "";
const char* filters[2] = { ".ch8", "*" };

//Bool for an infinite loop
bool isEmuRunning = true;

unsigned char emu_keys[16] = {
	SDLK_x,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_q,
	SDLK_w,
	SDLK_e,
	SDLK_a,
	SDLK_s,
	SDLK_d,
	SDLK_z,
	SDLK_c,
	SDLK_4,
	SDLK_r,
	SDLK_f,
	SDLK_v,
};

void checkKeyPressedEmu(int key, bool isUp) {
	for (int x = 0; x < 16; x++) {
		if (emu_keys[x] == key && !isUp) {
			chip8.keys[x] = 0x1;
		}
		else {
			chip8.keys[x] = 0x0;
		}
	}
}

void DrawMethodOne(SDL_Renderer* renderer, SDL_Texture* texture) {
	chip8.canDraw = false;
	uint32_t pixels[2048];
	for (int x = 0; x < 2048; x++) {
		if (chip8.gfx[x] == 1) {
			pixels[x] = 0xffffff;
		}
		else {
			pixels[x] = 0x595959;
		}
	}

	SDL_UpdateTexture(texture, NULL, pixels,64*sizeof(64));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void PlaySound(const char* filename) {
	Mix_Chunk* effect = Mix_LoadWAV(filename);
	Mix_PlayChannel(-1, effect, 0);
	chip8.canPlaySound = false;
}

void handleInitEmu(SDL_Renderer* render, SDL_Window* window) {
	chip8.Initialize();
	openFileTitle = tinyfd_openFileDialog("Select your chip 8 rom", "", 2, filters, NULL, 0);
	if (!openFileTitle) {
		tinyfd_messageBox("No game", "So you selected no game and emulator cannot handle this, because I'm lazy and didn't programmed it so,It will close as soon as you close this message", "ok", "error", 1);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(render);
		SDL_Quit();
		isEmuRunning = false;
	}
	else
	{
		chip8.loadGame(openFileTitle);
	}
}

void RunMainApp() {
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	SDL_Window* mainWindow = NULL;
	SDL_Renderer* render = NULL;
	SDL_CreateWindowAndRenderer(640, 320, 0, &mainWindow, &render);
	handleInitEmu(render, mainWindow);
	SDL_Event programEvent;
	SDL_Texture* texture = NULL;
	texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	while (isEmuRunning) {
		
		chip8.emulateCPUCycles();
		if (chip8.canDraw) {
			DrawMethodOne(render, texture);
			SDL_Delay(20);
		}
		
		if (chip8.canPlaySound) {
			PlaySound("music/sound.wav");
		}

		while (SDL_PollEvent(&programEvent)) {
			switch (programEvent.type) {
				case SDL_QUIT:
					SDL_DestroyWindow(mainWindow);
					SDL_DestroyRenderer(render);
					isEmuRunning = false;
					break;
				case SDL_KEYDOWN:
					cout << "Key pressed: " << programEvent.key.keysym.sym << endl;
					checkKeyPressedEmu(programEvent.key.keysym.sym, false);
					if (programEvent.key.keysym.sym == SDLK_F1) {
						handleInitEmu(render, mainWindow);
					}

					if (programEvent.key.keysym.sym == SDLK_ESCAPE) {
						SDL_Quit();
						SDL_DestroyWindow(mainWindow);
						SDL_DestroyRenderer(render);
						isEmuRunning = false;
					}
					break;
				case SDL_KEYUP:
					cout << "Key not pressed: " << programEvent.key.keysym.sym << endl;
					checkKeyPressedEmu(programEvent.key.keysym.sym, true);
					break;
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