/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Global.h
 * Author: pahlavil
 *
 * Created on February 18, 2016, 3:51 PM
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include "StreetsDatabaseAPI.h"
#include "kdlibtree/kdtree.hpp"
#include "B_Point2D.h"
#include "B_Point3D.h"
#include "graphics.h"
#include "B_TrafficIncident.h"
#include "B_Graph.h"
#include "B_Trie.h"
#include "GUI_Autocomplete.h"
#include "GUI_Autocomplete_Box.h"
#include "GUI_Search_Box.h"
#include "GUI_Path_Box.h"
#include "GA_DistanceMatrix.h"
#include <chrono>

using namespace std;

// Struct used for the KDTree::find_nearest_if function to differentiate
// names with the same street ID
struct Predicate{
    static unsigned streetID;
    
    bool operator()(const Point3D& rhs) const
    {
        return(rhs.getKey()==streetID);
    }
};

/*********************** TEST ****************************/
extern bool firstTime;
extern std::vector<unsigned> path;
/*********************************************************/
//Definitions of two intersections of path finding algorithm
extern unsigned intersectionFrom;
extern unsigned intersectionTo;
extern unsigned POITo;
extern std::string POI_Name;
//Definitions of two intersections that have been selected
extern bool intersectionFromSelected;
extern bool intersectionToSelected;

//Definition of data structures.
typedef std::pair<std::vector<unsigned>,std::vector<unsigned>> unsignedVectorPair;
typedef std::vector< unsignedVectorPair > idVectorPairVector;
typedef std::vector< std::vector<unsigned> > TwoDimVector;
typedef std::unordered_multimap<string,unsigned> stringToIdUnorderedMultimap;
typedef std::unordered_map<string,std::vector<unsigned>> stringToVectorUnorderedMap;
typedef std::unordered_map<unsigned,unsignedVectorPair> idToVectorPairMap;
typedef std::unordered_map<unsigned long long, unsigned> unsignedLongLongToUnsignedUnrderedMap;
typedef std::unordered_map<int, std::vector<unsigned>> FeatureTypeToUnsignedVector;
typedef KDTree::KDTree<3, Point3D> Tree3D;

extern double AVG_LAT;

extern double MAX_LAT;
extern double MIN_LAT;
extern double MAX_LON;
extern double MIN_LON;

/***************************************/
/* DECLARATION OF SOME DATA STRUCTURES */
extern std::vector<unsigned> streetSegMotorWay;
extern std::vector<unsigned> streetSegMajorRoads;
extern std::vector<unsigned> streetSegMinorRoads;
extern Tree3D majorIntersectionTree;
extern Tree3D minorIntersectiontree;
extern Tree3D motorwayIntersectionTree;

/********************************/
/*       GLOBAL VARIABLES       */
extern int current_city;            //Stores the current city ID
extern int map_selected;            //Stores the current number of map selected in map_selector

extern bool NameButtonPressed;      //Used to indicate if the name button is pressed
extern bool FindButtonPressed;      //Used to indicate if the search button is pressed
extern bool input_box_1_enabled;
extern bool input_box_2_enabled;
extern bool find_button_init_state;
extern bool showing_results_of_find_button;
extern bool showing_found_stuff;
extern bool show_incidents;
extern bool MapSelectorButtonPressed;
extern bool intersectionClicked;
extern bool legendButtonPressed;
extern bool draw_directives;
extern bool helpButtonPressed;
extern bool fillFromBox;
//extern int directiveVectorOffset;
//true is searchBox, false is pathbox
extern bool pathBoxSearchBoxSwitch;

extern LatLon intersectionPoint;

//extern t_bound_box window_size;     //Stores the current pop up window size
extern t_bound_box close_box_size;  //Stores the current pop up small close window size

/* for find window */
extern t_bound_box input_window1_size;
extern t_bound_box input_window2_size;
extern t_bound_box find_button_size;
extern t_bound_box back_button_size;

/* for map selector window */
extern t_bound_box map1_size;
extern t_bound_box map2_size;
extern t_bound_box map3_size;
extern t_bound_box map4_size;
extern t_bound_box map5_size;
extern t_bound_box map6_size;
extern t_bound_box map7_size;
extern t_bound_box map8_size;
extern t_bound_box ok_button_size;

extern std::string input1;
extern std::string input2;
extern std::vector<Point2D> found_stuff;
extern std::vector<unsigned> foundPath;
extern std::vector<string> foundDirectives;
extern t_bound_box initial_coords;

//DATA STRUCTURE IMPLEMENTATION
//Remember to redefine them when you are in other files.
extern stringToIdUnorderedMultimap streetIdToName;
extern idVectorPairVector streetIdToSegmentAndIntersectionId;
extern TwoDimVector insertsectionIdToSegmentId;
extern stringToVectorUnorderedMap streetIdToIntersectId;
extern std::vector<string> streetNames;
extern unsignedLongLongToUnsignedUnrderedMap OSM_way;
extern std::vector<LatLon> positionOfPOIs;
extern std::vector<std::vector<unsigned>> oneway_street_segments;
//extern FeatureTypeToUnsignedVector FeatureTypeToFeatureId;
extern unsignedLongLongToUnsignedUnrderedMap OSM_node;
extern FeatureTypeToUnsignedVector POITypeToPOIId;
extern std::vector<TrafficIncident> trafficIncidentsVector;
extern stringToVectorUnorderedMap streetNameToIntersectId;
extern stringToIdUnorderedMultimap nameToStreetId;
extern stringToIdUnorderedMultimap nameToPoiID;
extern DistanceMatrix distanceMatrix;

extern Tree3D intersectionsTree;
extern Tree3D pointOfInterestTree;
extern Tree3D trafficIncidentTree;
extern Trie namesDatabase;
extern AutoCompleteHandler autoComplete;
extern SearchBox searchBar;
extern PathBox pathBar;

// Two dimensional vector that stores features in the order
// in which they should be drawn
// the first dimension refers to what group of features should be drawn first
// the second dimension contains all the features in each group
extern TwoDimVector features;

extern Graph intersectionGraph;
//this is mainly for the multi-threading
extern std::vector<Graph> multiIntersectionGraph;

struct FeatureCategory{
    unsigned draw_order;
    unsigned draw_color;
    
    FeatureCategory(FeatureType featType){
        switch(featType){             
            case(Lake):
                draw_order = 1;
                draw_color = WATER;
                break;
                
            case(Island):
                draw_order = 2;
                draw_color = BGCOLOR;
                break;
                
            case(Park):
                draw_order = 3;
                draw_color = LAND;
                break;
                
            case(Greenspace):
                draw_order = 4;
                draw_color = GREENSPACE;
                break;
                
            case(Golfcourse):
                draw_order = 5;
                draw_color = GOLF_COURSE;
                break;
                
            case(River):    
                draw_order = 6;
                draw_color = WATER;
                break;
                
            case(Beach):
                draw_order = 7;
                draw_color = BEACH;
                break;
                
            case(Building):
                draw_order = 8;
                draw_color = BUILDING;
                break;   
                
            case(Shoreline):
                draw_order = 9;
                draw_color = LAND;
                break;
            
            default:
                draw_order = 0;
                draw_color = 0;
                break;
        }
    }
    
    FeatureCategory(int buildingType){
        switch(buildingType){
            case(APARTMENTS):
                draw_order = 9;
                draw_color = APARTMENTS_C;
                break;
            case(HOUSE):
                draw_order = 10;
                draw_color = HOUSE_C;
                break;
            case(RETAIL):
                draw_order = 11;
                draw_color = RETAIL_C;
                break;
            case(UNIVERSITY):
                draw_order = 12;
                draw_color = UNIVERSITY_WC;
                break;
            case(HOSPITAL_W):
                draw_order = 13;
                draw_color = HOSPITAL_WC;
                break;
        }    
    }
};

#endif /* GLOBAL_H */

