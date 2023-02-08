#pragma once

#include <SDL2/SDL.h>

namespace Chip8Emu {
	class Keyboard{
		private:
			bool quit = false;
			uint8_t keys[16];//[0]->'0'...[0xF]->'f'. Contains value, if pressed
		public:
			void updateKeys();
			const uint8_t* getPressedKeys();
			bool isKeyPressed(uint8_t keycode);
			bool shouldQuit();
	};
}