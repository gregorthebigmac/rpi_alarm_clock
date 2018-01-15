#include "sound.h"

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
	Mix_VolumeMusic(m_volume);
}

void sound::vol_down() {
	m_volume = m_volume - m_vol_increment;
	Mix_VolumeMusic(m_volume);
}

bool sound::play(std::string filename) {
	m_audio_file = filename;
	m_music = Mix_LoadMUS(m_audio_file.c_str());
	if (m_music) {
		Mix_VolumeMusic(m_volume);
		if (Mix_PlayMusic(m_music, 1) == 0)
			return true;
		else {
			std::cerr << "Mix_PlayMusic ERROR: " << Mix_GetError() << std::endl;
			Mix_FreeMusic(m_music);
			return false;
		}
	}
	else {
		std::cerr << "Mix_LoadMuS ERROR: " << Mix_GetError() << std::endl;
		Mix_CloseAudio();
		return false;
	}
}

void sound::stop() {
	if (is_playing())
		Mix_HaltMusic();
	if (m_music)
		Mix_FreeMusic(m_music);
	Mix_CloseAudio();
}

void sound::delay(int delay_in_milliseconds) {
	SDL_Delay(delay_in_milliseconds);
}