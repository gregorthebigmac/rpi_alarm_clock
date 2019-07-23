#ifndef SOUND_H
#define SOUND_H

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class sound {	
public:
	sound();	// ctor
	virtual ~sound();	// dtor
	
	// getters
	int get_vol() { return m_volume; }
	int get_vol_increment() { return m_vol_increment; }
	bool is_playing() { return Mix_PlayingMusic();}
	
	// setters
	void set_vol(int vol);
	void set_vol_increment(int vol_inc) { m_vol_increment = vol_inc; }
	void vol_up();
	void vol_down();
	
	// meat & potatoes
	void play(std::string filename);
	void stop();
	void delay(int delay_in_milliseconds) { SDL_Delay(delay_in_milliseconds); }

private:
	std::string m_audio_file;	// WARNING! 44.1kHz OGG files ONLY! Anything else can cause weird behavior!
	int m_volume;
	int m_vol_increment;
	Mix_Music* m_music;
};

sound::sound() {
	if (SDL_Init(SDL_INIT_AUDIO) != 0)
		std::cerr << "SDL_Init ERROR: " << SDL_GetError() << std::endl;
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) != 0)
		std::cerr << "Mix_OpenAudio ERROR: " << Mix_GetError() << std::endl;
	set_vol(0);
	set_vol_increment(10);
}

sound::~sound() {}

void sound::set_vol(int vol) {
	m_volume = vol;
	Mix_VolumeMusic(m_volume);
}

void sound::vol_up() {
	m_volume = m_volume + m_vol_increment;
	if (m_volume > 100)
		m_volume = 100;
	Mix_VolumeMusic(m_volume);
}

void sound::vol_down() {
	m_volume = m_volume - m_vol_increment;
	if (m_volume < 0)
		m_volume = 0;
	Mix_VolumeMusic(m_volume);
}

void sound::play(std::string filename) {
	m_audio_file = filename;
	m_music = Mix_LoadMUS(m_audio_file.c_str());
	if (m_music) {
		Mix_VolumeMusic(m_volume);
		if (Mix_PlayMusic(m_music, 1) != 0) {
			std::cerr << "Mix_PlayMusic ERROR: " << Mix_GetError() << std::endl;
			Mix_FreeMusic(m_music);
		}
	}
	else {
		std::cerr << "Mix_LoadMuS ERROR: " << Mix_GetError() << std::endl;
		Mix_CloseAudio();
	}
}

void sound::stop() {
	if (is_playing())
		Mix_HaltMusic();
	if (m_music) {
		set_vol(0);
		Mix_FreeMusic(m_music);
	}
}

#endif
