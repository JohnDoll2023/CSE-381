/* 
 * File:   raodm_hw3.cpp
 * Author: raodm
 * 
 * Copyright (C) 2021 raodm@miamioh.edu
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <sstream>
#include "raodm_hw3.h"

// The primary interface method that is invoked once to
// perform the necessary functionality.
void 
raodm_hw3::listFiles(const std::string& dirFilePath, const std::string& login,
                     const std::string& permType) {
    // First load the user information from the passwd.txt file
    // into the gidName unordered map
    loadUserIDs();
    // Next, load the group information into the gidUsers and gidName
    // unordered map.
    loadGroupInfo();
    // Now process the dirFilePath and list files that meet permType
    listFiles(dirFilePath, login, getUid(login), permType);
}

// Simple method to return the userID for a given loginID
int 
raodm_hw3::getUid(const std::string& login) const {
    // Iterate over each entry in the gidName map to determine
    // the user ID
    for (const auto& entry : uidName) {
        if (entry.second == login) {
            return entry.first;
        }
    }
    // If entry is not found then we simply return zero to list all files.
    return 0;
}

/**
 * Process data from a given text file and print files for which
 * the given user has read, write, or execute permissions.
 * 
 * @param filePath The path to the file from where the data for the files
 * is to be read.
 * 
 * @param The user's login ID for which the files are to be listed. If this
 * is an empty string, then all the files are listed.
 */
void 
raodm_hw3::listFiles(const std::string& filePath, const std::string& login,
        const int uid, const std::string chkType) {
    // Convert uid to string to ease look-up below.
    const std::string uidStr  = std::to_string(uid) + ',';
    // An unordered map to track a file's path because it is accessible by
    // the user (either via user permissions or via group permissions)
    std::unordered_map<std::string, bool> accPaths;
    // The data file to be processed.
    std::ifstream fileList(filePath);
    // Temporary variables used in the loop below.
    int filePerms, fileUid, fileGid;
    std::string path;
    while (fileList >> std::oct >> filePerms >> std::dec >> fileUid
                    >> fileGid >> path) {
        // Convert permissions to a bit-string for convenience
        const std::string perms = std::bitset<9>(filePerms).to_string();
        // Check if we have necessary permissions and print the file.
        if (havePerms(accPaths, fileUid, fileGid, path, perms, uid, chkType)) {
            // Print the information about the file.
            printFile(perms, fileUid, fileGid, path);
        }
    }
}

// Prints the file information.
void 
raodm_hw3::printFile(std::string perms, 
            const int fileUid, const int fileGid,
            const std::string& filePath) const {
    // Change 1s and 0s to rwx- for pretty printing.
    std::transform(perms.begin(), perms.end(), "rwxrwxrwx", perms.begin(),
                   [](auto flg, auto c) { return flg == '1' ? c : '-'; });
    std::cout << perms << " " 
            << std::setw(8) << std::left <<  uidName.at(fileUid)
            << " " << std::setw(8) << std::left << gidName.at(fileGid) 
            << " " << filePath << '\n';
}

// Checks if a given user has access to the file based on user, group,
// or others. It also checks the parent path (to see if user has access to it)
// based on the permission type.
bool 
raodm_hw3::havePerms(std::unordered_map<std::string, bool>& accPaths,
        const int fileUid, const int fileGid, const std::string& path, 
        const std::string& perms, const int uid, 
        const std::string& chkType) const {
    // First check permissions based on uid value and user permissions.
    bool havePerms = (fileUid == uid) && (perms.substr(0, 3) != "000");

    // Check if group has permissions for the file.
    // Convert uid to string to ease look-up below.
    const std::string uidStr  = std::to_string(uid) + ',';
    if (!havePerms && (chkType != "u") &&
            (gidUsers.at(fileGid).find(uidStr) != std::string::npos) &&
            (perms.substr(3, 3) != "000")) {
        havePerms = true;  // Have permissions via group settings
    }

    // Check if others has permissions for this file.
    if (!havePerms && (chkType != "u") && (perms.substr(6, 3) != "000")) {
        havePerms = true;  // Have permissions via group settings
    }

    // Finally check if parent path has permissions based on
    // the type of checks we are doing.
    if (havePerms && chkType == "r") {
        // Check if the parent path is in accessible paths
        const std::string parent = path.substr(0, path.rfind('/'));
        havePerms = (parent.empty()) || 
                (accPaths.find(parent) != accPaths.end());
    }

    // Override have permissions if login ID is zero
    if ((uid == 0) || havePerms) {
        havePerms = true;
        accPaths[path] = true;  // track accessible path
    }
    // The parent path is not accessible. So no access to child path
    return havePerms;
}

//---------------------------------------------------------------
//  DO  NOT   MODIFY   CODE  BELOW  THIS LINE
//---------------------------------------------------------------

void
raodm_hw3::loadUserIDs(const std::string& passFile) {
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
void
raodm_hw3::loadGroupInfo(const std::string& groupFile) {
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
    raodm_hw3 ls;
    ls.listFiles(dirFile, login, permType);
    return 0;
}
