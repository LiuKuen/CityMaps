#pragma once
#include <string>
#include "m1.h"
#include "m3.h"
#include "m4.h"
#include "graphics.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <X11/keysym.h>
#include "Global_var.h"
#include "GUI_Autocomplete.h"
#include "GUI_Autocomplete_Box.h"

/* the function to draw the map; should only be called in the main function */
void draw_map(void);





/******************************************************************************/
/********************* EasyGL Eventloop Callback Functions ********************/

/* the call back function used in event_loop to redraw the map */
void draw_screen(void);

/* the call back function used in event_loop to act on mouse button press */
void act_on_mouse_button_press (float x, float y, t_event_buttonPressed event);

/* the call back function used in event_loop to act on key press */
void act_on_key_press (char c, int keysym);

/******************************************************************************/





/******************************************************************************/
/*************************** Button Actions ***********************************/
/* action on traffic button */
void act_on_traffic_button(void (*drawscreen_ptr) (void));

/* action on find button */
void act_on_find_button(void (*drawscreen_ptr) (void));

/* action on map selector button */
void act_on_map_selector_button(void (*drawscreen_ptr) (void));

/* action on clear button */
void act_on_clear_button(void (*drawscreen_ptr) (void));

/* action on name button */
void act_on_name_button(void (*drawscreen_ptr) (void));

/* action on legend button */
void act_on_legend_button(void (*drawscreen_ptr) (void));

/* action on legend button */
void act_on_help_button(void (*drawscreen_ptr) (void));

/******************************************************************************/





/******************************************************************************/
/**************** Basic Drawing Functions Called In draw_screen ***************/

/* draw the stuff in found_stuff vector*/
void draw_found_stuff();

/* draw all streets based on current screen level */
void draw_streets(void);

/* draw all features */
void draw_features();

/* draw all one way arrows */
void draw_one_way_arrows();

/* draw all POIs */
void draw_POIs();

/* draw directives */
void draw_directive(std::vector<std::string>);

/* Draw help window*/
void draw_help();
/******************************************************************************/





/******************************************************************************/
/************************ Drawing Single Item Functions ***********************/
/* draw a single feature */
void draw_a_feature(unsigned featureID);

/* draw the street segment given a streetSegmentID */
void draw_a_street_segment(unsigned streetSegmentID);

/* draw an arrow given a location the orientation of the arrow */
void draw_an_arrow(Point2D origin,int degree);

/******************************************************************************/





/******************************************************************************/
/********************* Drawing Street Segment Functions ***********************/

/* this function sets the correct parameter(including color and thickness) for 
 * drawing street segments given the theOSMWay pointer*/
void set_street_segment_drawing_parameter(const OSMWay* theOSMWay, int type, int screen_level);

/******************************************************************************/





/******************************************************************************/
/************************** POI Functions *************************************/

//sets the colour of the point of interest
void set_POI_colour(int);

//plots the POI icon
void plot_POI_icon(Point2D, int);

void draw_outline(Point2D);

void draw_shop_icon(Point2D);

void draw_hospital_icon(Point2D);

void draw_college_icon(Point2D);

void draw_bank_icon(Point2D);

void draw_library_icon(Point2D);

void draw_warning_icon(Point2D);

/******************************************************************************/





/******************************************************************************/
/************************* Street Names Functions *****************************/

/* draw a name for a street segment on screen*/
void writeNameToScreen(unsigned street_segment_id,std::string street_name,int type);

/*Draw names for all the street on the current screen*/
void draw_all_names();

/*Return the other node in a street segment*/
unsigned otherNode(unsigned segmentID, unsigned intersectionID);

/*Perform a DFS of the street segments in a street*/
void visit_street_segments(const std::vector<unsigned>& streetSegmentVector, 
        const std::string& streetName, std::vector<bool>& visitedVector, 
        unsigned& intersectionID, std::vector<unsigned>& branchNodes, 
        Tree3D& namesTree, const unsigned& type);

/*Determine if current street segment was previously visited*/
bool isVisited(unsigned currSegment, const std::vector<unsigned>& streetSegmentVector,
        const std::vector<bool>& visitedVector);

/*Call the DFS function to visit all street segments and draw names on them*/
void drawNamesOnStreet(const std::vector<unsigned>&, const std::string&, 
        Tree3D& namesTree, const unsigned& type);

/*Determine if should draw name of current street segment on screen and calls */
/* writeNameToScreen accordingly                                              */
void drawName(const unsigned& streetSegment, const std::string& streetName, 
        Tree3D& namesTree, const unsigned& type);

/******************************************************************************/





/******************************************************************************/
/*********************** Drawing Pop Up Window functions **********************/
/* draw map selector window */
void draw_map_selector_window();

/*draws legend*/
void draw_legend();

void draw_path_window();

/******************************************************************************/

/* mouse button handlers */
void mouse_press_map_handler (float x, float y, t_event_buttonPressed event);
void mouse_press_find_window_handler (float x, float y, t_event_buttonPressed event);
void mouse_press_map_selector_window_handler (float x, float y, t_event_buttonPressed event);
void mouse_press_legend_window_handler(float x, float y, t_event_buttonPressed event);
void mouse_press_path_window_handler(float x, float y, t_event_buttonPressed event);
void mouse_press_help_window_handler(float x, float y, t_event_buttonPressed event);
void mouse_press_directive_window_handler(float x, float y, t_event_buttonPressed event);

/* action on ok button pressed in map selector */
void act_on_ok_button_pressed();

/******************************************************************************/





/******************************************************************************/
/************************* Traffic Drawing Functions **************************/

void draw_trafficIncidents();

void get_trafficIncidents();

/******************************************************************************/



/******************************************************************************/
/****************************Utility Functions*********************************/

/* draw the outline of a polygon  */
void drawpoly(t_point *points, int npoints);

/* return ture if x,y is inside of the box */
bool inside_box(float x,float y,t_bound_box box);

/* shorten the street name */
std::string shorten_street_name(std::string);

/* get angle between the vector (from point1 to point2) and the +x axis */
int get_angle(Point2D point1, Point2D point2);

/* return all streets id on the current screen */
std::map<unsigned,unsigned>  get_streets_ids_of_screen();

/* return all streets segment id on the current screen */
std::map<unsigned, unsigned> get_one_way_street_segemnts_id_from_screen();

/*set color using hexadecimal numbers*/
void set_color(int type);

/* return the current level of the map */
/* there are 12 levels for now */
int current_level(void);

/* highlight an intersection */
void highlight_intersection(Point2D intersection, int color);
void highlight_intersection(unsigned intersectionID, int color);

/* find all intersection within a range in a given kd tree */
std::vector<unsigned> find_intersections_within_range(LatLon centerLatLon, float range);

void highlightIntersectionOnclick(LatLon);

/* Delay function */
void delay(long milliseconds);

/* Reset all variables */
void clearALL();

/* Close all windows */
void clearAllWindow();
/* Test if a path is legal. Taken from m3 test files */
//bool path_is_legal(unsigned start_intersection, unsigned end_intersection, const std::vector<unsigned>& path);
/******************************************************************************/
