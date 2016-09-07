/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   autocomplete_window.h
 * Author: liujeffr
 *
 * Created on March 8, 2016, 8:27 PM
 */

#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include "graphics.h"
#include <X11/keysym.h>

using namespace std;


class AutocompleteCell{
public:
    AutocompleteCell(t_bound_box, string);
    void setInfo(t_bound_box, string);
    t_bound_box getWindow();
    string getQuery();
private:
    t_bound_box autocompleteWindow;
    string autocompleteQuery;
};

