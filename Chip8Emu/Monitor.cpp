#include "Monitor.h"

#include <iostream>

Chip8Emu::Monitor::Monitor(bool wrapPixel) {
	wrapPixel_ = wrapPixel;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		throw std::runtime_error(std::string("SDL could not initialize! SDL_Error: ").append(SDL_GetError()));
	}

	gWindow = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH * SCREEN_SCALE, SCREEN_HEIGHT * SCREEN_SCALE, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		throw std::runtime_error(std::string("Window could not be created! SDL_Error: ").append(SDL_GetError()));
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL) {
		throw std::runtime_error(std::string("Renderer could not be created! SDL_Error: ").append(SDL_GetError()));
	}
}

Chip8Emu::Monitor::~Monitor() {
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	SDL_Quit();
}

void Chip8Emu::Monitor::clear() {
	for (int i = 0; i < SCREEN_HEIGHT; i++)
		pixels[i].reset();
}

bool Chip8Emu::Monitor::setPixel(unsigned int x, unsigned int y) {
	changed = true;
	if(wrapPixel_){
		x%=SCREEN_WIDTH;
		y%=SCREEN_HEIGHT;
	}else{
		if(x>SCREEN_WIDTH-1||y>SCREEN_HEIGHT-1)
			return false;
	}
	pixels[y][x] = pixels[y][x]^1;
	return pixels[y][x] != 1;
}

void Chip8Emu::Monitor::render() {
	if(!changed)
		return;

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0xFF);
	SDL_RenderClear(gRenderer);

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		SDL_Rect pixel_rect = { 0,y * SCREEN_SCALE,SCREEN_SCALE,SCREEN_SCALE };
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			if(pixels[y][x] == 1){
				pixel_rect.x = x*SCREEN_SCALE;
				SDL_RenderFillRect(gRenderer,&pixel_rect);
			}
		}
	}
	SDL_RenderPresent(gRenderer);

	changed = false;
}