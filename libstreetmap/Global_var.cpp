/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <Global_var.h>

double AVG_LAT;

double MAX_LAT;
double MIN_LAT;
double MAX_LON;
double MIN_LON;

/*********************** TEST ****************************/
bool firstTime;
std::vector<unsigned> path;
/*********************************************************/



/* vectors of different kind of street segments */
std::vector<unsigned> streetSegMotorWay;
std::vector<unsigned> streetSegMajorRoads;
std::vector<unsigned> streetSegMinorRoads;

/* kdtrees for different kind of intersections */
Tree3D majorIntersectionTree;
Tree3D minorIntersectiontree;
Tree3D motorwayIntersectionTree;

//Definition of two intersections of path finding algorithm
unsigned intersectionFrom;
unsigned intersectionTo;
unsigned POITo;
std::string POI_Name;

//Definitions of two intersections that have been selected
bool intersectionFromSelected;
bool intersectionToSelected;

/* Global varibles definitions; see declaritions in m2.h */
int current_city;
int map_selected;

bool NameButtonPressed;
bool FindButtonPressed;
bool MapSelectorButtonPressed;
bool input_box_1_enabled;
bool input_box_2_enabled;
bool find_button_init_state;
bool showing_results_of_find_button;
bool show_incidents;
bool showing_found_stuff;
bool intersectionClicked;
bool legendButtonPressed;
bool helpButtonPressed;
bool pathBoxSearchBoxSwitch;
bool draw_directives;
bool fillFromBox;

int directiveVectorOffset;

LatLon intersectionPoint;

t_bound_box window_size; 
t_bound_box close_box_size;
t_bound_box input_window1_size;
t_bound_box input_window2_size;
t_bound_box find_button_size;
t_bound_box back_button_size;
t_bound_box map1_size;
t_bound_box map2_size;
t_bound_box map3_size;
t_bound_box map4_size;
t_bound_box map5_size;
t_bound_box map6_size;
t_bound_box map7_size;
t_bound_box map8_size;
t_bound_box ok_button_size;

std::string input1;
std::string input2;
std::vector<Point2D> found_stuff;
std::vector<unsigned> foundPath;
std::vector<string> foundDirectives;

std::vector<TrafficIncident> trafficIncidentsVector;

t_bound_box initial_coords = t_bound_box(-90,-90,90,90); //the values in this line don't matter

TwoDimVector features;

// 3 dimensional trees used for nearest neighbour searches
Tree3D intersectionsTree;
Tree3D pointOfInterestTree;
Tree3D trafficIncidentTree;

//hash table for find_street_ids_from_name()
stringToIdUnorderedMultimap nameToStreetId;

stringToIdUnorderedMultimap nameToPoiID;

// vector of pairs where index is a street ID
// and the stored values are pairs such that:
//  first:  vector of street segments ID's
//  second: vector of intersection ID's
idVectorPairVector  streetIdToSegmentAndIntersectionId;

// vector of std::vector<unsigned> where the indexes are intersection ID's and
// the elements are vectors of street segment ID's at each intersection
TwoDimVector insertsectionIdToSegmentId;

// hash table where the keys are street names and the values are sorted vectors
// of every intersection associated with that street name
stringToVectorUnorderedMap streetNameToIntersectId;

// sorted vector where the values are street names
// used to identify duplicate street names faster
std::vector<string> streetNames;

unsignedLongLongToUnsignedUnrderedMap OSM_way;

unsignedLongLongToUnsignedUnrderedMap OSM_node;


//vector that contains all the locations of POIs
std::vector<LatLon> positionOfPOIs;

//contains a vector of street segments that are one way
std::vector<std::vector<unsigned>> oneway_street_segments;

//contains POIs organized to their respective categories
FeatureTypeToUnsignedVector POITypeToPOIId;

DistanceMatrix distanceMatrix;

Graph intersectionGraph;
std::vector<Graph> multiIntersectionGraph;

Trie namesDatabase;
AutoCompleteHandler autoComplete;
SearchBox searchBar;
PathBox pathBar;

float KILL_PERCENTS = 0.3;
float GENESIZE_PERCENTS = 0.1;
float MUTATION_PERCENTS = 0.1;

float POPULATION_GENERATION_TIME = 2.0;
unsigned GREEDY_RANGE = 2;
