#include "Chip8.h"

#include <iostream>

Chip8Emu::Chip8::Chip8(Monitor *monitor, Keyboard *keyboard, Beeper *beeper, uint16_t speed) {
	monitor_ = monitor;
	keyboard_ = keyboard;
	beeper_ = beeper;
	speed_ = speed;
	loadSprites();
}

void Chip8Emu::Chip8::loadSprites() {
	const uint8_t sprites[5 * 8 * 16] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80, //F
	};

	for (int i = 0; i < 5 * 8 * 16; i++) {
		memory[i] = sprites[i];
	}
}

void Chip8Emu::Chip8::loadProgram(const uint8_t *program_data, size_t byte_count) {
	for (int i = 0; i < byte_count; i++) {
		if(0x200 + i >= 4096)
			break;
		memory[0x200 + i] = program_data[i];
	}
	/*for (int i = 0; i < 4096; i++) {
		std::cout<<i<<": "<< (int)memory[i] << std::endl;
	}*/
}

void Chip8Emu::Chip8::cycle() {
	for(int i = 0; i < speed_; i++){
		uint16_t nextInstruction = memory[pc]<<8 | memory[pc+1];
		//std::cout<<"next: "<<std::hex<<nextInstruction<<std::endl;

		pc += 2;
		executeInstruction(nextInstruction);
		//sound
	}

	if (!paused) {
		if (delayTimer > 0)
			delayTimer--;
		if (soundTimer > 0){
			soundTimer--;
			if(soundTimer <= 0)
				beeper_->stopBeeping();
		}
	}
}

void Chip8Emu::Chip8::executeInstruction(uint16_t instruction) {
	uint8_t x = (instruction & 0x0F00) >> 8;
	uint8_t y = (instruction & 0x00F0) >> 4;
	uint8_t n = instruction & 0xF;
	uint8_t kk = instruction & 0x00FF;

	switch (instruction & 0xF000) {
		case 0x0000:
			switch (kk) {
				case 0xE0://CLS
					monitor_->clear();
					break;
				case 0xEE://RET
					pc = stack[--sp];
					break;
				default:
					throw std::runtime_error("BAD OPCODE: "+instruction);
			}
			break;
		case 0x1000://JP addr
			pc = instruction & 0x0FFF;
			break;
		case 0x2000://CALL addr
			if(sp>=16)
				throw std::runtime_error("STACK OVERFLOW");
			stack[sp++] = pc;
			pc = instruction & 0x0FFF;
			break;
		case 0x3000://SE Vx, byte
			if(v[x] == kk) 
				pc += 2;
			break;
		case 0x4000://SNE Vx, byte
			if (v[x] != kk)
				pc += 2;
			break;
		case 0x5000://SE Vx, Vy
			if (n != 0)//last nibble must be 0
				throw std::runtime_error("BAD OPCODE: " + instruction);

			if (v[x] == v[y])
				pc += 2;
			break;
		case 0x6000://LD Vx, byte
			v[x] = kk;
			break;
		case 0x7000://ADD Vx, byte
			v[x] = v[x] + kk;
			break;
		case 0x8000:
			switch (n) {
				case 0x0://LD Vx, Vy
					v[x] = v[y];
					break;
				case 0x1://OR Vx, Vy
					v[x] = v[x] | v[y];
					break;
				case 0x2://AND Vx, Vy
					v[x] = v[x] & v[y];
					break;
				case 0x3://XOR Vx, Vy
					v[x] = v[x] ^ v[y];
					break;
				case 0x4://ADD Vx, Vy
					{
						uint16_t sum = v[x];
						sum += v[y];
						if(sum>0xFF)
							v[0xF] = 1;
						else
							v[0xF] = 0;
						v[x] = (uint8_t)(sum&0xFF);
					}
					break;
				case 0x5://SUB Vx, Vy
					if(v[x] > v[y])
						v[0xF] = 1;
					else
						v[0xF] = 0;
					v[x] = v[x] - v[y];
					break;
				case 0x6://SHR Vx {, Vy}
					v[0xF] = v[x] & 0x1;
					v[x] = v[x]>>1;
					break;
				case 0x7://SUBN Vx, Vy
					if(v[x] < v[y])
						v[0xF] = 1;
					else
						v[0xF] = 0;
					v[x] = v[y] - v[x];
					break;
				case 0xE://SHL Vx{ , Vy }
					if((v[x] & 0x80) == 0x80)
						v[0xF] = 1;
					else
						v[0xF] = 0;
					v[x] = v[x]<<1;
					break;
				default:
					throw std::runtime_error("BAD OPCODE: " + instruction);
			}
			break;
		case 0x9000://SNE Vx, Vy
			if (n != 0)//last nibble must be 0
				throw std::runtime_error("BAD OPCODE: " + instruction);

			if(v[x] != v[y])
				pc += 2;
			break;
		case 0xA000://LD I, addr
			i = instruction & 0x0FFF;
			break;
		case 0xB000://JP V0, addr
			pc = (instruction & 0x0FFF) + v[0];
			break;
		case 0xC000://RND Vx, byte
			v[x] = (std::rand() % 0x100) & kk;
			break;
		case 0xD000://DRW Vx, Vy, nibble
			v[0xF] = 0;
			for (int row = 0; row < n; row++) {
				uint8_t sprite = memory[i+row];
				for (int col = 0; col < 0x8; col++) {
					if ((sprite & 0x80) > 0) {
						if (monitor_->setPixel(v[x]+col,v[y]+row)) {
							v[0xF] = 1;
						}
					}
					sprite = sprite<<1;
				}
			}
			break;
		case 0xE000:
			switch (kk) {
				case 0x9E://SKP Vx
					if(keyboard_->isKeyPressed(v[x]))
						pc += 2;
					break;
				case 0xA1://SKNP Vx
					if (!(keyboard_->isKeyPressed(v[x])))
						pc += 2;
					break;
				default:
					throw std::runtime_error("BAD OPCODE: " + instruction);
			}
			break;
		case 0xF000:
			switch (kk) {
			case 0x07://LD Vx, DT
				v[x] = delayTimer;
				break;
			case 0x0A://LD Vx, K
				paused = true;
				{
					uint8_t const* keys = keyboard_->getPressedKeys();
					for (int i = 0; i <= 0xF; i++) {
						if (keys[i] != 0) {
							v[x] = keys[i];
							paused = false;
							break;
						}
					}
				}
				if(paused)
					pc -= 2;
				break;
			case 0x15://LD DT, Vx
				delayTimer = v[x];
				break;
			case 0x18://LD ST, Vx
				soundTimer = v[x];
				if(soundTimer > 0)
					beeper_->startBeeping();
				break;
			case 0x1E://ADD I, Vx
				i = i + v[x];
				break;
			case 0x29://LD F, Vx
				i = v[x]*5;
				break;
			case 0x33://LD B, Vx
				memory[i] = v[x] / 100;
				memory[i+1] = v[x] % 100 / 10;
				memory[i+2] = v[x] % 10;
				break;
			case 0x55://LD [I], Vx
				for (int b = 0; b <= x; b++)
					memory[i+b] = v[b];
				break;
			case 0x65://LD Vx, [I]
				for (int b = 0; b <= x; b++)
					v[b] = memory[i + b];
				break;
			default:
				throw std::runtime_error("BAD OPCODE: " + instruction);
			}
			break;
		default:
			throw std::runtime_error("BAD OPCODE: " + instruction);
	}
}