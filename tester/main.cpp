/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: jdoll
 * Copyright (C) 2021 dolljm
 * Created on February 3, 2021, 5:44 PM
 */

#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
using namespace std;
/*
 * 
 */
int main(int argc, char** argv) {
    unordered_map<string, int> test2;
    test2["abc"] = 1;
    test2["abc"] += 2;
    cout << test2["abc"] << endl;
    
    vector<int> test = {1, 2};
    test[0] += 5;
    cout << test[0] << endl;
    return 0;
}

