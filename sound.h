#ifndef SOUND_H
#define SOUND_H

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class sound {
public:
	sound();
	virtual ~sound();
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
	bool play(std::string filename);
	void stop();
	void delay(int delay_in_milliseconds);
	
private:
	std::string m_audio_file;	// WARNING! 44.1kHz OGG files ONLY! Anything else can cause weird behavior!
	int m_volume;
	int m_vol_increment;
	Mix_Music* m_music;
};

#endif
