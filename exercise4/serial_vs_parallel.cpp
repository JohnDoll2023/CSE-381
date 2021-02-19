/**
 * A simple source file to keep parts of the exercise nicely organized.
 * Copyright (C) 2021 John Doll
 */

#include "ChildProcess.h"

/** A simple method that must be modified to run 3 processes in
    parallel. The starter code for this method is included in the
    exercise document.  You can copy-paste the starter code from the
    document (ensure formatting is good) and then modify it.
    
    \note This method is already called from main()
*/
void runProcesses() {
    // The command to be run -- this just sleeps for 5 seconds.
    const StrVec cmd = {"sleep", "5"};
    // Run the same process 3 times in serial fashion.
    ChildProcess cp1, cp2, cp3;
    // 1st run
    cp1.forkNexec(cmd);
    cp2.forkNexec(cmd);
    cp3.forkNexec(cmd);
    cp3.wait();
    // 2nd run
    // cp.forkNexec(cmd);
    cp2.wait();
    // 3rd run
    // cp.forkNexec(cmd);
    cp1.wait();
}

