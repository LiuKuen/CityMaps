/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   search_box.h
 * Author: liujeffr
 *
 * Created on March 8, 2016, 10:32 AM
 */

#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <X11/keysym.h>
#include "graphics.h"
#include "GUI_Autocomplete_cell.h"
#include "GUI_Autocomplete.h"
#include <sstream>

using namespace std;

class AutocompleteBox{
public:
    AutocompleteBox();
    
    //first pair of floats control the offset of the text bar
    //last pair of floats control the offset of the autocomplete window
    void draw_autocomplete_box(float, float , float, float);
    void autocomplete_onclick_handler(float, float);
    void user_input(char, int);
    void draw_input_box(float, float);
    void draw_suggestions_box(float, float);
    void set_input(string);
    
    t_bound_box get_window_boundries();
    
    string get_input();
    
    void clear_input(){input.clear();}
    // Return true if the location is inside of the autocomplete boxes
    virtual bool clickedInside(float, float);
        
    // Set current highlighting
    void set_currentHighlighting(int keysym);
    bool isPressed(){return pressed;}
protected:
    void user_input_handler(char, int, string&);
    void autocomplete_window_handler(float, float);
    
    bool within_bounds(t_bound_box, float, float);
    
    string input;
    int currentHighlighting;
    bool pressed;
    bool autoCompletePressed;
    
    AutoCompleteHandler searchBoxAutoComplete;
    t_bound_box window;
    vector<AutocompleteCell> listOfQueries;
};