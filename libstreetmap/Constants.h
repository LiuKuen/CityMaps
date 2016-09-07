#ifndef CONSTANTS_H
#define CONSTANTS_H

// Change EXERCISE to 1 if you are running valgrind/exercise
#define EXERCISE        1

// m4 time limit
#define TIME_LIMIT 30
//#define POPULATION_GENERATION_TIME 2.0
#define POPULATION_GENERATION_SIZE 4000
#define MUTATION_REPETITION 3

//#define GREEDY_RANGE 3

extern float POPULATION_GENERATION_TIME;
extern unsigned GREEDY_RANGE;

#define LN10    2.302585093
/************************ Parameters for GA *****************************/
//#define KILL_PERCENTS   0.3
//#define GENESIZE_PERCENTS 0.1 //should always be less than 0.5
//#define MUTATION_PERCENTS   0.1

extern float KILL_PERCENTS;
extern float GENESIZE_PERCENTS;
extern float MUTATION_PERCENTS;


/************************************************************************/

// Change to 1 if you only want to run forward search
#define ONLY_FORWARD    1

// Change to 1 if you only want to run forward search
#define ONLY_BACKWARD    0

//use these defines if you need earth radius or conversion from degrees to radians
#define EARTH_RADIUS_IN_METERS 6372797.560856
#define DEG_TO_RAD 0.017453292519943295769236907684886

/******************************/
/*   PATH FINDING CONSTANTS   */
#define TURN_TIME       15

/********************/
/*  ROAD CONSTANTS  */
#define MOTORWAY        1
#define MAJORROADS      2
#define MINORROADS      3

/********************************/
/*    INTERSECTION CONSTANTS    */
#define MOTORWAYINTERSECTION    1
#define MAJORINTERSECTION       2
#define MINORINTERSECTION       3
#define ALLINTERSECTIONS        4

/************/
/*   MAPS   */
#define TORONTO         0
#define TORONTO_CANADA  1
#define NEWYORK         2
#define CAIRO_EGYPT     3
#define HAMILTON_CANADA 4
#define MOSCOW          5
#define SAINT_HELENA    6
#define LONDON_ENGLAND  7

/***************************************/
/*   ZOOM LEVELS UNDER WHICH TO DRAW   */
#define BUILDING_LEVEL  3
#define INCIDENT_LEVEL  9
#define PARK_LEVEL      12

/*********************/
/*   COLOR CONSTANT  */

#define BGcolor         0xE9E5DC
//first two digits determine Redness, next two is greeness, last two is blueness
//for roads
#define MOTORWAY_ORANGE 0xFA9E20
#define GREY1           0xE1DCD3
#define GREY2           0xF0EEE9
#define BGCOLOR         0xE9E5DC
#define WHITE_HEX       0xFFFFFF

//for features
#define LAND            0xCAE8AA
#define WATER           0x90caf9
#define BUILDING        0xF4F3EC
#define BEACH           0xFAF2C7
#define INTERSECTION    0xFF00FF
#define GREENSPACE      0x81C784
#define GOLF_COURSE     0x00695C

//for POI colours 
#define RESTAURANT_C    0xE91E63
#define SHOP_C          0x009688
#define UNIVERSITY_NC   0x673AB7 //node colour
#define HOSPITAL_NC     0xF4511E //node colour
#define BANK_C          0x607D8B
#define LIBRARY_C       0xFF5722

//  building colours
#define APARTMENTS_C    0x880E4F
#define HOUSE_C         0xD7CCC8
#define RETAIL_C        0xFFE0B2
#define UNIVERSITY_WC   0xE8DDBD //way colour
#define HOSPITAL_WC     0xEBD2CF //way colour


//For incidents
#define INCIDENT_COLOR 0xF44336
/********************/

#define NUMBER_OF_FEATURE_CATEGORIES 14

//  building types for feature categories
#define APARTMENTS      9
#define HOUSE           10
#define RETAIL          11
#define UNIVERSITY      12
#define HOSPITAL_W      13 //hospital way


//  poi types  
#define SHOP            0
#define COLLEGE         1
#define HOSPITAL_N      2 //hospital node
#define BANK            3
#define LIBRARY         4

#define LIGHT_BLUE      0x2196F3
#define White           0xFFFFFF
#define Black           0x000000

#define NUMBER_OF_CORES 8

struct DeliveryInfo {
    //Specifies a delivery order.
    //
    //To satisfy the order the item-to-be-delivered must have been picked-up 
    //from the pickUp intersection before visiting the dropOff intersection.

    DeliveryInfo(unsigned pick_up, unsigned drop_off)
        : pickUp(pick_up), dropOff(drop_off) {}


    //The intersection id where the item-to-be-delivered is picked-up.
    unsigned pickUp;

    //The intersection id where the item-to-be-delivered is dropped-off.
    unsigned dropOff;
};

enum DeliveryType {
    Pickup = 0,
    Dropoff,
    Depot
};

#endif /* CONSTANTS_H */

