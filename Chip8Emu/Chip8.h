#pragma once

/*
	http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.1
*/
#include "Monitor.h"
#include "Keyboard.h"
#include "Beeper.h"

#include <cstdint>

namespace Chip8Emu {
	class Chip8 {
	private:
		uint8_t memory[4096] = {0},
			v[16] = {0},//registers
			delayTimer = 0, 
			soundTimer = 0,
			sp = 0;//stack pointer
		uint16_t i = 0,//index, stores some addresses
			pc = 0x200;//program counter
		uint16_t stack[16] = {0};

		Monitor *monitor_;
		Keyboard *keyboard_;
		Beeper *beeper_;
		bool paused = false;
		uint16_t speed_;
	public:
		Chip8(Monitor *monitor, Keyboard *keyboard, Beeper *beeper, uint16_t speed);
		void cycle();
		void executeInstruction(const uint16_t instruction);
		void loadProgram(const uint8_t *program_data, const size_t byte_count);
		void loadSprites();
	};
}