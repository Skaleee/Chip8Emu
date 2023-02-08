#include "Beeper.h"

#include <math.h>
#include <iostream>

Chip8Emu::Beeper::Beeper(int frequency, int volume) {
	Beeper::frequency = frequency;
	Beeper::volume = volume;

	SDL_AudioSpec want;
	want.freq = 44100;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = 2048;
	want.callback = audioCallback;
	want.userdata = this;

	audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);//SDL_AUDIO_ALLOW_ANY_CHANGE);//audio format coult be wrong/not supported. check variable 4 "have".
	if (audioDevice < 0) {
		throw std::runtime_error(std::string("Could not open audio device! SDL_Error: ").append(SDL_GetError()));
	}
}

Chip8Emu::Beeper::~Beeper() {
	SDL_CloseAudioDevice(audioDevice);
}

void Chip8Emu::Beeper::audioCallback(void* userdata, uint8_t* stream, int len) {
	static const double pi = 3.14159265359;

	Beeper *beeper = (Beeper*)userdata;

	int16_t *buffer = (int16_t*)stream;//stream has 2 bytes per sample -> len/2 is sample count

	static int sampleNr = 0;
	for (int i = 0; i < len/2; i++, sampleNr++) {
		if(sampleNr > 44100)
			sampleNr = 0;
		buffer[i] = (int16_t)(beeper->volume*sin(pi*2*beeper->frequency*((double)sampleNr/44100)));
	}
}

void Chip8Emu::Beeper::startBeeping() {
	SDL_PauseAudioDevice(audioDevice, 0);
}

void Chip8Emu::Beeper::stopBeeping() {
	SDL_PauseAudioDevice(audioDevice, 1);
}