



#pragma once
#include "GUI_Autocomplete_Box.h"
#include <cmath>

using namespace std;

class PathBox{
public:
    PathBox();
    void draw_path_box();
    void pathbox_on_click_handler(float, float);
    void enable_path_box();
    
    void draw_searchbox();
    void user_input(char, int);
    bool clickInside(float, float);
    t_bound_box getToBoxBoundaries(){return to_box.get_window_boundries();}
    t_bound_box getFromBoxBoundaries(){return from_box.get_window_boundries();}
    AutocompleteBox* getFromBox(){return &from_box;}
    AutocompleteBox* getToBox(){return &to_box;}
private:
    void draw_cross();
    void draw_go();
    void find_path(string, string);
    //returns a vector that has split the string into different words for
    //every space between a word
    //ie) bay street & bloor street would return a vector of 5 components
    vector<string> parse(string);
    //combines the names that were created by parse to their respective POI name
    //or street name
    vector<string> combine_names(vector<string>);
    
    //determines if a name is a POI or a intersection it will return a vector of street
    //segment id's
    vector<unsigned> determine_type_of_name(string, string);

    bool within_bounds(t_bound_box, float, float);
    
    t_bound_box go_button_boundries;
    t_bound_box close_button_boundries;
    
    AutocompleteBox to_box;
    AutocompleteBox from_box;
    
    bool draw_path;
    bool draw_box;
    bool clear_on_click_from_box;
    bool clear_on_click_to_box;
    bool to_box_pressed;
    bool from_box_pressed;
};