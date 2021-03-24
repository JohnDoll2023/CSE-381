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

int LevenshteinDistance(vector<char> s, vector<char> t) {
    int n = t.size();
    int m = s.size();
    std::vector<std::vector<int>> d(m + 1, std::vector<int>(n + 1));
    for (int i = 0; i < n; i++) {
        d[i][0] = i;
    }
    for (int i = 1; i < n; i++) {
        d[0][i] = i;
    }
    for (int j = 1; j < n; j++) {
        // use formula to fill in the rest of the row
        for (int i = 1; j < m; i++) {
            int substitutionCost;
            if (s[i] == t[j]) {
                substitutionCost = 0;
            } else {
                substitutionCost = 1;
            }
            d[i][j] = min(d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] +
                        substitutionCost);
        }
    }
    return d[m][n];
}

void threadMain(StrVec dict, StrVec wordList, vector<vector<string>>& results, 
        size_t start, size_t end) {
    StrVec wordListPartition(end - start);
    for (size_t i = start; i < end; i++) {
        wordListPartition[i - start] = wordList[i];
    }
    for (size_t i = 0; i < wordListPartition.size(); i++) {
        for (size_t j = 0; j < dict.size(); j++) {
            if (binary_search(dict.begin(), dict.end(), 
                wordListPartition[i]) == 0) {
                results[i][j] = "  0:" + wordListPartition[i];
                break;
            }
            int lDist = LevenshteinDistance(dict[j], wordListPartition[i]);
            if (lDist < 4) {
                results[i][j] = " " + to_string(lDist) + ":" + dict[j];
            }
        }
    }
}

void print(vector<vector<string>> results, StrVec wordList) {
    for (size_t i = 0; i < wordList.size(); i++) {
        cout << wordList[i] << " --";
        for (int j = 0; j < min((int)results[i].size(), 5); j++) {
            cout << results[i][j];
            if (results[i][j].find("0") != string::npos) {
                break;
            }
            if (j + 1 != min((int)results[i].size(), 5)) {
                cout << ",";
            }
        }
        cout << endl;
    }
}

StrVec loadFile(string file) {
    StrVec words;
    string word;
    ifstream inFile(file);
    while (getline(inFile, word)) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        words.push_back(word);
    }
    return words;
}

int main(int argc, char* argv[]) {
    const StrVec dict = loadFile("dictionary.txt");
    const StrVec wordList = loadFile(argv[1]);
    vector<vector<string>> results;
    vector<thread> threads;
    const int numThreads = argc == 3 ? stoi(argv[2]) : 1;
    const int count = (wordList.size() / numThreads) + 1;
    for (int thr = 0; thr < numThreads; thr++) {
        size_t start = thr * count;
        size_t end = min(wordList.size(), (start + count));
        threads.push_back(thread(threadMain, dict, wordList, ref(results), 
                start, end));
    }
    for (auto& t : threads) {
        t.join();
    }
    for (size_t i = 0; i < results.size(); i++) {
        sort(results[i].begin(), results[i].end());
    }
    print(results, wordList);
    return 0;
}
