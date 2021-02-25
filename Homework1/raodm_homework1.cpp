/**
 * A custom web-server that performs some simple Natural Language
 * Processing (NLP) on data from a given URL.
 *
 * Copyright (C) 2020 raodm@miamioh.edu
 */

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iomanip>


/** A convenience format string to generate results in HTML
    format. Note that this format string has place holders in the form
    %1%, %2% etc.  These are filled-in with actual values.  For
    example, you can generate actual values as shown below:

    \code

    int wc = 10, totChars = 20;
    float avgWordLen = 4.25;

    auto html = boost::str(boost::format(HTMLData) % wc %
                           totChars % avgWordLen);

    \endcode
*/
const std::string HTMLData = R"(<html>
  <body>
    <h2>Analysis results</h2>
    <p>Total number of words: %1%</p>
    <p>Number of words with 2-or-more vowels: %2%</p>
    <p>Average length of words with 2-or-more vowels: %3%</p>
  </body>
</html>
)";

/** The HTTP response header to be sent to the client.

    Note that this format string has a place holders in the form %1%
    for the length of the data being sent back to the client.  This
    value can be filled-in as shown below:

    \code

    std::strind data = "Some data to be sent";
    auto header = boost::str(boost::format(HTTPRespHeader) % data.length());

    \endcode
*/
const std::string HTTPRespHeader =
    "HTTP/1.1 200 OK\r\n"
    "Server: localhost\r\n"
    "Connection: Close\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %1%\r\n\r\n";

/**
 * It assumes the input stream is an HTTP GET request (hence it is
 * important to understand the input format before implementing this
 * method).  This method extracts the URL to be processed from the 1st
 * line of the input stream.
 *
 * IMPORTANT --- This method must read and discards any HTTP headers
 * in the inputs.  If you don't do this your program will not work
 * correctly with web-browsers.
 *
 * For example, if the 1st line of input is "GET
 * /http://localhost:8080/~raodm HTTP/1.1" then this method returns
 * "http://localhost:8080/~raodm"
 *
 * \note See earlier exercise(s) for hints for implementing this method.
 *
 * @return This method returns the path specified in the GET
 * request.
 */
std::string extractURL(std::istream& is) {
    std::string line, url;

    // Extract the GET request line from the input
    std::getline(is, line);
    // Read and skip HTTP headers. Without reading & skipping HTTP
    // headers, your program will not work correctly with
    // web-browsers.
    for (std::string hdr; std::getline(is, hdr) &&
             !hdr.empty() && hdr != "\r";) {}

    // Do basic substring operation to extract the URL that is
    // delimited by space from the first line of input.
    std::istringstream(line) >> url >> url;
    return url.substr(1);
}

/**
 * Helper method to break down a URL into hostname, port and path. For
 * example, given the url: "https://localhost:8080/~raodm/one.txt"
 * this method returns <"localhost", "8080", "/~raodm/one.txt">
 *
 * Similarly, given the url: "ftp://ftp.files.miamioh.edu/index.html"
 * this method returns <"ftp.files.miamioh.edu", "80", "/index.html">
 *
 * @param url A string containing a valid URL. The port number in URL
 * is always optional.  The default port number is assumed to be 80.
 *
 * @return This method returns a std::tuple with 3 strings. The 3
 * strings are in the order: hostname, port, and path.  Here we use
 * std::tuple because a method can return only 1 value.  The
 * std::tuple is a convenient class to encapsulate multiple return
 * values into a single return value.
 */
std::tuple<std::string, std::string, std::string>
breakDownURL(const std::string& url) {
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

/**
 * Process HTTP response data obtained from one web-server and send
 * results (as HTTP response) the web-browser.
 *
 * \param[in] is The input stream from where the HTTP response is to
 * be read and the number of words are to be counted. This method
 * should also compute the total number of characters in the words. In
 * addition, it should also compute average number of characters per
 * word.
 *
 * \param[out] os The output stream to where HTTP response along with
 * HTML data is to be sent back to the client.
 */
void process(std::istream& is, std::ostream& os) {
    // Get the first HTTP response line and ensure it has 200 OK in it
    // to indicate that the data stream is good.  However, in this
    // example, we assume that the data is fine.  First skip over HTTP
    // response headers.
    for (std::string hdr; std::getline(is, hdr) &&
             !hdr.empty() && hdr != "\r";) {}    

    // Track the 1st and 2nd longest word so that it can be reported back
    // to the client (as required in this project). Note that std::string
    // methods, find_first_of and find_last_of may come in handy.
    
    int totWc     = 0;  // The number of words processed
    int vowWc     = 0;  // The number of words with 2-or-more vowels
    int vowTotLen = 0;  // The total length of words with 2-or-more vowels

    // The list of vowels in English to ease finding them
    const std::string Vowels = "aeiouAEIOU";

    // The varible word is just a temporary to read 1-word at a time.
    for (std::string word; (is >> word); totWc++) {
        if (word.find_first_of(Vowels) != word.find_last_of(Vowels)) {
            // Found a word with 2-or-more vowels
            vowWc++;  // Track number of words with 2-or-more vowels
            vowTotLen += word.length();  // Track length of vowels
        }
    }
    
    // Generate results to be sent back to the client in HTML format.
    const float avgWordLen = vowTotLen * 1.0f / vowWc; 
    auto htmlData = boost::str(boost::format(HTMLData) %
                               totWc % vowWc % avgWordLen);
    // Generate the response.
    os << boost::format(HTTPRespHeader) % htmlData.length()
       << htmlData;
}

//-------------------------------------------------------------------------
//  STUDY THE CODE BELOW.
//  BUT DO  NOT  MODIFY  CODE  BELOW  THIS  LINE.
//-------------------------------------------------------------------------

/**
 * Helper method to process HTTP request from a web-browser and send
 * response back.  This method operates in the following manner:
 *
 *  1. It uses the extractURL method to get the URL of the data file
 *     to be processed from the GET request.
 *
 *  2. It then uses he breakDownURL method to obtain the host, port,
 *     and path of the data file to be processed. the GET request from
 *     the URL
 *
 *  3. It the 
 */
void serveClient(std::istream& is = std::cin, std::ostream& os = std::cout) {
    // Have helper method extract the URL for downloading data from
    // the input HTTP GET request.
    auto url = extractURL(is);
    std::cout << "URL to be processed is: " << url << std::endl;

    // Next extract download URL components. That is, given a URL
    // "http://www.users.miamioh.edu/raodm/ones.txt", 
    std::string hostname, port, path;
    std::tie(hostname, port, path) = breakDownURL(url);
    std::cout << "Processing file " << std::quoted(path) << " from "
              << std::quoted(hostname) << ":" << std::quoted(port) << " ...\n";

    // Start the download of the file (that the user wants to be
    // processed) at the specified URL.  We use a BOOST tcp::iostream.
    boost::asio::ip::tcp::iostream data(hostname, port);
    data << "GET "   << path     << " HTTP/1.1\r\n"
         << "Host: " << hostname << "\r\n"
         << "Connection: Close\r\n\r\n";
    // Have the helper method process the file's data and print/send
    // results (in HTTP/HTML format) to a given output stream.
    process(data, os);
}

/**
 * The main function that serves as a test harness based on
 * command-line arguments.
 *
 * \param[in] argc The number of command-line arguments.  This test
 * harness can work with zero or one command-line argument.
 *
 * \param[in] argv The actual command-line arguments.
 */
int main(int argc, char *argv[]) {
    // Check and use a given input data file for testing.
    if (argc == 2) {
        // In this situation, this program processes inputs from a
        // given data file for testing.  That is, instead of a
        // web-browser we just read inputs from a data file.
        std::ifstream getReq(argv[1]);
        if (!getReq.good()) {
            std::cerr << "Unable to open " << argv[1] << ". Aborting.\n";
            return 2;
        }
        // Have the serveClient method process inputs from a given
        // file for testing.
        serveClient(getReq);
        return 0;
    }


    // If a command-line argument has not been specified, we operate
    // as a web-server that can accept and process 1 request from an
    // actual web-browser.
    using namespace boost::asio;
    using namespace boost::asio::ip;

    // Create a server socket to accept connections over the Internet
    // from a web-browser.
    io_service service;
    tcp::acceptor server(service, tcp::endpoint(tcp::v4(), 34747));
    server.listen();
    // Print port number so that the user knows which port the
    // operating system has assigned to this program.
    std::cout << "Server is listening on port "
              << server.local_endpoint().port() << std::endl;

    // Accept connection from a web-browser
    tcp::iostream browser;
    server.accept(*browser.rdbuf());
    // Have the serveClient method do the processing by reading inputs
    // from the client and sending results back to the client.
    serveClient(browser, browser);
    return 0;  // Successful run.
}
