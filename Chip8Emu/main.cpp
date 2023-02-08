#define SDL_MAIN_HANDLED

#include "Monitor.h"
#include "Keyboard.h"
#include "Chip8.h"
#include "Beeper.h"

#include <iostream>
#include <fstream>
//#include <chrono>
//#include <thread>

struct rom {
	size_t size;
	uint8_t* data;
};

struct rom* readROM(const char* filename){
	uint8_t *buffer = (uint8_t*)malloc(4096);
	struct rom *rom = (struct rom*)malloc(sizeof(struct rom));
	if (!buffer || !rom) {
		std::cerr << "Could not allocate memory!" << std::endl;
		return nullptr;
	}

	std::fstream file;
	file.open(filename,std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Could not open file!" << std::endl;
		return nullptr;
	}
	file.read((char*)buffer,4096);

	rom->size = file.gcount();
	file.close();
	rom->data = (uint8_t*)realloc(buffer, rom->size);
	if (!(rom->data)) {
		std::cerr << "Could not reallocate memory!" << std::endl;
		return nullptr;
	}

	return rom;
};

int main(int argc, char* argv[]) {
	using namespace Chip8Emu;

	if (argc < 5) {
		std::cerr<<"Usage: Chip8Emu <ROM=SPACE-INVADER> <WRAP-SCREEN=1> <CPUSPEED=10> <VOLUME=5000>"<<std::endl;
	}
	
	struct rom *rom = readROM(argc>=2?argv[1]:"SPACE-INVADER");
	if (!rom) {
		return -1;
	}

	const uint8_t FPS = 60;
	const bool WRAP_SCREEN = argc >= 3 ? atoi(argv[2]) : true;
	const uint16_t CPUSPEED = argc >= 4 ? atoi(argv[3]) : 10;
	const uint16_t VOLUME = argc >= 5 ? atoi(argv[4]) : 5000;

	Monitor monitor = Monitor(WRAP_SCREEN);
	Keyboard keyboard = Keyboard();
	Beeper beeper = Beeper(1000, VOLUME);
	Chip8 cpu = Chip8(&monitor, &keyboard, &beeper, CPUSPEED);
	cpu.loadProgram(rom->data,rom->size);

	while (!keyboard.shouldQuit()) {
		keyboard.updateKeys();
		cpu.cycle();
		monitor.render();
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
		SDL_Delay(1000/FPS);
	}

	free(rom->data);
	free(rom);
}