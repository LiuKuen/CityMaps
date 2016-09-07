#pragma once //protects against multiple inclusions of this header file

#include "Global_var.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "Constants.h"
#include "B_TrafficIncident.h"
#include <algorithm>

using namespace std;

/***********FUNCTION DECLARATION*********************/

//Initialize all data structures
void init_layer_2_data_structure();

void init_layer_1_data_structure();

void init_streetIdToSegmentAndIntersectionId_intersection();

void init_insertsectionIdToSegmentId();

void init_streetIdToName_AND_streetNames();

void init_streetIdToIntersectId();

void init_streetIdToSegmentAndIntersectionId_streetSegment();

void init_intersectionsTree();

void init_pointOfInterestTree();

void init_OSM_way();

void init_OSM_node();

void init_POI_position();

void init_oneway_street_segments();

void init_features();

void init_sub_intersectionsTree();

void init_POI_types();

void init_intersectionGraph();

void init_POI_names_hash_table();

//for the autocomplete of intersection names and POI names
void init_names_database();

//delete all data structurs 
void delete_data_structure();

//function to load map streets.bin file
bool load_map(std::string map_name);

//close the loaded map
void close_map();

//function to return street id(s) for a street name
//return a 0-length vector if no street with this name exists.
std::vector<unsigned> find_street_ids_from_name(std::string street_name);

//function to return the street segments for a given intersection 
std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id);

//function to return street names at an intersection (include duplicate street names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersection_id);

// Determines whether the given street segment can go to the provided intersection
// Takes into account one way streets
inline bool canGetToIntersection(StreetSegmentInfo streetSeg, unsigned intersectionID){
    //check if street either goes to the intersection...
    if(streetSeg.to == intersectionID)
        return true;
    //...or comes from it but is a two-way street
    if(streetSeg.from == intersectionID && !streetSeg.oneWay)
        return true;
    return false;
}

//can you get from intersection1 to intersection2 using a single street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2);

//find all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id);

//for a given street, return all the street segments
std::vector<unsigned> find_street_street_segments(unsigned street_id);

//for a given street, find all the intersections
std::vector<unsigned> find_all_street_intersections(unsigned street_id);


//function to return all intersection ids for two intersecting streets
//this function will typically return one intersection id between two street names
//but duplicate street names are allowed, so more than 1 intersection id may exist for 2 street names
std::vector<unsigned> find_intersection_ids_from_street_names(std::string street_name1, std::string street_name2);

//find distance between two latitude/longitude coordinates
double find_distance_between_two_points(LatLon point1, LatLon point2);

//find the length of a given street segment
double find_street_segment_length(unsigned street_segment_id);

//find the length of a whole street
double find_street_length(unsigned street_id);

//find the travel time to drive a street segment (time(minutes) = distance(km)/speed_limit(km/hr) * 60
double find_street_segment_travel_time(unsigned street_segment_id);

//find the nearest point of interest to a given position
unsigned find_closest_point_of_interest(LatLon my_position);

//find the nearest intersection (by ID) to a given position
unsigned find_closest_intersection(LatLon my_position);

//find the nearest traffic incident (by ID) to a given position
unsigned find_closest_traffic_incident(LatLon my_position);

//find the nearest intersections to a given position within a range
unsigned find_closest_intersection_within_range(LatLon my_position, double range);

std::vector<unsigned> find_one_way_street_segments(unsigned);

/* this function contains all initializations of the map */
void map_init(int map_name);

/* update message to city name */
void update_message_to_current_city();

/* initialization of vectors which classify street based on OSMWay highway types */
void init_street_segment_classification(void);

inline unsigned otherNode(StreetSegmentInfo segInfo, unsigned intersectionID){
    if (intersectionID == segInfo.to)
        return segInfo.from;
    else
        return segInfo.to;
}
