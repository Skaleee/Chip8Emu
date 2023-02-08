#pragma once

#include <SDL2/SDL.h>
#include <bitset>

namespace Chip8Emu{
	class Monitor {
	private:
		bool wrapPixel_;

		bool changed = false;
		static const int SCREEN_WIDTH = 64,
			SCREEN_HEIGHT = 32;
		int SCREEN_SCALE = 10;

		SDL_Window *gWindow = NULL;
		SDL_Renderer *gRenderer = NULL;

		std::bitset<SCREEN_WIDTH> pixels[SCREEN_HEIGHT];
	public:
		Monitor(bool wrapPixel);
		~Monitor();
		bool setPixel(unsigned int x, unsigned int y);
		void clear();
		void render();
	};
}