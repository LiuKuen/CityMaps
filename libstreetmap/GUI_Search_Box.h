#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include "graphics.h"
#include <X11/keysym.h>
#include "GUI_Autocomplete_Box.h"
#include "math.h"
#include "B_Point2D.h"

using namespace std;

class SearchBox: public AutocompleteBox{
public:
    SearchBox();
    void draw_search_box();
    void searchbox_onclick_handler(float, float);
    void clear_intersection_icons();
    void enable_search_box();
    void draw_pathbox();
private:
    void draw_magnifying_glass();
    void draw_direction_icon();
    void parse_user_input();
    
    void find_intersection(string, string, string);
    void highlight_intersection(Point2D);
    
    vector<LatLon> intersectionLatLon;
    
    bool draw_box;
    bool draw_found_intersection_icon;
    bool clear_on_click;
    
    t_bound_box magnifying_glass_boundires;
    t_bound_box direction_icon_boundries;
};