/* 
 * File:   dolljm_hw3.cpp
 * Author: John Doll
 * Copyright (C) 2021 John Doll
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <vector>
#include "dolljm_hw3.h"

// The primary interface method that is invoked once to
// perform the necessary functionality.
void 
dolljm_hw3::listFiles(const std::string& dirFilePath, const std::string& login,
                     const std::string& permType) {
    // First review the starter code (which is essentially solution to 
    // the previous homework).
    
    // Next, suitably implement this method. Of course, you may add additional
    // methods and instance variables to the class using necessary helper
    
    // Call other two methods given
    loadUserIDs();
    loadGroupInfo();
    
    // create input stream to read the list of files and their permissions
    std::ifstream inFile(dirFilePath);
    
    // variables to assist in reading from file
    std::string line, path;
    int fileUid, fileGid, filePerms;
    
    // loops through every line in the file, separates each term into own
    // variable, sends variables to helper method to check if user meets file
    // permissions
    while (getline(inFile, line)) {
        std::istringstream is(line);
        is >> std::oct >> filePerms >> std::dec >> fileUid >> fileGid >> path;
        const std::string perms = std::bitset<9>(filePerms).to_string();
        if (checkFile(login, perms, fileGid, permType, fileUid, path, 
                dirFilePath)) {
            print(perms, fileUid, fileGid, path);
        }
    }
}

// Checks permissions of files
bool
dolljm_hw3::checkFile(const std::string login, const std::string perms, 
        const int fileGid, const std::string pType, const int fileUid,
        std::string path, const std::string fPath) {
    // print all files if root is chosen userID
    if (login == "root") {
        return true;
    }
    
    // check if either of the user permissions are set to on and check to make
    // sure that the user specified is allowed access to file
    // 
    // else if checks if the other (last 3 digits) permissions are on and that
    // the specified permission is not set to user
    //
    // last else if checks that "r" permission is entered and then for the user
    // and group permissions before sending to helper method to check if parent
    // directories are also accessible by same user
    if (perms.substr(0, 3).find("1") != std::string::npos &&
                uidName.at(fileUid) == login && pType != "r") {
        return true;
    } else if ((perms.substr(6, 3).find("1") != std::string::npos && 
            pType != "u" && pType != "r")) { 
        return true;
    } else if (pType == "r" && ((perms.substr(0, 3).find("1") != 
            std::string::npos && uidName.at(fileUid) == login) || (
            perms.substr(6, 3).find("1") != std::string::npos && 
            pType == "r"))) {
        return checkRFile(login, path, fPath);
    }
    return false;
}

// Recursively checks parent paths to find if user is able to access at each 
// level of path
bool
dolljm_hw3::checkRFile(const std::string login, std::string path, 
        std::string fPath) {
    // calls helper method to get parent path
    std::string parentPath = getParentPath(path);
    
    // checks if parent method is down to base, in which case all previous paths
    // are accessible by user so we return true, this is base case
    if (parentPath.size() < 2) {
        return true;
    }
    
    // creates variables for looping through file to find parent path and
    // comparing file permissions
    std::string line, pPath = "";
    std::ifstream inFile(fPath);
    int fileUid, fileGid, filePerms;
    
    // loops through file to find parent path and checks if it is accessible by
    // the user and then recursively calls the method to check its parent path
    while (getline(inFile, line)) {
        std::istringstream is(line);
        is >> std::oct >> filePerms >> std::dec >> fileUid >> fileGid >> pPath;
        const std::string perms = std::bitset<9>(filePerms).to_string();
        if (parentPath == pPath) {
            if (perms.substr(0, 3).find("1") != std::string::npos &&
                uidName.at(fileUid) == login) {
                return checkRFile(login, parentPath, fPath);
            } else if ((perms.substr(6, 3).find("1") != std::string::npos)) { 
                return checkRFile(login, parentPath, fPath);
            }
        }
    }
    return false;
}

// Gets the parent path of the given path
std::string
dolljm_hw3::getParentPath(std::string path) {
    // creates variables to splice up path
    std::string parentPath, temp = "";
    std::vector<std::string> filePath;
    std::replace(path.begin(), path.end(), '/', ' ');
    std::istringstream is(path);
    
    // splices path name
    while (is >> temp) { 
        filePath.push_back(temp); 
    }
    
    // returns path with all but last argument so that we have parent path
    for (size_t i = 0; i < filePath.size() - 1; i++) {
        parentPath += ("/" + filePath[i]);
    }
    return parentPath;
}

// Used to print out the permissions of the files
void
dolljm_hw3::print(std::string perms, const int fileUid, const int fileGid,
           const std::string& filePath) {
    std::string retPerms;
    // convert perms from binary to characters
    for (size_t i = 0; i < perms.length(); i++) {
        if (perms.substr(i, 1) == "1") {
            if (((i + 1) % 3) == 0) {
                retPerms += "x";
            } else if (((i + 2) % 3) == 0) {
                retPerms += "w";
            } else {
                retPerms += "r";
            }
        } else {
            retPerms += "-";
        }
    }
    std::cout << retPerms
            << " " << std::setw(8) << std::left << uidName.at(fileUid)
            << " " << std::setw(8) << std::left << gidName.at(fileGid)
            << " " << filePath << '\n';
}

//---------------------------------------------------------------
//  DO  NOT   MODIFY   CODE  BELOW  THIS LINE
//---------------------------------------------------------------

// Load userIDs and login IDs from passwd.txt file into the uidName
// unordered map. This method is essentially a copy-paste from an
// earlier homework's solution.
void
dolljm_hw3::loadUserIDs(const std::string& passFile) {
    std::ifstream passwd(passFile);
    // Process line-by-line from the file.
    std::string line;
    while (std::getline(passwd, line)) {
        std::string login, dummy;
        int uid;
        // Replace ':' with space because space is one of the default 
        // delimiter used by istringstream to parse strings.
        std::replace(line.begin(), line.end(), ':', ' ');
        // Now extract the values we need.  Simple.
        std::istringstream(line) >> login >> dummy >> uid;
        // Add entry to the instance variable unordered map
        uidName[uid] = login;
    }
}

// Load user info into gidName and gidUsers map (instance variables)
// This method is essentially a copy-paste from an earlier homework's solution.
void
dolljm_hw3::loadGroupInfo(const std::string& groupFile) {
    // File stream to read line-by-line from groups file
    std::ifstream group(groupFile);
    // Process line-by-line from the file.
    std::string line;
    while (std::getline(group, line)) {
        // Variables to be read below.
        std::string grpName, memberList, dummy;
        int gid = 0;
        // Extract the group name, gid, and member list after replacing
        // ':' with space because space is one of the default delimiters used
        // by istringstream to parse strings.
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream(line) >> grpName >> dummy >> gid >> memberList;
        // Add a comma to ease finding userID later on.
        memberList += ',';
        // Store the group membership information in unordered map to
        // return back.
        gidName[gid]  = grpName;     // Group name
        gidUsers[gid] = memberList;  // Group member user IDs
    }
}

/**
 * The main method extracts the command-line arguments and invokes
 * the listFiles primary-method to perform the necessary processing.
 * 
 * @param argc The number of command-line arguments. 
 * 
 * @param argv The actual command-line arguments. The list of 
 * command-line arguments are assumed to be in the following order:
 *     1. First one is the file containing directory listing to process
 *     2. The login ID of the user. This can be empty string.
 *     3. The permission to apply. This can be "u", "g", or "r".
 * @return This method always returns zero.
 */
int main(int argc, char *argv[]) {
    // Extract the command-line arguments supplied
    const std::string dirFile  = (argc > 1 ? argv[1] : "files1.txt");
    const std::string login    = (argc > 2 ? argv[2] : "raodm");
    const std::string permType = (argc > 3 ? argv[3] : "u");
    // Have the class method perform the necessary operations
    dolljm_hw3 ls;
    ls.listFiles(dirFile, login, permType);
    return 0;
}
