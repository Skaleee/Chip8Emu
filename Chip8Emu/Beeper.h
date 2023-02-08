#pragma once

#include <SDL2/SDL.h>

namespace Chip8Emu {
	class Beeper {
		private:
			SDL_AudioDeviceID audioDevice;

			static void audioCallback(void *userdata, uint8_t *stream, int len);
		public:
			uint16_t frequency;//soundwave frequency
			uint16_t volume;
			
			Beeper(int frequency, int volume); //samples must be a power of two
			~Beeper();
			void startBeeping();
			void stopBeeping();
	};
}