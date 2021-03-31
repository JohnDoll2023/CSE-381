/* 
 * A very lightweight (light as air) implementation of a simple CSV-based 
 * database system that uses SQL-like syntax for querying and updating the
 * CSV files.
 * Copyright (C) 2021 John Doll
 */

#include <string>
#include <fstream>
#include <tuple>
#include <algorithm>
#include "SQLAir.h"
#include "HTTPFile.h"

/**
 * A fixed HTTP response header that is used by the runServer method below.
 * Note that this a constant (and not a global variable)
 */
const std::string HTTPRespHeader = "HTTP/1.1 200 OK\r\n"
    "Server: localhost\r\n"
    "Connection: Close\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: ";

// API method to perform operations associated with a "select" statement
// to print columns that match an optional condition.
void SQLAir::selectQuery(CSV& csv, bool mustWait, StrVec colNames, 
        const int whereColIdx, const std::string& cond, 
        const std::string& value, std::ostream& os) {
    // set new vector equal to all column names
    StrVec allCol = csv.CSV::getColumnNames();
    // Convert any "*" to suitable column names. See CSV::getColumnNames()
    colNames = colNames[0] == "*" ? allCol : colNames;
    // First print the column names.
    os << colNames << std::endl;
    // Print each row that matches an optional condition.
    // create counter for printing # of rows
    int count = 0;
    for (const auto& row : csv) {
        // if where clause not used then print all rows, otherwise check values
        // of where to clause to determine if row should be printed
        if (whereColIdx == -1 || SQLAirBase::matches(
            row.at(csv.getColumnIndex(allCol[whereColIdx])), cond, value)) { 
            // increment counter because we know this row will be printed
            count++;
            std::string delim = "";
            for (const auto& colName : colNames) {
                    os << delim << row.at(csv.getColumnIndex(colName));
                    delim = "\t"; 
            }
            // if matches is true, then increment and print, 
            os << std::endl;
        }
    }
    // print out row count
    os << count << " row(s) selected." << std::endl;
}

void
SQLAir::updateQuery(CSV& csv,  bool mustWait, StrVec colNames, StrVec values, 
        const int whereColIdx, const std::string& cond, 
        const std::string& value, std::ostream& os)  {
    // Update each row that matches an optional condition.
    throw Exp("update is not yet implemented.");
}


void 
SQLAir::insertQuery(CSV& csv, bool mustWait, StrVec colNames, 
        StrVec values, std::ostream& os) {
    throw Exp("insert is not yet implemented.");
}

void 
SQLAir::deleteQuery(CSV& csv, bool mustWait, const int whereColIdx, 
        const std::string& cond, const std::string& value, std::ostream& os) {
    throw Exp("delete is not yet implemented.");
}

void
SQLAir::serveClient(std::istream& is, std::ostream& os) {
    // create query string
    std::string query;
    // get to url line to retrieve query
    is >> query >> query;
    // run through rest of client headers
    for (std::string hdr; std::getline(is, hdr) && !hdr.empty() &&
        hdr != "\r";) {}
    // check if query is in url line
    if (query.find("query") != std::string::npos) {
        // create output string stream 
        std::ostringstream oss;
        // begin try catch for process method
        try {
            // create query to be sent to processor
            query = Helper::url_decode(query).substr(1 + query.find("="));
            SQLAirBase::process(query, oss);
        } catch (const std::exception &exp) {
            // print error if something goes wrong
            oss << "Error: " << exp.what() << std::endl;
        }  // end try catch
        // Print http response headers with proper formatting
        std::string httpstr = oss.str();
        os << HTTPRespHeader << std::to_string(httpstr.size()) << "\r\n\r\n";
        os << httpstr;
    } else {
        // if query is not in the url, then read from the file that is supplied
        http::file output(query.substr(1));
        os << output;  
    }
}

// The method to have this class run as a web-server. 
void 
SQLAir::runServer(boost::asio::ip::tcp::acceptor& server, const int maxThr) {
    while (true) {
        // create client stream 
        auto client = std::make_shared<boost::asio::ip::tcp::iostream>();
        server.accept(*client->rdbuf());
        // create thread with lambda as function
        std::thread thr([this, client](){
            serveClient(*client, *client); });
        // run threads asynchronously
        thr.detach();
    }
}

// Convenience helper method to return the CSV object for a given
// file or URL.
CSV& SQLAir::loadAndGet(std::string fileOrURL) {
    // If the parameter is empty string, we use our most recent as default.
    if (fileOrURL == "") {
        std::lock_guard<std::mutex> guard(recentCSVMutex);
        fileOrURL = recentCSV;  // use most recent CSV as the default.
    }
    // If the requested data is already in-memory then just return it
    if (inMemoryCSV.find(fileOrURL) == inMemoryCSV.end()) {
        // The data was not in memory. So we need to load it
        CSV csv;   // Load data into this csv
        if (fileOrURL.find("http://") == 0) {
            // This is an URL. We have to get the stream from a web-server
            // Implement this feature.
            std::string host, port, path;
            std::tie(host, port, path) = Helper::breakDownURL(fileOrURL);
            // set web stream equal to helper method
            boost::asio::ip::tcp::iostream web = HTTPProcess(host, port, path);
            // load csv data from web connection
            csv.load(web);
        } else {
            // We assume it is a local file on the server. Load that file.
            std::ifstream data(fileOrURL);
            csv.load(data);  // This method may throw exceptions.
        }
        // Move (instead of copy) the CSV data into our in-memory CSVs
        inMemoryCSV[fileOrURL].move(csv);
    }
    // When control drops here, we must have a CSV to return
    std::lock_guard<std::mutex> guard(recentCSVMutex);
    recentCSV = fileOrURL;   // Update most recently used CSV as default.
    return inMemoryCSV.at(fileOrURL);
}

boost::asio::ip::tcp::iostream
SQLAir::HTTPProcess(std::string host, std::string port, std::string path) {
    // line for reading from client
    std::string line;
    // connect to client
    boost::asio::ip::tcp::iostream client(host, port);
    // Get headers from client
    client << "GET " << path << " HTTP/1.1\r\n" << "Host: " << host
               << "\r\n" << "Connection: Close\r\n\r\n";
    // check to see if client connection is valid
    if (client.good()) {
        // Get first client header
        std::getline(client, line);
        // check if connection was made successfully
        if (static_cast<int>(line.find("200 OK")) != -1) {
            // run through rest of client headers
            for (std::string hdr; std::getline(client, hdr) && !hdr.empty() &&
                    hdr != "\r";) {}
        } else {
            // trim client header for error reporting
            line = Helper::trim(line);
            // throw 404 error
            throw Exp("Error (" + line + ") getting " + path + " from " + host +
                    " at port " + port);
        }
    } else {
        // throw connection error
        throw Exp("Unable to connect to " + host + " at port " + port);
    }
    // return client connection
    return client;
}

// Save the currently loaded CSV file to a local file.
void 
SQLAir::saveQuery(std::ostream& os) {
    if (recentCSV.empty() || recentCSV.find("http://") == 0) {
        throw Exp("Saving CSV to an URL using POST is not implemented");
    }
    // Create a local file and have the CSV write itself.
    std::ofstream csvData(recentCSV);
    inMemoryCSV.at(recentCSV).save(csvData);
    os << recentCSV << " saved.\n";
}
