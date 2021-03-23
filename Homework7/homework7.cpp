/**
 * A program to use multiple threads to spell check words from a given 
 * text file.
 * Copyright (C) 2021 John Doll
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <thread>
#include <cmath>
#include <unordered_map>

using namespace std;

// Shortcut/synonym for a vector of strings
using StrVec = vector<string>;

vector<string> loadFile(string file) {
    StrVec words;
    string word;
    ifstream inFile(file);
    while (getline(inFile, word)) {
        words.push_back(word);
    }
    return words;
}

int main(int argc, char* argv[]) {
    StrVec dict = loadFile("dictionary.txt");
    StrVec wordList = loadFile(argv[1]);
    vector<thread> threads;
    int numThreads = argc == 3 ? stoi(argv[2]) : 1;
    const int count = (wordList.size() / numThreads) + 1;
    for (int thr = 0; thr < numThreads; thr++) {
        size_t start = thr * count;
        size_t end = min(wordList.size(), (start + count));
        threads.push_back(thread());
    }
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
