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

        // getters
        std::vector<std::string>get_terminal_feedback() { return m_terminal_feedback; }
        std::vector<std::string>get_errors() { return m_errors; }
        bool still_busy() { return m_still_working; }
        
        // meat and potatoes
        std::string exec(const char *cmd, bool realtime_terminal_output);   // executes a command in the terminal (can run headless by passing FALSE to 2nd arg)

    private:
        bool m_still_working;   // indicates if the last command entered is still working. Kind of redundant, tbh, because your program can't do anything until it's done, anyway.
        std::vector<std::string> m_terminal_feedback;   // all console output is returned line-by-line as a vector of strings
        std::vector<std::string> m_errors;              // any errors returned by the PIPE will be stored here (this does NOT include errors from your command, itself. ONLY PIPE errors)
};

command::command() {
    m_still_working = false;    // default. Keeps track of whether or not it's currently executing a command or still getting feedback from the terminal.
}

command::~command() {}

std::string command::exec(const char* cmd, bool realtime_terminal_output) {
    m_terminal_feedback.clear();
    m_errors.clear();
    if (!still_busy()) {
        m_still_working = true;
        char buffer[128];
        FILE* pipe = popen(cmd, "r");
        if (!pipe) {
            m_errors.push_back("popen() failed!");
            if (realtime_terminal_output)
            	std::cout << "popen() failed!" << std::endl;
            throw::std::runtime_error("popen() failed!");
        }
        try {
            while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL) {
                	std::string temp = buffer;
                    m_terminal_feedback.push_back(temp);
                    if (realtime_terminal_output)
                    	std::cout << temp;
                }
            }
        } catch (...) {
            pclose(pipe);
            m_still_working = false;
            m_errors.push_back("fgets failed!");
            if (realtime_terminal_output)
            	std::cout << "fgets failed!" << std::endl;
            throw;
            return "fgets failed!";
        }
        pclose(pipe);
        m_still_working = false;
        return "ok";
    }
    else {
        m_errors.push_back("busy. Try again later.");
        if (realtime_terminal_output)
        	std::cout << "busy. Try again later." << std::endl;
        return "busy";
    }
}

#endif // COMMAND_HPP
