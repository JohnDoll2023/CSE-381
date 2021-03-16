/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 * Copyright (C) 2021 John Doll
 */

#include <math.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace std;

void prime(const unsigned long long int input, string& result);

int main(int argc, char *argv[]) {
    // vector of threads
    vector<thread> threads;
    // vector of strings for output
    vector<string> out(argc-1);
    // looping through vector of strings to give them default string which
    // prints if num is prime
    for (int i = 0; i < argc - 1; i++) {
        out[i] = argv[i + 1];
        out[i] += ": Is already prime.";
    }
    // sends over number and string from vector of strings to thread
    for (int i = 1; i < argc; i++) {
        threads.push_back(thread(prime, stoull(argv[i]), ref(out[i - 1])));
    }
    // rejoins threads
    for (auto& t : threads) {
        t.join();
    }
    // print out from vector of strings
    for (auto entry : out) {
        cout << entry << endl;
    }
    return 0;
}

void prime(const unsigned long long int input, string& ret) {
    // loops to check all possible factors
    bool num1, num2;
    for (unsigned long long int i = 2; i < sqrt(input) + 1; i++) {
        // enters if statement if it has factors meaning not prime
        if (input % i == 0) {
            // checking to see if either factor is prime
            for (unsigned long long int j = 2; j < sqrt(input/i) + 1; j++) {
                // checks first factor, sets to true if not prime
                if (i % j == 0 && i != 2 && !num1) { 
                    num1 = true;
                }
                // checks second factor, sets to true if not prime
                if ((input/i) % j == 0 && (input/i) != 2 && !num2) {
                    num2 = true;
                }
                // ends for loop if both factors have been proven not prime
                if ((num1 && num2) || ((j > sqrt(i) && num2))) {
                    break;
                }
            }
            // concatenating string for printing in main method
            ret = to_string(input) + " = " + to_string(i);
            ret += num1 ? " * " : " (prime) * ";
            ret += num2 ? to_string(input/i) : to_string(input/i) + " (prime)";
            // end loop as we have found factors
            return;
        }
    }
}
