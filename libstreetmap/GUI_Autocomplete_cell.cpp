/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "GUI_Autocomplete_cell.h"

AutocompleteCell::AutocompleteCell(t_bound_box boxBounds, string autoCompleteQueries){
    autocompleteWindow = boxBounds;
    autocompleteQuery = autoCompleteQueries;
}


void AutocompleteCell::setInfo(t_bound_box boxBounds, string autoCompleteQueries){
    autocompleteWindow = boxBounds;
    autocompleteQuery = autoCompleteQueries;
}

t_bound_box AutocompleteCell::getWindow(){
    return autocompleteWindow;
}

string AutocompleteCell::getQuery(){
    return autocompleteQuery;
}