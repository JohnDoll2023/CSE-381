/** 
 * A custom web-server that can process HTTP GET requests and:
 *    1. Respond with contents of a given HTML file
 *    2. Run a specified program and return the output from the process.
 *
 * Copyright (C) 2020 raodm@miamioh.edu.
 */

#include <sys/wait.h>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <thread>
#include <future>
#include <vector>
#include "HTTPFile.h"
#include "ChildProcess.h"
#include "HTMLFragments.h"

// Convenience namespace to streamline the code below.
using namespace boost::asio;
using namespace boost::asio::ip;

// Forward declaration for url_decode method defined below.  We need
// this just to organize the starter code into sections that students
// should not modify (to minimize problems for students).
std::string url_decode(std::string url);

// Forward declaration for serveClient that is defined further below.
void serveClient(std::istream& is, std::ostream& os, bool);

/**
 * Runs the program as a server that listens to incoming connections.
 * 
 * @param port The port number on which the server should listen.
 */
void runServer(int port) {
    // Setup a server socket to accept connections on the socket
    io_service service;
    // Create end point
    tcp::endpoint myEndpoint(tcp::v4(), port);
    // Create a socket that accepts connections
    tcp::acceptor server(service, myEndpoint);
    std::cout << "Server is listening on "
              << server.local_endpoint().port()
              << " & ready to process clients...\n";
    // Process client connections one-by-one...forever
    using TcpStreamPtr = std::shared_ptr<tcp::iostream>;
    while (true) {
        // Create garbage-collected, shared object on heap so we can
        // send it to another thread and not worry about life-time of
        // the socket connection.
        TcpStreamPtr client = std::make_shared<tcp::iostream>();
        // Wait for a client to connect    
        server.accept(*client->rdbuf());
        // Process request from client on a separate background thread
        // I am using a lambda here so that I don't have to write
        // another method (no, not lazy, just simple :-)).
        std::thread thr([client](){ serveClient(*client, *client, true); });
        thr.detach();
    }
}

void threadMain(int pid, std::string &stats, bool genChart) {
    // declare exitCode and count as 0
    int exitCode = 0;
    int count = 0;
    // create fileName string and line and json strings
    std::string fileName = "/proc/" + std::to_string(pid) + "/stat";
    std::string line, json ="";
    // while loop that uses waitpid to continue until process is done
    while (waitpid(pid, &exitCode, WNOHANG) == 0) {
        // sleep for a second so we can get stats every second
        sleep(1);
        // create stream to read from /proc file
        std::ifstream file(fileName);
        // get stats from proc file
        std::getline(file, line);
        // use boost split to put all the stat values into nums array for easier
        // access
        std::vector<std::string> nums;
        boost::split(nums, line, boost::is_any_of(" "));
        // operate on numbers to get them in the units we want
        int utime = std::round(stoi(nums[13])/sysconf(_SC_CLK_TCK));
        int stime = std::round(stoi(nums[14])/sysconf(_SC_CLK_TCK));
        int mem = stoi(nums[22])/1000000;
        // add certain values from nums that we want for stats string for output
        stats += "<tr><td>" + std::to_string((++count)) + "</td><td>" + 
                std::to_string(utime) + "</td><td>" + std::to_string(stime) + 
                "</td><td>" + std::to_string(mem) + "</td></tr>\n";
        // if we want to generate a chart, then we add our stats to our json 
        // array
        if (genChart) {
            json += ",\n[" + std::to_string(count) + ", " + std::to_string(utime
                    + stime) + ", " + std::to_string(mem) + "]";
        }
    }
    // put chunked response together
    stats += htmlMid2 + json + htmlEnd;
}

/**
 * This is pretty much a copy-paste from a previous exercise solution.
 * This method uses the ChildProcess class to run a program and sends
 * the program outputs as HTTP chunked-responses.
 *
 * @param cmd The command to be executed. This command should be already
 * url_decoded and is in the form "ls -la ./"
 *
 * @param os The output stream to where the output from the command is
 * to be written in chunked HTTP response format.
 * 
 * @param genChart If flag is true, then graph data is also printed.
 */
void sendCmdOutput(std::string& cmd, std::ostream& os, bool genChart) {
    // Split the command into words for processing using helper method
    // in ChildProcess.
    const StrVec args = ChildProcess::split(cmd);
    // Create the child process.
    ChildProcess cp;
    // get pid from cp
    int pid = cp.forkNexecIO(args);
    // create stats string to store data and html that we'll need to print
    std::string stats = htmlMid1;
    // create thread for the process
    std::thread thread(threadMain, pid, std::ref(stats), genChart);
    // Get the outputs from the child process.
    std::istream& is = cp.getChildOutput();
    // Send the output from child process to the client as chunked response
    os << http::DefaultHttpHeaders << "text/html\r\n\r\n";
    // print pre-built html chunk
    os << std::hex << htmlStart.size() << "\r\n" << htmlStart << "\r\n";
    // Send each line as a separate chunk.
    for (std::string line; std::getline(is, line);) {
        line += "\n";  // Add the newline that was not included by getline
        os << std::hex << line.size() << "\r\n" << line << "\r\n";
    }
    // It is important to wait for the process to finish. Internally,
    // the method below calls waitpid system call.
    int exitCode = cp.wait();
    const std::string line = "Exit code: " + std::to_string(exitCode) + "\n";
    os << std::hex << line.size() << "\r\n" << line << "\r\n";
    // rejoin thread
    thread.join();
    // print chunk that we just edited and combined with pre-built chunks
    os << std::hex << stats.size() << "\r\n" << stats << "\r\n";
    // Finally send the trailing end-of-stream chunk
    os << "0\r\n\r\n";
}

//------------------------------------------------------------------
//  DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//------------------------------------------------------------------

/**
 * Process HTTP request (from first line & headers) and
 * provide suitable HTTP response back to the client.
 * 
 * @param is The input stream to read data from client.
 *
 * @param os The output stream to send data to client.
 * 
 * @param genChart If flag is true, then graph data is also printed.
 */
void serveClient(std::istream& is, std::ostream& os, bool genChart) {
    // Read headers from client and print them. This server
    // does not really process client headers
    std::string line, path;
    // Read the "GET" word and then the path.
    is >> line >> path;
    // Skip/ignore all the HTTP request & headers for now.
    while (std::getline(is, line) && (line != "\r")) {}
    // Check and dispatch the request appropriately
    if (path.find("/cgi-bin/exec?cmd=") == 0) {
        // This is a command to be processed. So use a helper method
        // to streamline the code.
        // Remove the "/cgi-bin/exec?cmd=" prefix to help process the
        // command.
        auto cmd = url_decode(path.substr(18));
        sendCmdOutput(cmd, os, genChart);
    } else if (!path.empty()) {
        // In this case we assume the user is asking for a file.  Have
        // the helper http class do the processing.
        path = "." + path;  // make path with-respect-to pwd.
        // Use the http::file helper method to send the response back
        // to the client.
        os << http::file(path);
    }
}

/** Convenience method to decode HTML/URL encoded strings.

    This method must be used to decode query string parameters
    supplied along with GET request.  This method converts URL encoded
    entities in the from %nn (where 'n' is a hexadecimal digit) to
    corresponding ASCII characters.

    \param[in] str The string to be decoded.  If the string does not
    have any URL encoded characters then this original string is
    returned.  So it is always safe to call this method!

    \return The decoded string.
*/
std::string url_decode(std::string str) {
    // Decode entities in the from "%xx"
    size_t pos = 0;
    while ((pos = str.find_first_of("%+", pos)) != std::string::npos) {
        switch (str.at(pos)) {
            case '+': str.replace(pos, 1, " ");
            break;
            case '%': {
                std::string hex = str.substr(pos + 1, 2);
                char ascii = std::stoi(hex, nullptr, 16);
                str.replace(pos, 3, 1, ascii);
            }
        }
        pos++;
    }
    return str;
}

/**
 * The main function that serves as a test harness based on
 * command-line arguments.
 *
 * \param[in] argc The number of command-line arguments.  This test
 * harness can work with zero or one command-line argument.
 *
 * \param[in] argv The actual command-line arguments.  If this is an
 * number it is assumed to be a port number.  Otherwise it is assumed
 * to be an file name that contains inputs for testing.
 */
int main(int argc, char *argv[]) {
    // Check and use first command-line argument if any as port or file
    const std::string True = "true";  // Just a sentinel value
    std::string arg = (argc > 1 ? argv[1] : "0");
    const bool genChart = (argc > 2 ? (argv[2] == True) : false);
    // Check and use a given input data file for testing.
    if (arg.find_first_not_of("1234567890") == std::string::npos) {
        // All characters are digits. So we assume this is a port
        // number and run as a standard web-server
        runServer(std::stoi(arg));
    } else {
        // In this situation, this program processes inputs from a
        // given data file for testing.  That is, instead of a
        // web-browser we just read inputs from a data file.
        std::ifstream getReq(arg);
        if (!getReq.good()) {
            std::cerr << "Unable to open " << arg << ". Aborting.\n";
            return 2;
        }
        // Have the serveClient method process the inputs from a given
        // file for testing.
        serveClient(getReq, std::cout, genChart);
    }
    // All done.
    return 0;
}