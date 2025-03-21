#ifndef DOLLJM_HW4_H
#define DOLLJM_HW4_H

/**
 * This source file contains the definition for the ChildProcess
 * class.  This class provides a convenient API to fork and execute
 * other programs.
 *
 * Copyright (C) 2020 raodm@miamiOH.edu
 */

#include <string>
#include <vector>

// A convenience shortcut to a vector-of-strings
using StrVec = std::vector<std::string>;

// ------------------------------------------------------------------- //
// ****  NOTE: NEVER NEVER put "using namespace" IN A HEADER FILE  *** //
// ------------------------------------------------------------------- //

/**
 * A simple class to help with forking & executing programs in a child
 * process.  This class maintains the PID of the child process in an
 * instance variable.
 */
class dolljm_hw4 {
public:
    /** A simple default (no-argument) constructor.  This constructor
        merely initializes (so the body of the method should be
        empty!) the childPid instance variable to -1.
    */
    dolljm_hw4();
    
    /** The destructor. This method cleans-up any resources (like open
        files etc.). However, this class is very simple and the
        destructor is just an empty method.
        
        Note: Destructor for an object is called when the object goes
        out of scope.
    */
    ~dolljm_hw4();

    /** The primary method in this class that:

        1. First uses the fork system call to create a child process.
        2. In the child process it calls myExec to execute a program.
        3. In the parent process, it stores the value in childPid and
           returns the childPid value.

        \param[in] argList The list of command-line arguments.  The
        first entry is assumed to be the command to be executed. 
          
        \return This method returns the pid value of the child process
        forked by this method.
    */
    int forkNexec(const StrVec& argList);

    /** Helper method to wait for child process to finish.  This
        method calls the waitpid system call. It obtains the exit code
        of the child process from the 2nd argument of the waitpid
        system call.

        \return This method returns the exit code of the child
        process.
    */
    int wait() const;
    
    /**
     * Reads input stream, checks to see if line is blank or a comment, and then
     * sends appropriate lines to be read word by word into a vector. After this
     * the first word is checked to see what command to execute, and then from
     * there our data is sent off to helper methods
     * @param is input stream cin or other specified input
     * @param prompt sends > or specified string to user after completed entry
     */
    void process(std::istream& is = std::cin, const std::string& prompt = "> ");
    
    /**
     * Prints the running command with the whole command sent to output
     * @param cmd a vector of words to be sent to output
     */
    void print(StrVec cmd);
    
    /**
     * Checks whether the specified input should be run or parallel or serial
     * fashion and then sends off to helper method accordingly
     * @param cmd contains list of commands to be run
     */
    void parallelOrSerial(StrVec cmd);
    
    /**
     * Gets url and disassembles it so that we can create stream from it to
     * process incoming commands
     * @param url the url to be processed
     * @param parallel true if commands are supposed to be run parallel and
     * false otherwise
     */
    void processURL(std::string url, bool parallel);
    
    /**
     * Reads the input stream and sends lines with commands to a vector, which
     * is then looped through to print our running statements, and then is 
     * looped through to fork our commands. Then we loop through the vector once
     * more to get our exit codes before finishing
     * @param is the input stream to read the data from to be executed
     */
    void processParallel(std::istream& is);
    
    /**
     * Disassembles the url so that we can form a connection to it and read from
     * it
     * @param url the url
     * @return a tuple of the hostname, port and path
     */
    std::tuple<std::string, std::string, std::string>
    breakDownURL(const std::string& url);
    
protected:
    /** A helper method to setup pointers and call execvp system call.
        This method should be called from a child process.  Don't call
        this method directly.  Instead, call the forkNexec API method.

        NOTE: This method is a copy-paste from the lecture slides.
        
        \param[in] argList The list of command-line arguments.  The
        first entry is assumed to be the command to be executed.
    */
    void myExec(StrVec argList);
    
private:
    /** The only instance variable in this class.  It is initialized
        to -1 in the constructor.  The value is changed by the
        forkNexec method.
    */
    int childPid;
};

#endif
