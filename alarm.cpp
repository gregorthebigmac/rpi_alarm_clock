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

// convert string to float
template<typename T>
T StringToNumber(const std::string& numberAsString) {
	T valor;
	std::stringstream stream(numberAsString);
	stream >> valor;
	if (stream.fail())
		cout << "Error: failed to cast str_data to float! [" << numberAsString << "]." << endl;
	else
		return valor;
}

// simple time struct to keep track of:
// - alarm time
// - beginning time tracker
// - end time tracker
// beginning and end time trackers are for counting every 10 seconds before increasing volume
struct time_tracker {
	int hour;
	int minute;
	int second;
};

time_tracker tt_alarm, tt_begin, tt_end;
sound snd;
command cmd;

// functions
double weight_check();
void get_time_now();
int get_second_diff();
int parse_string(string &str_data, char target_char);
void mark_time();
void load_alarm_time();
bool alarm_time_check();

// global variables
const double overweight = 170.0;
const double underweight = 120.0;
double actual_weight;
int counter = 0;
string alarm_filename = "data/alarm.ogg";
bool time_to_wake_up = false;

int main(int argc, char *argv[]) {
	cout << "starting..." << endl;
	snd.set_vol(0);
	while (true) {
		load_alarm_time();
		get_time_now();
		alarm_time_check();
		if (time_to_wake_up) {	
			actual_weight = weight_check();
			mark_time();
			while ((actual_weight >= overweight) || (actual_weight <= underweight)) {
				cout << "weight=[" << actual_weight << "] ALARM!" << endl;
				cout << "vol=[" << snd.get_vol() << "]" << endl;
				get_time_now();
				if (snd.is_playing()) {
					if (get_second_diff() > 5) {
						snd.vol_up();
						mark_time();
					}
				}
				else {
					snd.play(alarm_filename.c_str());
					mark_time();
				}
				actual_weight = weight_check();
			}
			while ((actual_weight > underweight) && (actual_weight < overweight)) {
				cout << "weight=[" << actual_weight << "] NOMINAL" << endl;
				cout << "vol=[" << snd.get_vol() << "]" << endl;
				mark_time();
				if (snd.is_playing())
					snd.stop();
				actual_weight = weight_check();
			}
		}
		else {
			usleep(1000 * 60 * 1000);	// sleep for 60 sec
		}
	}
}

double weight_check() {
	cmd.exec("sudo ./rpi_serial getweight", false);
	vector<string> serial_weight;
	serial_weight = cmd.get_terminal_feedback();
	double weight = StringToNumber<double>(serial_weight[0]);
	return weight;
}

void get_time_now() {
	time_t t = time(0);
	struct tm * current_time = localtime ( & t );
	tt_end.hour	  = current_time->tm_hour;
	tt_end.minute = current_time->tm_min;
	tt_end.second = current_time->tm_sec;
}

void mark_time() {
	time_t t = time(0);
	struct tm * current_time = localtime ( & t );
	tt_begin.hour	= current_time->tm_hour;
	tt_begin.minute = current_time->tm_min;
	tt_begin.second = current_time->tm_sec;
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
	else
		cout << "Error: [" << target_char << "] not found in string [" << str_data << "]." << endl;
}

int get_second_diff() {
	int result = abs(tt_end.second - tt_begin.second);
	result = result + ( abs(tt_end.minute - tt_begin.minute) * 60 );
	result = result + ( abs(tt_end.hour - tt_begin.hour) * 3600 );
	return result;
}

void load_alarm_time() {
	vector<string> alarm_data;
	std::ifstream fin;
	fin.open("data/alarm.time");
	if (fin.is_open()) {
		while(!fin.eof()) {
			string temp_data;
			getline(fin, temp_data);
			alarm_data.push_back(temp_data);
		}
		fin.close();
	}
	else
		cout << "failed to open [alarm.time]. please check your file!" << endl;
	if (alarm_data.size() > 0) {
		bool finished = false;
		while (!finished) {
			if (alarm_data.at(0).size() > 0) {
				tt_alarm.hour	= parse_string(alarm_data[0], ':');
				tt_alarm.minute = parse_string(alarm_data[0], ':');
				tt_alarm.second = parse_string(alarm_data[0], ' ');
				if ((tt_alarm.hour > -1) && (tt_alarm.minute > -1) && (tt_alarm.second > -1))
					finished = true;
				else {
					cout << "Error: Alarm time failed to parse from file. Please check file!" << endl;
					finished = true;
				}
			}
			else
				cout << "Error: alarm.time appears to be empty! Please check file!" << endl;
		}
	}
	else
		cout << "Error: alarm.time appears to be empty! Please check file!" << endl;
}

bool alarm_time_check() {
	cout << "alarm time is:   " << tt_alarm.hour << ":" << tt_alarm.minute << ":" << tt_alarm.second << endl;
	cout << "current time is: " << tt_end.hour << ":" << tt_end.minute << ":" << tt_end.second << endl;
	if ((tt_end.hour - tt_alarm.hour) == 0) {
		if ((tt_end.minute - tt_alarm.minute) > -1) {
			time_to_wake_up = true;
			cout << "Time to wake up!" << endl;
		}
		else {
			time_to_wake_up = false;
			cout << "zzz..." << endl;
		}
	}
	else if ((tt_end.hour - tt_alarm.hour) > 0) {
		cout << "Time to wake up!" << endl;
		time_to_wake_up = true;
	}
	else {
		cout << "zzz..." << endl;
		time_to_wake_up = false;
	}
}
