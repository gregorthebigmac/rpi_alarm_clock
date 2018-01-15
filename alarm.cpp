#include "sound.hpp"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <sstream>
#include <command.hpp>

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
	if (stream.fail()) {
		cout << "Error: failed to cast str_data to float! [" << numberAsString << "]." << endl;
	}
	else {
		return valor;
	}
}

bool alarm_is_engaged;
double weight;

int c_year	 = 0,
	c_month	 = 0,
	c_day	 = 0,
	c_hour	 = 0,
	c_minute = 0,
	c_second = 0;

int a_year	 = 0,
	a_month	 = 0,
	a_day	 = 0,
	a_hour	 = 0,
	a_minute = 0,
	a_second = 0;

sound snd;
command cmd;

double weight_check();
void get_time_now();
int get_second_diff();
void mark_time();
int counter = 0;

int main(int argc, char *argv[]) {
	cout << "starting..." << endl;
	snd.set_vol(0);
	while (true) {
		weight = weight_check();
		cout << "weight reads [" << weight << "]" << endl;
		mark_time();
		while ((weight > 149.9) || (weight < 109.9)) {
			get_time_now();
			if (snd.music_is_playing()) {
				if (get_second_diff() > 5) {
					snd.vol_up();
					mark_time();
				}
			}
			else {
				snd.play("/home/gfox/cpp/vscode/alarm_clock/song.ogg");
				mark_time();
			}
			weight = weight_check();
			cout << "weight reads [" << weight << "]" << endl;
			if (weight < 110.0)
				break;
		}
		while ((weight > 109.9) && (weight < 149.9)) {
			mark_time();
			if (snd.music_is_playing())
				snd.stop();
			weight = weight_check();
			cout << "weight reads [" << weight << "]" << endl;
		}
	}
}

double weight_check() {
	cout << "getting weight from arduino..." << endl;
	cmd.exec("sudo /home/gfox/cpp/vscode/serial/btest getweight", true);
	vector<string> serial_weight;
	serial_weight = cmd.get_terminal_feedback(counter);
	counter++;
	double weight = StringToNumber<double>(serial_weight[0]);
	cout << "weight reads [" << weight << "]" << endl;
	return weight;
}

void get_time_now() {
	time_t t = time(0);
	struct tm * current_time = localtime ( & t );
	c_year	 = current_time->tm_year + 1900;
	c_month	 = current_time->tm_mon + 1;
	c_day	 = current_time->tm_mday;
	c_hour	 = current_time->tm_hour;
	c_minute = current_time->tm_min;
	c_second = current_time->tm_sec;
}

void mark_time() {
	time_t t = time(0);
	struct tm * current_time = localtime ( & t );
	a_year	 = current_time->tm_year + 1900;
	a_month	 = current_time->tm_mon + 1;
	a_day	 = current_time->tm_mday;
	a_hour	 = current_time->tm_hour;
	a_minute = current_time->tm_min;
	a_second = current_time->tm_sec;
}

int get_second_diff() {
	int result = abs(a_second - c_second);
	result = result + ( abs(a_minute - c_minute) * 60 );
	result = result + ( abs(a_hour - c_hour) * 3600 );
	return result;
}
