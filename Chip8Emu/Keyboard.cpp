#include "Keyboard.h"

//#include <iostream>

void Chip8Emu::Keyboard::updateKeys() {
	/*for (int i = 0; i <= 0xF; i++)
		keys[i] = 0;*/
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if ('0' <= e.key.keysym.sym && e.key.keysym.sym <= '9')
					keys[e.key.keysym.sym - '0'] = e.key.keysym.sym;
				else if ('a' <= e.key.keysym.sym && e.key.keysym.sym <= 'f')
					keys[e.key.keysym.sym - 'a' + 10] = e.key.keysym.sym;
				break;
			case SDL_KEYUP:
				if ('0' <= e.key.keysym.sym && e.key.keysym.sym <= '9')
					keys[e.key.keysym.sym - '0'] = 0;
				else if ('a' <= e.key.keysym.sym && e.key.keysym.sym <= 'f')
					keys[e.key.keysym.sym - 'a' + 10] = 0;
				break;
		}
	}
	/*for (int i = 0; i <= 0xF; i++)
		std::cout<<(int)keys[i]<<" ";
	std::cout<<std::endl;*/
}

const uint8_t* Chip8Emu::Keyboard::getPressedKeys() {
	return keys;
}

bool Chip8Emu::Keyboard::isKeyPressed(uint8_t keycode) {
	return keys[keycode];
}

bool Chip8Emu::Keyboard::shouldQuit() {
	return quit;
}