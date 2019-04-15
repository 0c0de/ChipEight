#include <iostream>
#include <SDL.h>
#include "CPU.h"
#include "KEY.h"
#include "SDL_thread.h"
#include "SDL_audio.h"
using namespace std;

Chip8 chip8;
void my_audio_callback(void *userdata, Uint8 *stream, int len);

// variable declarations
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play
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


void my_audio_callback(void *userdata, Uint8 *stream, int len) {

	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);

	audio_pos += len;
	audio_len -= len;
}

void PlaySound(const char* filename) {
		// local variables
		static Uint32 wav_length; // length of our sample
		static Uint8 *wav_buffer; // buffer containing our audio file
		static SDL_AudioSpec wav_spec; // the specs of our piece of musi

		/* Load the WAV */
		// the specs, length and buffer of our wav are filled
		if (SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length) == NULL) {
			cerr << "An error occurred loading wav file " << SDL_GetError() << endl;
		}
		// set the callback function
		wav_spec.callback = my_audio_callback;
		wav_spec.userdata = NULL;
		// set our global static variables
		audio_pos = wav_buffer; // copy sound buffer
		audio_len = wav_length; // copy file length

		/* Open the audio device */
		if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
			fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
			exit(-1);
		}

		/* Start playing */
		SDL_PauseAudio(0);

		// wait until we're don't playing
		while (audio_len > 0) {
			SDL_Delay(0.1);
		}

		// shut everything down
		SDL_CloseAudio();
		SDL_FreeWAV(wav_buffer);
		chip8.canPlaySound = false;
	
}

void RunMainApp() {
	SDL_Window* mainWindow = NULL;
	SDL_Renderer* renderer = NULL;
	int result = SDL_CreateWindowAndRenderer(640, 320, SDL_WINDOW_OPENGL, &mainWindow, &renderer);
	chip8.Initialize();
	chip8.loadGame("C:/Users/josel/source/repos/CHIP-8 Emulator/CHIP-8 Emulator/GAMES/UFO");
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
			if (chip8.canDraw) {
				DrawMethodOne(renderer, texture);
				SDL_Delay(20);
			}

			if (chip8.canPlaySound) {
				PlaySound("music/sound.wav");
			}

			while (SDL_PollEvent(&programEvent)) {
				switch (programEvent.type) {
					case SDL_QUIT:
						isRunning = false;
						break;
					case SDL_KEYDOWN:
						cout << "Key pressed: " << programEvent.key.keysym.sym << endl;
						checkKeyPressedEmu(programEvent.key.keysym.sym, false);
					case SDL_KEYUP:
						cout << "Key not pressed: " << programEvent.key.keysym.sym << endl;
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