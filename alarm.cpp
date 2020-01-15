#include <iostream>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <sstream>
#include <fstream>
#include "command.hpp"
#include "sound.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

// simple struct for tracking time
struct time_tracker {
	int hr, min, sec;
};

// functions
int weight_check();
void get_time_now(time_tracker &tt_time);
void load_alarm_time();
int parse_string(string &str_data, char target_char);
bool is_alarm_now();
void write_error_to_file(string s_error);

time_tracker tt_alarm_begin, tt_alarm_end;	// establishing window of alarm period
time_t t_comp_begin, t_comp_end;	// used for tracking passing of time
time_tracker tt_time_now;	// current time
sound snd;
command cmd;

// global variables... deal with it!
const int overweight  = 170000;	// these are raw ADC readings from load cells, not converted to human-readable units!
const int underweight = 130000;	// these are raw ADC readings from load cells, not converted to human-readable units!
int weight_reading;
string path = "/home/gfox/git_repos/rpi_alarm_clock/";
string alarm_filename = path + "data/alarm.ogg";
bool time_to_wake_up = false;
bool special_time = false;

int main(int argc, char *argv[]) {
	snd.set_vol(0);
	snd.set_vol_increment(5);
	load_alarm_time();
	while (true) {
		get_time_now(tt_time_now);
		time_to_wake_up = is_alarm_now();
		if (time_to_wake_up) {
			weight_reading = weight_check();
			cout << "weight = [" << weight_reading << "] ";
			if ((weight_reading > underweight) && (weight_reading < overweight)) {
				cout << "NOMINAL" << endl;
				if (snd.is_playing()) {
					snd.set_vol(0);
					snd.stop();
				}
			}
			else if (weight_reading < underweight) {
				cout << "UNDER WEIGHT! ALARM!" << endl;
				if (snd.is_playing()) {
					t_comp_end = time(0);	// setting to time now
					if (t_comp_end > t_comp_begin) {
						snd.vol_up();
						cout << "vol = [" << snd.get_vol() << "]" << endl;
						t_comp_begin = time(0);
					}
				}
				else {
					t_comp_begin = time(0);
					snd.play(alarm_filename);
					cout << "vol = [" << snd.get_vol() << "]" << endl;
				}
			}
			else if (weight_reading > overweight) {
				cout << "OVER WEIGHT! ALARM!" << endl;
				if (snd.is_playing()) {
					t_comp_end = time(0);	// setting to time now
					if (t_comp_end > t_comp_begin) {
						snd.vol_up();
						cout << "vol = [" << snd.get_vol() << "]" << endl;
						t_comp_begin = time(0);
					}
				}
				else {
					t_comp_begin = time(0);
					snd.play(alarm_filename);
					cout << "vol = [" << snd.get_vol() << "]" << endl;
				}
			}
			else { cout << "Weight check failed! I don't know how I got here..." << endl; }
		}
		else { cout << "zzz..." << endl; }
		usleep(1000 * 2000);	// sleep for 2 sec
		cout << endl;
	}
	return 0;
}

int weight_check() {
	vector<string> serial_weight;
	vector<string> error_list;
	string cmd_str = "sudo " + path + "rpi_serial getweight";
	cmd.exec(cmd_str.c_str(), serial_weight, error_list, true);
	if (error_list.size() > 0) {
		for (int i = 0; i < error_list.size(); i++) {
			cout << error_list[i] << endl;
		}
		return -1;
	}
	std::stringstream str2i(serial_weight[0]);
	int weight = 0;
	str2i >> weight;
	return weight;
}

void get_time_now(time_tracker &tt_time) {
	time_t t = time(0);
	struct tm * current_time = localtime ( & t );
	tt_time.hr = current_time->tm_hour;
	tt_time.min = current_time->tm_min;
	tt_time.sec = current_time->tm_sec;
}

void load_alarm_time() {
	vector<string> alarm_data;
	std::ifstream fin;
	string alarm_time_filename = path + "data/alarm.time";
	fin.open(alarm_time_filename.c_str());
	if (fin.is_open()) {
		while(!fin.eof()) {
			string temp_data;
			getline(fin, temp_data);
			alarm_data.push_back(temp_data);
		}
		fin.close();
	}
	else {
		write_error_to_file("failed to open [alarm.time]. Please check your file!");
		return;
	}
	if (alarm_data.size() > 0) {
		bool finished = false;
		while (!finished) {
			if (alarm_data.at(0).size() > 0) {
				tt_alarm_begin.hr  = parse_string(alarm_data[0], ':');
				tt_alarm_begin.min = parse_string(alarm_data[0], ':');
				tt_alarm_begin.sec = parse_string(alarm_data[0], ' ');
				if ((tt_alarm_begin.hr > -1) && (tt_alarm_begin.min > -1) && (tt_alarm_begin.sec > -1)) {
					tt_alarm_end = tt_alarm_begin;
					if (tt_alarm_end.hr > 21)
						tt_alarm_end.hr = tt_alarm_end.hr -22;
					else
						tt_alarm_end.hr = tt_alarm_end.hr + 2;
					finished = true;
				}
				else {
					string alarm_error = "Error: Alarm time failed to parse from file. Please check data/alarm.time!";
					cout << alarm_error << endl;
					write_error_to_file(alarm_error);
					finished = true;
				}
			}
			else {
				write_error_to_file("Error: alarm.time appears to be empty! Please check data/alarm.time!");
				finished = true;
			}
		}
	}
	else { write_error_to_file("Error: alarm.time appears to be empty! Please check data/alarm.time!"); }
}

int parse_string(string &str_data, char target_char) {
	int found = str_data.find(target_char);
	if (found != string::npos) {
		string temp_data = str_data.substr(0, found);
		str_data.erase(0, (found + 1));
		std::stringstream str2i(temp_data);
		int target_int;
		str2i >> target_int;
		return target_int;
	}
	else {
		string s_char;
		s_char[0] = target_char;
		string s_error = "Error: [" + s_char + "] not found in string [" + str_data + "].";
		write_error_to_file(s_error);
		return -1;
	}
}

bool is_alarm_now() {
	bool alarm_is_now = false;
	bool time_adjusted = false;
	
	cout << "alarm time is: " << tt_alarm_begin.hr << ":" << tt_alarm_begin.min << ":" << tt_alarm_begin.sec << endl;
	cout << "time now is: " << tt_time_now.hr << ":" << tt_time_now.min << ":" << tt_time_now.sec << endl;

	if ((tt_alarm_begin.hr > 21) && (tt_time_now.hr < 2)) {
		time_adjusted = true;
		tt_time_now.hr = tt_time_now.hr + 24;
		tt_alarm_end.hr = tt_alarm_end.hr + 24;
	}
	if (tt_time_now.hr == tt_alarm_begin.hr) {
		if (tt_time_now.min >= tt_alarm_begin.min)
			alarm_is_now = true;
		else
			alarm_is_now = false;
	}
	else if (tt_time_now.hr == tt_alarm_end.hr) {
		if (tt_time_now.min <= tt_alarm_end.min)
			alarm_is_now = true;
		else
			alarm_is_now = false;
	}
	else if ((tt_time_now.hr > tt_alarm_begin.hr) && (tt_time_now.hr < tt_alarm_end.hr))
		alarm_is_now = true;
	else
		alarm_is_now = false;
	
	if (time_adjusted) {
		tt_time_now.hr = tt_time_now.hr - 24;
		tt_alarm_end.hr = tt_alarm_end.hr - 24;
	}
	return alarm_is_now;
}

void write_error_to_file(string s_error) {
	std::ofstream fout;
	string filename = path + "data/error.log";
	fout.open(filename.c_str(), std::ios::out | std::ios::app);
	if (fout.is_open()) {
		fout << s_error << endl;
		fout.close();
	}
	else { cout << "failed to open [error.log]. You got some issues!" << endl; }
}
