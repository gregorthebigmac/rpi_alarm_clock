#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

class command {
public:
    command();
    virtual ~command();        
    
	// main function. Fire and forget. Takes two args. command to execute as c_str, and bool: TRUE to print terminal output in realtime, FALSE to run quiet (or headless)
    void exec(const char *cmd, bool realtime_terminal_feedback);
	// overloaded main function. Takes additional two args, both are vector<string> for storing terminal feedback and any errors that may arise.
	void exec(const char *cmd, std::vector<std::string> &terminal_output, std::vector<std::string> &error_list, bool realtime_terminal_feedback);
};

command::command() {}
command::~command() {}

void exec(const char *cmd, bool realtime_terminal_feedback) {
	char buffer[128];
	FILE *_pipe = popen(cmd, "r");
	if (!_pipe) {
		if (realtime_terminal_feedback)
			std::cout << "popen() failed!" << std::endl;
		throw ::std::runtime_error("popen() failed!");
        try {
            while (!feof(_pipe)) {
                if (fgets(buffer, 128, _pipe) != NULL) {
                	std::string temp = buffer;
					if (realtime_terminal_feedback)
						std::cout << temp;
                }
            }
        } catch (...) {
            pclose(_pipe);
            if (realtime_terminal_feedback)
            	std::cout << "unknown error" << std::endl;
            throw;
        	std::cout << "Failed to fetch data from FILE *_pipe!";
        }
        pclose(_pipe);
    }
}

void command::exec(const char *cmd, std::vector<std::string> &terminal_output, std::vector<std::string> &error_list, bool realtime_terminal_feedback) {
	char buffer[128];
	FILE *_pipe = popen(cmd, "r");
	if (!_pipe) {
		error_list.push_back("popen() failed!");
		if (realtime_terminal_feedback)
			std::cout << "popen() failed!" << std::endl;
		throw ::std::runtime_error("popen() failed!");
        try {
            while (!feof(_pipe)) {
                if (fgets(buffer, 128, _pipe) != NULL) {
                	std::string temp = buffer;
                    terminal_output.push_back(temp);
					if (realtime_terminal_feedback)
						std::cout << temp;
                }
            }
        } catch (...) {
            pclose(_pipe);
            error_list.push_back("Failed to fetch data from FILE *_pipe!");
            if (realtime_terminal_feedback)
            	std::cout << "unknown error" << std::endl;
            throw;
        	std::cout << "Failed to fetch data from FILE *_pipe!";
        }
        pclose(_pipe);
    }
}

#endif // COMMAND_HPP
