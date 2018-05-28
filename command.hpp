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
    // Fire and forget. Takes a terminal command as first arg, and set 2nd arg bool to true to see terminal output in real time, false for quiet.
    void exec(const char *cmd, bool realtime_terminal_feedback);
    // overloaded method. Takes two vectors of strings as args, and pushes all terminal output to the first one, and errors reported to the second one. same rules for original two args holds.
    void exec(const char *cmd, std::vector<std::string> &terminal_output, std::vector<std::string> &error_list, bool realtime_terminal_feedback);
};

command::command() {}   // ctor
command::~command() {}  // dtor

void command::exec(const char* cmd, bool realtime_terminal_feedback) {
    char buffer[128];
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        if (realtime_terminal_feedback)
            std::cout << "popen() failed!" << std::endl;
        throw::std::runtime_error("popen() failed!");
    }
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL) {
                if (realtime_terminal_feedback)
                    std::cout << buffer;
            }
        }
    } catch (...) {
        pclose(pipe);
        if (realtime_terminal_feedback)
            std::cout << "fgets failed!" << std::endl;
        throw;
    }
    pclose(pipe);
}

void command::exec(const char* cmd, std::vector<std::string> &terminal_output, std::vector<std::string> &error_list, bool realtime_terminal_feedback) {
    char buffer[128];
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        error_list.push_back("popen() failed!");
        if (realtime_terminal_feedback)
            std::cout << "popen() failed!" << std::endl;
        throw::std::runtime_error("popen() failed!");
    }
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL) {
                std::string temp = buffer;
                terminal_output.push_back(temp);
                if (realtime_terminal_feedback)
                    std::cout << temp;
            }
        }
    } catch (...) {
        pclose(pipe);
        error_list.push_back("fgets failed!");
        if (realtime_terminal_feedback)
            std::cout << "fgets failed!" << std::endl;
        throw;
    }
    pclose(pipe);
}

#endif // COMMAND_HPP
