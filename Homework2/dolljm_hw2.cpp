/*
 * Purpose: Print login-IDs of users given a set of group IDs
 * File: dolljm_hw2.cpp
 * Copyright (C) 2021 John Doll
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <vector>

using namespace std;

using IntStrMap = unordered_map<int, string>;

/**
 * Takes in a string to be split and returned as a vector with the split values
 * @param line the string to be split
 * @param splitter the value to split the string by
 * @return the separated items of the line into a vector
 */
vector<string> splitLine(string line, char splitter) {
    // replace colons with commas to get separate terms in line
    replace(begin(line), end(line), splitter, ' ');
    
    // create input stringstream for the line, a vector to put each term in, and
    // a string to help iterate through the line
    istringstream is(line);
    vector<string> ret;
    string word;
    
    // put each term into the vector
    while (is >> word) {
        ret.push_back(word);
    }
    
    return ret;
}

/**
 * Reads a file containing userIDs and usernames and returns an unordered_map
 * with those values
 * @return unordered_map of userIDs matched with their usernames
 */
IntStrMap readPasswd() {
    // access file
    ifstream inFile("passwd.txt");
    
    // create map of IDs and string line to help iterate through the file
    string line;
    IntStrMap id;
    
    // split every line of the file and assign UID and loginID to map
    while (getline(inFile, line)) {
        vector<string> terms = splitLine(line, ':');
        if (terms.size() > 2) {
            id[stoi(terms[2])] = terms[0];
        }
    }
    return id;
}
/**
 * Cross references the groupIDs, userIDs, and group names, and user names to
 * find out which users belongs with which groups
 * @return a tuple of an IntStrMap and an unordered_map that has int groupID and
 * IntStrMap
 */
tuple<IntStrMap, unordered_map<int, vector<string>>> readGroups() {
    unordered_map<int, string> id = readPasswd();
    // access file
    ifstream inFile("groups.txt");
    
    // create string line to help iterate through input, map for groupIDs and 
    // map for userIDs in that group
    string line;
    IntStrMap group;
    unordered_map<int, vector<string>> users;
    
    // Run through each line and assign groupID and name to one map, and groupID
    // and associated userIDs to another map
    while (getline(inFile, line)) {
        // gives values for group name, number, and numbers of users accounts
        vector<string> terms = splitLine(line, ':');
        
        // assign group name and number to map
        group[stoi(terms[2])] = terms[0];
        
        // gets userIDs in a single group
        vector<string> UID = splitLine(terms[3], ',');
        
        // removes 0 from userID lists that occur when no other IDs given
        if (UID.size() == 1 && stoi(UID.at(0)) == 0 && terms[0] == "bin") {
           UID.erase(remove(UID.begin(), UID.end(), "0"), UID.end());
        }
        
        // vector for userIDs and usernames
        vector<string> usersInGroup;
        
        // adds these specific userIDs to vector to be put with map that has 
        // groupID and vector of userIDs and their usernames
        for (size_t i = 0; i < UID.size(); i++) {
            if (id.find(stoi(UID[i])) != end(id)) {
                usersInGroup.push_back(id[stoi(UID[i])] + "(" + UID[i] + ")");
            }
        }
        users[stoi(terms[2])] = usersInGroup;
    }
    
    // return tuple so that each map is returned
    return {group, users};
}
/**
 * Main method: reads in tuple from another method call, splits each item in 
 * tuple into two separate maps. Iterates through group map from command line 
 * input to see if there are matches, and if there are, print out users
 * associated with this groupID
 * @param argc number of command line inputs
 * @param argv command line inputs in array
 * @return 0 if executed properly
 */
int main(int argc, char *argv[]) {    
    // gets two maps from the two input files
    tuple<IntStrMap, unordered_map<int, vector<string>>> maps = readGroups();
    
    // first map contains groupID, second has groupID and userID paired with
    // usernames
    IntStrMap group = get<0>(maps);
    unordered_map<int, vector<string>> users = get<1>(maps);
    
    // take groupID from user, find if it matches any known groups, and then
    // print info and users associated with that group
    for (int i = 1; i < argc; i++) {
        if (group.find(stoi(argv[i])) != end(group)) {
            cout << stoi(argv[i]) << " = " << group[stoi(argv[i])] << ":";
            for (const auto& entry : users[stoi(argv[i])]) {
                cout << " " << entry;
            }
            cout << endl;
        } else {
            cout << stoi(argv[i]) << " = Group not found." << endl;
        }
    }
    return 0;
}
