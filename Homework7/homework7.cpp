/**
 * A program to use multiple threads to spell check words from a given 
 * text file.
 * Copyright (C) 2021 John Doll
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <thread>

using namespace std;

// Shortcut/synonym for a vector of strings
using StrVec = vector<string>;

// forward declaration of method
StrVec loadWords(const string file);

// global dictionary of valid words
const StrVec dict = loadWords("dictionary.txt");

/**
 * calculates how different one word is from another word, taken from 
 * wikipedia: https://en.wikipedia.org/wiki/Levenshtein_distance iterative
 * with full matrix pseudo code
 * @param s first word
 * @param t second word
 * @return the difference in characters
 */
int levenshteinDistance(string s, string t) {
    // get size of the strings
    int m = s.size();
    int n = t.size();
    // create matrix for computing character differences
    vector<vector<int>> d(m + 1, vector<int>(n + 1));
    // filling in the edge values with incrementing numbers representing the
    // letter location in the string
    for (int i = 1; i <= m; i++) {
        d[i][0] = i;
    }
    for (int i = 1; i <= n; i++) {
        d[0][i] = i;
    }
    // calculate if there is a difference between characters at each location
    for (int j = 1; j <= n; j++) {
        for (int i = 1; i <= m; i++) {
            int substitutionCost;
            // if letters are equal, then we don't add to our difference
            if (s[i - 1] == t[j - 1]) {
                substitutionCost = 0;
            } else {
                substitutionCost = 1;
            }
            // difference in characters at location is updated in matrix
            d[i][j] = min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + 
                           substitutionCost});
         }
    }
    // return overall word difference
    return d[m][n];
}

string checker(string word) {
    // create string to return and vector of strings to hold output before it is
    // cleaned
    string ret;
    StrVec outcomes;
    // send word to lowercase
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    // check levenshtein distance of the word and store results to vector if it
    // is lower than 4
    for (size_t i = 0; i < dict.size(); i++) {
        int lDist = levenshteinDistance(dict[i], word);
        if (lDist < 4) {
            outcomes.push_back(to_string(lDist) + ":" + dict[i]);
        }
    }
    // sort our outcomes in order from smallest levenshtein distance to largest
    sort(outcomes.begin(), outcomes.end());
    // add outputs to return string
    for (int i = 0; i < min(static_cast<int>(outcomes.size()), 5); i++) {
        // if distance is 0 then this is the only word that should be returned
        if (outcomes[i].find("0") != string::npos) {
            ret = outcomes[i];
            break;
        } else if (i + 1 != min(static_cast<int>(outcomes.size()), 5)) {
            // add word with comma after for next word
            ret += outcomes[i] + ", ";
        } else {
            // last word in ret string
            ret += outcomes[i];
        }
    }
    return ret;
}

/**
 * Initiates threads to run program
 * @param wordList list of words to be checked against the dictionary
 * @param results the differences between words and printable output
 * @param start where the array begins traversing
 * @param end where the array ends traversing
 */
void threadMain(StrVec wordList, StrVec& results, size_t start, 
                size_t end) {
    for (size_t i = start; i < end; i++) {
        results[i] = checker(wordList[i]);
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
        words.push_back(word);
    }
    return words;
}

/**
 * Main method driving the program
 * @param argc number of arguments the program received
 * @param argv the arguments in a char array
 * @return 0 if success, a different number otherwise
 */
int main(int argc, char* argv[]) {
    // loads in words to be checked for in the dictionary
    const StrVec words = loadWords(argv[1]);
    // creates vector for printable output later
    StrVec results(words.size());
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
    // print results
    for (size_t i = 0; i < words.size(); i++) {
        cout << words[i] << " -- " << results[i] << endl;
    }
    return 0;
}
