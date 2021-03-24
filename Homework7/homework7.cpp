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

// forward declaration of method
StrVec loadWords(const string file);

// global dictionary of valid words
const StrVec dict = loadWords("dictionary.txt");

/**
 * calculates how different one word is from another word
 * @param s first word
 * @param t second word
 * @return the difference in characters
 */
int levenshteinDistance(string s, string t) {
    // get size of the strings
    int n = t.size();
    int m = s.size();
    // create matrix for computing character differences
    std::vector<std::vector<int>> d(m + 1, std::vector<int>(n + 1));
    // 
    for (int i = 1; i <= m; i++) {
        d[i][0] = i;
    }
    //
    for (int i = 1; i <= n; i++) {
        d[0][i] = i;
    }
    // calculate if there is a difference between characters at each location
    for (int j = 1; j <= n; j++) {
        for (int i = 1; i <= m; i++) {
            int substitutionCost;
            // if letters are equal, then we don't add to our difference
            if (s[i] == t[j]) {
                substitutionCost = 0;
            } else {
                substitutionCost = 1;
            }
            // difference in characters at location is updated in matrix
            d[i][j] = min(d[i - 1][j] + 1, min(d[i][j - 1] + 1, d[i - 1][j - 1] 
                    + substitutionCost));
         }
    }
    // return overall word difference
    return d[m][n];
}

void threadMain(StrVec wordList, vector<vector<string>>& results, size_t start, 
                size_t end) {
    // StrVec words;
//    for (size_t i = start; i < end; i++) {
//        words[i - start] = wordList[i];
//    }
    for (size_t i = start; i < end; i++) {
        for (size_t j = 0; j < dict.size(); j++) {
            if (binary_search(dict.begin(), dict.end(), wordList[i])) {
                results[i][j] = "  0:" + wordList[i];
                break;
            }
            int lDist = levenshteinDistance(dict[j], wordList[i]);
            if (lDist < 4) {
                results[i][j] = " " + to_string(lDist) + ":" + dict[j];
            }
        }
    }
}

/**
 * Prints out results of program
 * @param results the vector of vector of strings to be printed
 * @param wordList contains original words to match to results vector
 */
void print(vector<vector<string>> results, StrVec wordList) {
    for (size_t i = 0; i < wordList.size(); i++) {
        cout << wordList[i] << " --";
        for (int j = 0; j < min(static_cast<int>(results[i].size()), 5); j++) {
            cout << results[i][j];
            if (results[i][j].find("0") != string::npos) {
                break;
            }
            if (j + 1 != min(static_cast<int>(results[i].size()), 5)) {
                cout << ",";
            }
        }
        cout << endl;
    }
}

/**
 * Loads words from specified files in to string vector
 * @param file the file where will words will be read from
 * @return vector of all the words in the file
 */
StrVec loadWords(const string file) {
    // create vector for incoming words and word string for input stream
    StrVec words;
    string word;
    // input stream to read file
    ifstream inFile(file);
    // loop to iterate through file and read every word into the vector
    while (getline(inFile, word)) {
        // need to move this line
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        words.push_back(word);
    }
    return words;
}

int main(int argc, char* argv[]) {
    // loads in words to be checked for in the dictionary
    const StrVec words = loadWords(argv[1]);
    // creates vector for printable output later
    vector<vector<string>> results;
    // vector of threads
    vector<thread> threads;
    // determines number of threads
    const int numThreads = argc == 3 ? stoi(argv[2]) : 1;
    // compute range of values each thread should iterate over
    const int count = (words.size() + numThreads - 1) / numThreads;
    // loop to create threads and begin processing data
    for (int thr = 0; thr < numThreads; thr++) {
        // beginning index of our words to be checked
        size_t start = thr * count;
        // ending index of our words to be checked
        size_t end = min(words.size(), (start + count));
        threads.push_back(thread(threadMain, words, ref(results), start, end));
    }
    // join threads
    for (auto& t : threads) {
        t.join();
    }
    // sort results
    for (size_t i = 0; i < results.size(); i++) {
        sort(results[i].begin(), results[i].end());
    }
    // print results
    print(results, words);
    return 0;
}
