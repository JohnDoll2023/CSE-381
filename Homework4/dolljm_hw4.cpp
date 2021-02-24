#ifndef DOLLJM_HW4_CPP
#define DOLLJM_HW4_CPP

/**
 * This source file contains the implementation for the various
 * methods defined in the ChildProcess class.
 *
 * Copyright (C) 2020 raodm@miamioh.edu
 */

// All the necessary #includes are already here
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "dolljm_hw4.h"

/** NOTE: Unlike Java, C++ does not require class names and file names
 * should match.  Hence when defining methods pertaining to a specific
 * class, the class
 */

// This method is just a copy-paste from lecture notes. This is done
// to illustrate an example.
void
dolljm_hw4::myExec(StrVec argList) {
    std::vector<char*> args;    // list of pointers to args
    for (auto& s : argList) {
        args.push_back(&s[0]);  // address of 1st character
    }
    // nullptr is very important
    args.push_back(nullptr);
    // Make execvp system call to run desired process
    execvp(args[0], &args[0]);
    // In case execvp ever fails, we throw a runtime execption
    throw std::runtime_error("Call to execvp failed!");
}

// Implement the constructor
dolljm_hw4::dolljm_hw4() : childPid(-1) {
    // childPid is initialized and not assigned!  Hence body is empty.
}

// Implement the destructor.  The destructor is an empty method
// because this class does not have any resources to release.
dolljm_hw4::~dolljm_hw4() {
    // Maybe it is a good idea to call wait() method here to ensure
    // that the child process does not get killed? On the other hand,
    // if we call wait() here and the child process misbehaves, then
    // it will appear as if this program is buggy -- Awwww.. -- no
    // good choice.  In this situation, a good API is to leave it to
    // the user to decide what is the best course of action.
}

// Use the comments in the header to implement the forkNexec method.
// This is a relatively simple method with an if-statement to call
// myExec in the child process and just return the childPid in parent.
int
dolljm_hw4::forkNexec(const StrVec& strVec) {
    // Fork and save the pid of the child process
    childPid = fork();
    // Call the myExec helper method in the child
    if (childPid == 0) {
        // We are in the child process
        myExec(strVec);
    }
    // Control drops here only in the parent process!
    return childPid;
}

// Use the comments in the header to implement the wait method.  This
// is a relatively simple method which uses waitpid call to get
// exitCode as shown in Slide #6 of ForkAndExec.pdf
int
dolljm_hw4::wait() const {
    int exitCode = 0;  // Child process's exit code
    waitpid(childPid, &exitCode, 0);  // wait for child to finish
    return exitCode;
}

// Reads input from user and sends back appropriate output with use of helper
// methods
void
dolljm_hw4::process(std::istream& is, const std::string& prompt, bool 
                      parallel) {
    // create line and word to iterate through text input
    std::string line, word;
    // keeps continuous input coming from user unless they enter "exit"
    while (std::cout << prompt, std::getline(std::cin, line) && line !="exit") {
        // processes line if it is not empty or a comment
        if (!line.empty() && line != "\r" && line.find_first_of("#", 0) != 0) {
            // create vector and stringstream to hold command line input
            StrVec cmd;
            std::istringstream is(line);
            // add each word from command line to a vector
            while (is >> std::quoted(word)) {
                cmd.push_back(word);
            }
            // checks which method of execution was selected and runs
            // correspondingly for parallel, serial, and unspecified
            if (cmd[0] == "PARALLEL") {
                parallelCmd(cmd);
            } else if (cmd[0] == "SERIAL") {
                serialCmd(cmd);
            } else {
                // forks process and then calls myExec
                int pid = forkNexec(cmd);
                // runs if parent process, prints out command and exit code
                if (pid != 0) {
                    print(cmd);
                    waitpid(pid, nullptr, 0);
                    std::cout << "Exit code: " << wait() << std::endl;
                }
            }
        }
    }
}

// parallel command calls helper method
void
dolljm_hw4::parallelCmd(StrVec cmd) {
    processURL(cmd[1]);
}

// serial command calls helper method
void
dolljm_hw4::serialCmd(StrVec cmd) {
    processURL(cmd[1]);
}

// goes through URL and reads in data from it
void
dolljm_hw4::processURL(std::string url) {
    // create vector to store words from url
    std::vector<std::string> cmd;
    // create helper strings to get the specific url details
    std::string hostname, port, path, word;
    // call helper method to give us the parts of the url we need
    std::tie(hostname, port, path) = breakDownURL(url);
    // creates a stream from url with hostname and port
    boost::asio::ip::tcp::iostream data(hostname, port);
    // go through all unneeded http response headers
    for (std::string hdr; std::getline(data, hdr) &&
             !hdr.empty() && hdr != "\r";) {}
    // send our commands over to be processed
    process(data, "> ", false);
}

// returns us the pieces of the url that we need
std::tuple<std::string, std::string, std::string>
dolljm_hw4::breakDownURL(const std::string& url) {
    // The values to be returned.
    std::string hostName, port = "80", path = "/";
    
    // Extract the substrings from the given url into the above
    // variables.  This is very simple 174-level logic problem.

    // First find index positions of sentinel substrings to ease
    // substring operations.
    const size_t hostStart = url.find("//") + 2;
    const size_t pathStart = url.find('/', hostStart);
    const size_t portPos   = url.find(':', hostStart);
    // Compute were the hostname ends. If we have a colon, it ends
    // with a ":". Otherwise the hostname ends with the last
    const size_t hostEnd   = (portPos == std::string::npos ? pathStart :
                              portPos);
    // Now extract the hostName and path and port substrings
    hostName = url.substr(hostStart, hostEnd - hostStart);
    path     = url.substr(pathStart);
    if (portPos != std::string::npos) {
        port = url.substr(portPos + 1, pathStart - portPos - 1);
    }
    // Return 3-values encapsulated into 1-tuple.
    return {hostName, port, path};
}

// Prints out the command back to the output
void
dolljm_hw4::print(StrVec cmd) {
    // preface before rest of output to signal what is being executed
    std::cout << "Running:";
    // each input is now repeated back to output
    for (size_t i = 0; i < cmd.size(); i++) {
        std::cout << " " << cmd[i];
    }
    // end the current line
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    // create class object
    dolljm_hw4 p;
    // call process method to run program
    p.process();
    return 0;
}
#endif
