/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   autocomplete.h
 * Author: liujeffr
 *
 * Created on March 5, 2016, 11:19 PM
 */

#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include "B_Trie.h"
#include <X11/keysym.h>

using namespace std;

class AutoCompleteHandler{
public:
    // Update the autocompleteQueries given a new string
    void updateQueries(Trie&, int, string);
    
    // Complete the input string
    void autoCompleteString(int, string&,int);
    
    // Get the size of the current autocompleteQueries
    int numberOfQueries(){ return autocompleteQueries.size(); };
    
    // Get the query in the autocompleteQueries given an index
    string getQueriesAt(int index) { return autocompleteQueries[index]; };

    
private:
    vector<string> autocompleteQueries;
};