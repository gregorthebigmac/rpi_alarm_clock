#include <command.hpp>	// custom class I wrote
#include "sound.h"	// custom class I wrote (essentially just a simple use of SDL-mixer)
#include <ctime>
#include <unistd.h>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;
using std::vector;
using std::string;

bool alarm_is_engaged = false;
enum weight { underweight, nominal, overweight };

command cmd;

void check_alarm_time();
void get_current_time();
void decide_sleep_time();
int calculate_sleep_time();
enum weight check_weight();
void alarm_engage();
void disengage_alarm();
int parse_string(string &str_data, char target_char);

// current time. It's checked frequently enough that it warrants global variables.
int c_year	 = 0,
	c_month	 = 0,
	c_day	 = 0,
	c_hour	 = 0,
	c_minute = 0,
	c_second = 0;

// target time. It's checked frequently enough that it warrants global variables.
int a_year	 = 0,
	a_month	 = 0,
	a_day	 = 0,
	a_hour	 = 0,
	a_minute = 0,
	a_second = 0;

int main(int argc, char *argv[]) {
	while(true) {
		if (alarm_is_engaged)
			check_weight();
		else {
			check_alarm_time();
			get_current_time();
			decide_sleep_time();
		}
	}
	return 0;
}

void check_alarm_time() {
	// first we get the target time, then we compare that to the current time
	vector<string>alarm_data;
	// get alarm time
	std::ifstream fin;
	fin.open("./dataalarm.time");
	if (fin.is_open()) {
		while(!fin.eof()) {
			string temp_data;
			getline(fin, temp_data);
			alarm_data.push_back(temp_data);
		}
		fin.close();
	}
	else { cout << "failed to open [alarm.time]. please check your file." << endl; }
	if (alarm_data.size() > 0) {
		bool finished = false;
		while (!finished) {
			if (alarm_data.at(0).size() > 0) {
				a_year 	 = parse_string(alarm_data[0], '-');
				a_month  = parse_string(alarm_data[0], '-');
				a_day 	 = parse_string(alarm_data[0], ' ');
				a_hour 	 = parse_string(alarm_data[0], ':');
				a_minute = parse_string(alarm_data[0], ':');
				a_second = parse_string(alarm_data[0], ' ');
				if ((a_year > -1) && (a_month > -1) && (a_day > -1) && (a_hour > -1) && (a_minute > -1) && (a_second > -1)) {
					alarm_data.clear();
					alarm_data.resize(0);
					finished = true;
				}
			}
		}
	}
}

void get_current_time() {
	time_t t = time(0);
	struct tm * current_time = localtime ( & t );
	c_year	 = current_time->tm_year + 1900;
	c_month	 = current_time->tm_mon + 1;
	c_day	 = current_time->tm_mday;
	c_hour	 = current_time->tm_hour;
	c_minute = current_time->tm_min;
	c_second = current_time->tm_sec;
}

void decide_sleep_time() {
	if (a_year < (c_year + 1)) {
		if (a_month < (c_month + 1)) {
			if (a_day < (c_day + 1)) {
				if (a_hour < (c_hour + 1)) {
					if (a_minute < c_minute) {
						while (!alarm_is_engaged) {
							if ((a_second - c_second) < 60 ) {
								if (a_second <= c_second) { alarm_engage(); }
								else { check_weight();} 
							}
							else { check_weight(); }
							get_current_time();
						}
					}
					else { sleep(calculate_sleep_time()); }
				}
			}
		}
	}
}

int calculate_sleep_time() {
	int day_diff = (a_day - c_day) * (3600 * 24);
	int hr_diff = (a_hour - c_hour) * 3600;
	int min_diff = (a_minute - c_minute) * 60;
	int sec_diff = a_second - c_second;
	float total_diff = day_diff + hr_diff + min_diff + sec_diff;
	return (total_diff * 0.5);
}

enum weight check_weight() {
	cmd.exec("sudo /home/gfox/cpp/vscode/serial/btest getweight", true);
	vector<string> serial_weight;
	serial_weight = cmd.get_terminal_feedback(0);
}

void alarm_engage() {
	
}

void disengage_alarm() {
	
}

int parse_string(string &str_data, char target_char) {
	int found = str_data.find(target_char);
	if (found != string::npos) {
		string temp_data = str_data.substr(0, found);
		str_data.erase(0, (found+1));
		std::stringstream str2i(temp_data);
		int target_int;
		str2i >> target_int;
		return target_int;
	}
	else { cout << "Error: [" << target_char << "] not found in string [" << str_data << "]." << endl; }
}