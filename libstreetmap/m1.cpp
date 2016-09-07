#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include <unordered_map>
#include <algorithm>
#include "stdlib.h"
#include <algorithm>
#include "time.h"
#include "Global_var.h"
#include "B_Trie.h"
#include "B_Graph.h"


//If set too 1, program outputs load time for each datastructure
#define OUTPUT_BUILD_TIMES 0



//load the map
bool load_map(std::string map_name) 
{
    POITo = std::numeric_limits<unsigned>::infinity();
    clock_t t;
    t = clock();
    
    std::string map_name_OSM = map_name;
    std::string::size_type a = map_name_OSM.find("streets");
        map_name_OSM.replace(a,7,"osm");
        
    bool load_success = loadOSMDatabaseBIN(map_name_OSM);
    t = clock()-t;
    cout << "loadOSMDatabaseBIN "<< ((float)t)/CLOCKS_PER_SEC<< " seconds." << endl;    
    t = clock();
    
    load_success = load_success && loadStreetsDatabaseBIN(map_name);
    
    // create any data structures here to speed up your API functions
    init_layer_2_data_structure();
    
    init_layer_1_data_structure();   
    
    if((a = map_name.find("toronto_canada"))!=string::npos)
        map_init(TORONTO_CANADA);
    else if((a = map_name.find("toronto"))!=string::npos)
        map_init(TORONTO);
    else if((a = map_name.find("newyork"))!=string::npos)
        map_init(NEWYORK);
    else if((a = map_name.find("moscow"))!=string::npos)
        map_init(MOSCOW);
    else if((a = map_name.find("hamilton_canada"))!=string::npos)
        map_init(HAMILTON_CANADA);
    else if((a = map_name.find("saint_helena"))!=string::npos)
        map_init(SAINT_HELENA);
    else if((a = map_name.find("cairo_egypt"))!=string::npos)
        map_init(CAIRO_EGYPT);
    else if((a = map_name.find("london_england"))!=string::npos)
        map_init(LONDON_ENGLAND);
    else
        assert(("This line should never be executed!",1==0));
    
    t = clock()-t;
    cout << "Loading data structures took "<< ((float)t)/CLOCKS_PER_SEC<< " seconds." << endl;
    
    return load_success;
}


//Initialize all data structures
void init_layer_2_data_structure()
{
    cout << "Creating all data structures..." << endl;
    
    //Time data structures build
    int numLoops(8);
    clock_t t[numLoops];
    t[0] = clock();
    
    const unsigned numberOfStreets = getNumberOfStreets();
    const unsigned numberOfIntersections = getNumberOfIntersections();
    
    //"Pre-allocate" memory for vector data structures
    streetIdToSegmentAndIntersectionId.resize(numberOfStreets+1);
    insertsectionIdToSegmentId.resize(numberOfIntersections+1);
    
    //Generate all the data structures
    
    /*LOOP #1*/
    t[1] = clock();
    //initialize streetIdToSegmentAndIntersectionId_intersection
    init_streetIdToSegmentAndIntersectionId_intersection();
    t[1]=clock()-t[1];
    
    /*LOOP #2*/
    t[2] = clock();
    //initialize insertsectionIdToSegmentId
    init_insertsectionIdToSegmentId();
    t[2]=clock()-t[2];
    
    /*LOOP #3*/
    t[3] = clock();
    init_streetIdToName_AND_streetNames();
    t[3]=clock()-t[3];

    std::sort(streetNames.begin(),streetNames.end());
    
    /*LOOP #4*/
    t[4] = clock();
    //go through every street names
    init_streetIdToIntersectId();
    t[4]=clock()-t[4];

    /*LOOP #5*/
    t[5] = clock();
    //insert all street segment index to streetID2IntersAndSegsVector
    //loop through every street segment
    init_streetIdToSegmentAndIntersectionId_streetSegment();
    t[5]=clock()-t[5];
    
    /*LOOP #6*/
    t[6] = clock();
    //insert all intersections in a 3 dimensional tree
    init_intersectionsTree();
    t[6]=clock()-t[6];
    
    /*LOOP #7*/
    t[7] = clock();
    //insert all POIs in a 3 dimensional tree
    init_pointOfInterestTree();
    t[7]=clock()-t[7];
    
    /*LOOP #8*/
    t[8] = clock();
    //insert all features in a 2 dimensional vector
    t[8]=clock()-t[8];
           
    init_POI_position();
    
    init_intersectionGraph();
    
    init_names_database();
    
    init_POI_names_hash_table();
    
    /*****************************testing**************************************/
    std::vector<DeliveryInfo> deliveries;
    std::vector<unsigned> depots;
    
    deliveries = {DeliveryInfo(105225, 56051), 
                  DeliveryInfo(104672, 66348),
                  DeliveryInfo(98141, 84950), 
                  DeliveryInfo(97060, 67133), 
                  DeliveryInfo(88129, 84132), 
    
                  DeliveryInfo(64436, 20885), 
                  DeliveryInfo(28619, 64489),
                  DeliveryInfo(79055, 10192),
                  DeliveryInfo(56060, 24341)};
    depots = {2, 9256};
    
    //traveling_courier(deliveries, depots);
    /**************************************************************************/
    #if OUTPUT_BUILD_TIMES
    for(int i=1; i<numLoops; i++)
        cout << "LOOP #"<< i << " took "<< ((float)t[i])/CLOCKS_PER_SEC<< " seconds." << endl;
    #endif
    
    cout << "Finished creating all data structures..." << endl;
    t[0] = clock()-t[0];
    cout << "Initialization took "<< ((float)t[0])/CLOCKS_PER_SEC<< " seconds." << endl;
}

void init_streetIdToSegmentAndIntersectionId_intersection(){
    
    MIN_LAT = 91;
    MAX_LAT = -91;
    MIN_LON = 91;
    MAX_LON = -91;
    
    const unsigned numberOfIntersections = getNumberOfIntersections();
    for(unsigned intersection_id = 0; intersection_id < numberOfIntersections; intersection_id++)
    {
        LatLon position = getIntersectionPosition(intersection_id);
        
        if(position.lat<MIN_LAT)
            MIN_LAT = position.lat;
        if(position.lat>MAX_LAT)
            MAX_LAT = position.lat;
        
        if(position.lon<MIN_LON)
            MIN_LON = position.lon;
        if(position.lon>MAX_LON)
            MAX_LON = position.lon;
        
        //number of street segments connected to that intersection
        unsigned numberOfStreetSegments = getIntersectionStreetSegmentCount(intersection_id);
        //loop through every street segment
        for(unsigned i = 0; i < numberOfStreetSegments; i++)
        {
            unsigned streetSegmentId = getIntersectionStreetSegment(intersection_id,i);
            unsigned streetId = getStreetSegmentInfo(streetSegmentId).streetID;
            streetIdToSegmentAndIntersectionId[streetId].first.push_back(intersection_id);
        }//end of nested for loop
    }//end of for loop
    
    AVG_LAT = (MAX_LAT+MIN_LAT)/2;
}

void init_insertsectionIdToSegmentId(){
    const unsigned numberOfIntersections = getNumberOfIntersections();
    for(unsigned intersection_id = 0; intersection_id<numberOfIntersections; intersection_id++)
        {
            unsigned numOfStSeg = getIntersectionStreetSegmentCount(intersection_id);
            for(unsigned i = 0; i < numOfStSeg; i++)
            {
                insertsectionIdToSegmentId[intersection_id].push_back(getIntersectionStreetSegment(intersection_id,i));
            }
        }
}

void init_streetIdToName_AND_streetNames(){
    const unsigned numberOfStreets = getNumberOfStreets();
    for(unsigned street_id = 0; street_id < numberOfStreets; street_id++)
        {
            //insert street names to streets_d
            nameToStreetId.insert(stringToIdUnorderedMultimap::value_type(getStreetName(street_id), street_id));
            streetNames.push_back(getStreetName(street_id));
        }
}

void init_streetIdToIntersectId(){
    unsigned streetNamesSize = streetNames.size();
    for(unsigned street_name_index = 0; street_name_index < streetNamesSize; street_name_index++)
        {
            if(street_name_index == 0 || (streetNames[street_name_index] != streetNames[street_name_index-1]))
            {
                //get the index array associated with one street name
                std::vector<unsigned> ids = find_street_ids_from_name(streetNames[street_name_index]);
                //allIntersections stores all intersections ids with one street name
                std::vector<unsigned> allIntersections;
                //go through every street with one street name
                for(unsigned j = 0; j < ids.size(); j++)
                {
                    //temp stores intersec ids for one street id
                    std::vector<unsigned> temp = find_all_street_intersections(ids[j]);
                    //append index vector to allIntersections
                    allIntersections.insert(allIntersections.end(),temp.begin(), temp.end());
                }
                //sort allIntersections
                std::sort(allIntersections.begin(), allIntersections.end());
                //insert allIntersections
                streetNameToIntersectId.emplace(streetNames[street_name_index], allIntersections);
            }
        }
}

void init_streetIdToSegmentAndIntersectionId_streetSegment(){
    const unsigned numberOfStreetSegments = getNumberOfStreetSegments();
    for(unsigned street_segment_id = 0; street_segment_id < numberOfStreetSegments; street_segment_id++)
    {
        unsigned street_id = getStreetSegmentInfo(street_segment_id).streetID;
        //insert the street segment at the beginning
        streetIdToSegmentAndIntersectionId[street_id].second.push_back(street_segment_id);
    }
}

void init_intersectionsTree(){
    const unsigned numberOfIntersections = getNumberOfIntersections();
    for(unsigned intersection_id = 0; intersection_id < numberOfIntersections; intersection_id++)
    {
        Point3D newPoint(getIntersectionPosition(intersection_id), intersection_id);
        intersectionsTree.insert(newPoint);
    }
}

void init_pointOfInterestTree(){
    const unsigned numberOfPOIs = getNumberOfPointsOfInterest();
    for(unsigned  POI_id=0; POI_id<numberOfPOIs; POI_id++)
        {
            Point3D newPoint(getPointOfInterestPosition(POI_id), POI_id);
            pointOfInterestTree.insert(newPoint);
        }
}

void init_layer_1_data_structure(){
    init_OSM_way();
    init_OSM_node();
    //initializes all building types
    init_POI_types();
    init_features();
    //init_building_types();
}

void init_OSM_way(){
    unsigned long long numberOfOSMWays = getNumberOfWays();
    
    for (unsigned long long i=0;i<numberOfOSMWays;i++){
        const OSMWay* theWay=getWayByIndex(i);
        OSM_way.emplace(theWay->id(),i);
    }
}

void init_OSM_node(){
    unsigned numberOfOSMNodes = getNumberOfNodes();
    
    for (unsigned i = 0; i < numberOfOSMNodes; i++){
        const OSMNode* theNode = getNodeByIndex(i);
        OSM_node.emplace(theNode->id(),i);
    }
}

void init_POI_position(){
    const unsigned numberOfPointsOfInterest = getNumberOfPointsOfInterest();
    
    //each index in the vector corresponds to a POI id. Each element in the vector
    //corresponds to a position of a specific index (POI )
    for(unsigned i = 0; i < numberOfPointsOfInterest; i++){
        positionOfPOIs.push_back(getPointOfInterestPosition(i));
    }
}

void init_oneway_street_segments(){   
    unsigned numberOfStreets = getNumberOfStreets();
    
    for(unsigned i = 0; i < numberOfStreets; i++){
        std::vector<unsigned> street_segment = find_street_street_segments(i);
    }
}

void init_features(){
    //note the features data structure is logically split into two distinct features
    // 1) natural features + manmade features of the earth
    // 2) buildings
    
    const unsigned numberOfFeatures = getNumberOfFeatures();
    features.resize(NUMBER_OF_FEATURE_CATEGORIES);
    
    for(unsigned featureID = 0; featureID < numberOfFeatures; featureID++){ 
        //check for building types
        OSMID OSMFeatureId = getFeatureOSMID(featureID);
        const OSMWay* OSMFeatureWay = getWayByIndex(OSM_way[OSMFeatureId]); 
        unsigned tagCount = getTagCount(OSMFeatureWay);
        std::pair<std::string,std::string> featureTypeTag;
        
        for(unsigned tagCounter = 0; tagCounter < tagCount; tagCounter++){
                featureTypeTag = getTagPair(OSMFeatureWay, tagCounter);
                //filter out specific tags and load them into the data structure
                //the constants are already lined up properly
                if(featureTypeTag.second == "apartments"){
                    
                    features[APARTMENTS].push_back(featureID);
                }
                else if(featureTypeTag.second == "house"){
                    
                    features[HOUSE].push_back(featureID);
                }
                else if(featureTypeTag.second == "retail"){ 
                    
                    features[RETAIL].push_back(featureID);
                }
                else if(featureTypeTag.second == "university"){
                    
                    features[UNIVERSITY].push_back(featureID);
                }
                else if(featureTypeTag.second == "hospital"){
                    
                    features[HOSPITAL_W].push_back(featureID);
                }
            }//end of first for loop
        
        //check for the rest of the feature types
        FeatureType featType = getFeatureType(featureID);
        FeatureCategory featCategory(featType);
        
        // Check if valid feature type
        if(featCategory.draw_color)    
            features[featCategory.draw_order].push_back(featureID);
    }
}

void init_POI_types(){
    const unsigned numberOfPointsOfInterest = getNumberOfPointsOfInterest();
    
    POITypeToPOIId.reserve(5);
    
    //go through the POIs and sort them to their respective categories
    for(unsigned POIID = 0; POIID < numberOfPointsOfInterest; POIID++){
        OSMID pointOfInterestId = getPointOfInterestOSMNodeID(POIID);
        
        const OSMNode* POINode = getNodeByIndex(OSM_node[pointOfInterestId]);
        unsigned numberOfTags = getTagCount(POINode);
        
        std::pair<std::string,std::string> POITag;
              
        //sort the id's into their respective categories
        for(unsigned tagCount = 0; tagCount < numberOfTags; tagCount++){
            //every OSMNode has a list of tags, all we want are the ones that are
            //relevant to us such as the ones we below
            POITag = getTagPair(POINode, tagCount);

            if(POITag.first == "shop"){
                POITypeToPOIId[SHOP].push_back(POIID);
            }
            else if(POITag.second == "university"){
                POITypeToPOIId[COLLEGE].push_back(POIID);
            }
            else if(POITag.second == "hospital"){
                POITypeToPOIId[HOSPITAL_N].push_back(POIID);
            }
            else if(POITag.second == "bank"){
                POITypeToPOIId[BANK].push_back(POIID);
            }
            else if(POITag.second == "library"){
                POITypeToPOIId[LIBRARY].push_back(POIID);
            }
        }// end of inner for loop
    }// end of outer for loop
}

void init_names_database(){
    unsigned numberOfIntersections = getNumberOfIntersections();
    
    for(unsigned intersectionIndex = 0; intersectionIndex < numberOfIntersections; intersectionIndex++){
        string intersectionName = getIntersectionName(intersectionIndex);
        namesDatabase.addWord(intersectionName);
    }
    
    unsigned numberOfPointsOfInterest = getNumberOfPointsOfInterest();
    
    for(unsigned poiIndex = 0; poiIndex < numberOfPointsOfInterest; poiIndex++){
        string poiName = getPointOfInterestName(poiIndex);
        namesDatabase.addWord(poiName);
    }
    
    unsigned numberOfStreets = getNumberOfStreets();
    
    for(unsigned nameIndex = 0; nameIndex < numberOfStreets; nameIndex++){
        string streetName = getStreetName(nameIndex);
        namesDatabase.addWord(streetName);
    }
}

// Initialize the graph
void init_intersectionGraph(){
    
    //resize to eight because the computer can only handle running 8 threads simultaneously  
    multiIntersectionGraph.resize(8);
    
    unsigned numIntersections = getNumberOfIntersections();
    for(unsigned intersectionID=0; intersectionID<numIntersections; intersectionID++)
    {
        intersectionGraph.insert(intersectionID);
        for(unsigned i = 0; i < NUMBER_OF_CORES; i++){
            multiIntersectionGraph[i].insert(intersectionID);
        }
    }
    
    for(unsigned i = 0; i < NUMBER_OF_CORES; i++){
        multiIntersectionGraph[i].connectVertices();
    }
    intersectionGraph.connectVertices();
}

void init_POI_names_hash_table(){
    const unsigned numberOfPointsOfInterest = getNumberOfPointsOfInterest();
     
    for(unsigned i = 0; i < numberOfPointsOfInterest; i++)
        nameToPoiID.insert(stringToIdUnorderedMultimap::value_type(getPointOfInterestName(i), i));
}



//delete all data structures
void delete_data_structure()
{
    cout << "Deleting all data structures..." << endl;
    
    nameToStreetId.clear();
    nameToPoiID.clear();
    streetNameToIntersectId.clear();
    streetIdToSegmentAndIntersectionId.clear();
    insertsectionIdToSegmentId.clear();
    
    intersectionsTree.clear();
    pointOfInterestTree.clear();
    trafficIncidentTree.clear();
    
    OSM_node.clear();
    OSM_way.clear();
    features.clear();
    
    positionOfPOIs.clear();
    POITypeToPOIId.clear();
    
    streetSegMotorWay.clear();
    streetSegMajorRoads.clear();
    streetSegMinorRoads.clear();
    
    majorIntersectionTree.clear();
    minorIntersectiontree.clear();
    motorwayIntersectionTree.clear();
    
    intersectionGraph.clear();
    namesDatabase.freeTrie();
    multiIntersectionGraph.clear();
    
    cout << "Finish deleting all data structures..." << endl;
}

//close the map

void close_map()
{
    closeOSMDatabase();
    closeStreetDatabase();
    // destroy/clear any data structures you created in load_map
    delete_data_structure();
}

//function to return street id(s) for a street name
//return a 0-length vector if no street with this name exists.
std::vector<unsigned> find_street_ids_from_name(std::string street_name)
{
    std::vector<unsigned> vecs;
    
    auto its = nameToStreetId.equal_range(street_name);
    for (auto iterator = its.first; iterator != its.second; ++iterator) {
        vecs.push_back(iterator->second);
    }
    return vecs;
}

//function to return street names at an intersection (include duplicate street names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersection_id) 
{
    //get the ID's of all of the street segments going through this intersection
    vector<unsigned> *segments = &insertsectionIdToSegmentId[intersection_id];
    
    //declare vector of names of the segments
    std::vector<std::string> names;
    unsigned streetID;
    
    //get streetID of each segment and add the name of the street to the returned vector
    for(unsigned i = 0; i< segments->size(); i++)
    {
        streetID = getStreetSegmentInfo((*segments)[i]).streetID;
        names.push_back(getStreetName(streetID));
    }
    
    return names;
}

// Function that return whether you can go from intersection 1 to intersection 2 
// along a single street segment
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) 
{
    // corner case: an intersection is connected to itself
    if(intersection_id1==intersection_id2)
        return true;
    
    unsigned numSeg1 = getIntersectionStreetSegmentCount(intersection_id1);
    vector<unsigned> *Segs1 = &insertsectionIdToSegmentId[intersection_id1];    
    StreetSegmentInfo StSegInfo;

    // check if any of the street segments from intersection 1 connects to intersection 2
    for(unsigned i=0; i<numSeg1; i++)
    {
        StSegInfo = getStreetSegmentInfo((*Segs1)[i]);
        if( canGetToIntersection(StSegInfo, intersection_id2))
            return true; 
    }
    return false;
}

//find all intersections reachable by traveling down one street segment 
//from given intersection taking into account one way streets
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id) 
{
    
    std::vector<unsigned> streetSegments = insertsectionIdToSegmentId[intersection_id];
    std::vector<unsigned> adjacentIntersections;
    
    unsigned numStreetSegments = streetSegments.size();
    unsigned otherNodeIndex;
    
    for(unsigned i = 0; i < numStreetSegments; i++)
    {
        //  the info for the ith street ssegment
        StreetSegmentInfo stSegInfo = getStreetSegmentInfo(streetSegments[i]);
        
        //get index that is not the starting intersection
        if(intersection_id == stSegInfo.from)
            otherNodeIndex = stSegInfo.to;
        else
            otherNodeIndex = stSegInfo.from;
        
        //check for one way streets and add new intersection to return vector if it is not already in it
        if(canGetToIntersection(stSegInfo, otherNodeIndex)){
            //Check if it is already in the return vector
            auto it = find(adjacentIntersections.begin(),adjacentIntersections.end(), otherNodeIndex);
            if(it==adjacentIntersections.end())
                adjacentIntersections.push_back(otherNodeIndex);            
        }
    }//end of for loop
    return adjacentIntersections;
}

// Function that returns a vector of all the intersection between 
// any two streets that are names name1 and name 2
std::vector<unsigned> find_intersection_ids_from_street_names
(std::string street_name1, std::string street_name2) 
{
        //vector for all street segments that have the same intersection
        std::vector<unsigned> intersectionsFromStreetNames;

        //finds street ids from names
        std::vector<unsigned> streetName1Id = find_street_ids_from_name(street_name1);
        std::vector<unsigned> streetName2Id = find_street_ids_from_name(street_name2);

        //declare return values
        std::vector<unsigned>street1Intersecs = streetNameToIntersectId[street_name1];
        std::vector<unsigned>street2Intersecs = streetNameToIntersectId[street_name2];

        //If the two inputs are not the same, proceed normally
        if(street_name1 != street_name2){
            
            //find the common elements between the two vectors
            std::set_intersection(
                    begin(street1Intersecs), 
                    end(street1Intersecs), 
                    begin(street2Intersecs), 
                    end(street2Intersecs), 
                    back_inserter(intersectionsFromStreetNames)
            );
            
        // If the two street names are the same, look for any intersection that
        // has at least 3 street segments stemming out of it with the street name 
        } else{
            
            unsigned numIntersecs = street1Intersecs.size();
            unsigned streetNameCount, currentIntersec;
            std::vector<std::string> currentSegments;
            
            // Cycle through every intersection
            for(unsigned i=0; i<numIntersecs; i++){
                currentIntersec = street1Intersecs[i];
                currentSegments = find_intersection_street_names(currentIntersec);
                
                //Count how many times the street_name appears at every intersection
                streetNameCount = std::count(currentSegments.begin(), currentSegments.end(), street_name1);
                
                //If the name apears at least 3 times, then the street intersects with itself
                if(streetNameCount >= 3)
                    intersectionsFromStreetNames.push_back(currentIntersec);
            }
        }

        return intersectionsFromStreetNames;
}

double find_distance_between_two_points(LatLon point1, LatLon point2) {
    double localLatAverage = (point1.lat + point2.lat) / 2 * DEG_TO_RAD;

    // convert Lat Long to Cartesian coordinates
    // Note: LatLon is in radians
    double delta_x = (point1.lon - point2.lon) *  cos(localLatAverage);
    double delta_y = point2.lat - point1.lat;

    //Find distance using the Pythagorean theorem
    //NOTE: DEG_TO_RAD is factored from x1, x2, y1 and y2
    return std::sqrt((delta_y)*(delta_y) + (delta_x)*(delta_x)) * EARTH_RADIUS_IN_METERS * DEG_TO_RAD;
}

double find_street_segment_length(unsigned street_segment_id) 
{
    StreetSegmentInfo streetSegment = getStreetSegmentInfo(street_segment_id);
    double distance = 0;
    
    //get the beginning point and end point of a street segment
    LatLon point1 = getIntersectionPosition(streetSegment.from);
    LatLon point2 = getIntersectionPosition(streetSegment.to);
    
    unsigned numberOfCurvedPoints = streetSegment.curvePointCount;
    
    if(numberOfCurvedPoints == 0)
        return find_distance_between_two_points(point1, point2);
    
    for(unsigned indexOfCurvePoint=0; indexOfCurvePoint < numberOfCurvedPoints - 1; indexOfCurvePoint++)
    {
        distance += find_distance_between_two_points(getStreetSegmentCurvePoint(street_segment_id,indexOfCurvePoint),
                getStreetSegmentCurvePoint(street_segment_id,indexOfCurvePoint+1));
    }
    return distance + find_distance_between_two_points(point1, getStreetSegmentCurvePoint(street_segment_id, 0)) +
            find_distance_between_two_points(point2, getStreetSegmentCurvePoint(street_segment_id, numberOfCurvedPoints-1));
}

//Return the total length of a street
double find_street_length(unsigned street_id) 
{
    std::vector<unsigned> streetSegmentIds;
    double distance = 0;
    
    //returns a vector of all the street segments in a given street id
    streetSegmentIds = find_street_street_segments(street_id);
    
    //adds all the length of each street segment (in meters)
    for(unsigned i = 0; i< streetSegmentIds.size(); i++)
        distance += find_street_segment_length(streetSegmentIds[i]);
    
    return distance;
}

// Find the time it takes to travel a street segment at 60kph
double find_street_segment_travel_time(unsigned street_segment_id) 
{
    //distance is converted from meters to km
    double distance = find_street_segment_length(street_segment_id) / 1000;
    StreetSegmentInfo streetSegment = getStreetSegmentInfo(street_segment_id);
    
    //speed limit is in km/hour
    return distance / streetSegment.speedLimit * 60;
}

//Finds the closest point of interest to a given location
unsigned find_closest_point_of_interest(LatLon my_position) 
{
    //Convert lat/lon to a 3D point
    Point3D my_pos(my_position, 3);
    
    //Calls the KDTree::find_nearest()
    std::pair <Tree3D::const_iterator, double> found(pointOfInterestTree.find_nearest(my_pos));
    Point3D foundPoint(*(found.first));
    return foundPoint.getKey();
}

// Find the closest intersection to a given location
unsigned find_closest_intersection(LatLon my_position) 
{
    //Convert lat/lon to a 3D point
    Point3D my_pos(my_position, 3);
    
    //Calls the KDTree::find_nearest()
    std::pair <Tree3D::const_iterator, double> found(intersectionsTree.find_nearest(my_pos));
    Point3D foundPoint(*(found.first));
    return foundPoint.getKey();
}

// Find the closest intersection to a given location
unsigned find_closest_traffic_incident(LatLon my_position) 
{
    //Convert lat/lon to a 3D point
    Point3D my_pos(my_position, 3);
    
    //Calls the KDTree::find_nearest()
    std::pair <Tree3D::const_iterator, double> found(trafficIncidentTree.find_nearest(my_pos));
    Point3D foundPoint(*(found.first));
    return foundPoint.getKey();
}

std::vector<unsigned> find_all_street_intersections(unsigned street_id)
{
    sort(streetIdToSegmentAndIntersectionId[street_id].first.begin(),
            streetIdToSegmentAndIntersectionId[street_id].first.end());
    streetIdToSegmentAndIntersectionId[street_id].first.erase(unique(streetIdToSegmentAndIntersectionId[street_id].first.begin(),
            streetIdToSegmentAndIntersectionId[street_id].first.end()),streetIdToSegmentAndIntersectionId[street_id].first.end());
    return streetIdToSegmentAndIntersectionId[street_id].first;
}

std::vector<unsigned> find_street_street_segments(unsigned street_id)
{
    return streetIdToSegmentAndIntersectionId[street_id].second;
}

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id)
{
    return insertsectionIdToSegmentId[intersection_id];
}


void map_init(int map_name){
    
    current_city=map_name;
    
    init_street_segment_classification();
    
    Point2D location(getIntersectionPosition(0));
    
    initial_coords = t_bound_box(location.x-0.18,location.y-0.18,
                            location.x+0.18,location.y+0.18);
    
    set_visible_world(initial_coords);
    
    /* Global Varibles Init */
    NameButtonPressed=false;
    FindButtonPressed=false;
    input_box_1_enabled=false;
    input_box_2_enabled=false;
    show_incidents=false;
    MapSelectorButtonPressed=false;
    legendButtonPressed=false;
    intersectionFromSelected=false;
    intersectionToSelected=false;
    pathBoxSearchBoxSwitch=true;
}

void init_street_segment_classification(void) {
    const unsigned numberOfStreetSegments = getNumberOfStreetSegments();
    std::pair<std::string, std::string> theTag;
    OSMID id;
    const OSMWay* theOSMWay;
    unsigned numberOfTags;

    for (unsigned street_segment_id = 0; street_segment_id < numberOfStreetSegments; street_segment_id++) {
        id = getStreetSegmentInfo(street_segment_id).wayOSMID;
        theOSMWay = getWayByIndex(OSM_way[id]);
        numberOfTags = getTagCount(theOSMWay);

        for (unsigned i = 0; i < numberOfTags; i++) {
            theTag = getTagPair(theOSMWay, i);
            if (theTag.first.compare("highway") == 0) {
                if (theTag.second.compare("motorway") == 0) {
                    streetSegMotorWay.push_back(street_segment_id);
                } else if (theTag.second.compare("secondary") == 0 || theTag.second.compare("trunk") == 0 ||
                        theTag.second.compare("primary") == 0 || theTag.second.compare("tertiary") == 0) {
                    streetSegMajorRoads.push_back(street_segment_id);
                } else
                    streetSegMinorRoads.push_back(street_segment_id);
                break;
            }
        }
    }
    sort(streetSegMotorWay.begin(), streetSegMotorWay.end());
    sort(streetSegMajorRoads.begin(), streetSegMajorRoads.end());
    sort(streetSegMinorRoads.begin(), streetSegMinorRoads.end());
}
