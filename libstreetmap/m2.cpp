#include "m2.h"
#include "GUI_Autocomplete_Box.h"
#include "GUI_Search_Box.h"
#include "GUI_Path_Box.h"
#include "GA_Population.h"

// Initialize static variable in Predicate class
unsigned Predicate::streetID = 0;

void draw_map() {
    std::cout << "About to start graphics.\n";

    // Should be same as BGCOLOR macro
    t_color BGColor(233, 229, 220);
    init_graphics("Map", BGColor);

    // Show name of current city
    update_message_to_current_city();

    set_keypress_input(true);

    // Create all buttons (Window, MapSelector, Find, Show Names, Traffic, Clear, Legend)
    create_button("Window", "Map Selector", act_on_map_selector_button);    
    create_button("Map Selector","Show Names", act_on_name_button);
    create_button("Show Names", "Traffic", act_on_traffic_button);
    create_button("Traffic", "Clear", act_on_clear_button);        
    create_button("Clear", "Legend", act_on_legend_button);      
    create_button("Legend", "Help", act_on_help_button);

    set_visible_world(initial_coords);
    
    event_loop(act_on_mouse_button_press, NULL, act_on_key_press, draw_screen);

    close_graphics();
    std::cout << "Graphics closed down.\n";

}

void draw_screen(void) {
    
    set_draw_mode(DRAW_NORMAL); // Should set this if your program does any XOR drawing in callbacks.
    //clearscreen(); /* Should precede drawing for all drawscreens */

    setlinestyle(SOLID);
    setlinewidth(1);
    setcolor(BLACK);
    
    draw_features();

    draw_streets();
    
    draw_POIs();

    draw_one_way_arrows();

    // Draw different items on map depending on which buttons are pressed
    // and whether an item was highlighted
    
    if (show_incidents)
        draw_trafficIncidents();

    if (intersectionClicked)
        highlightIntersectionOnclick(intersectionPoint);

    if (showing_found_stuff)
        draw_found_stuff();
    
    if(NameButtonPressed)
        draw_all_names();

    if (MapSelectorButtonPressed)
        draw_map_selector_window();
    
    if (legendButtonPressed)
        draw_legend();
    
    if (helpButtonPressed)
        draw_help();
    
    if(foundPath.size()!=0 /*|| (intersectionFromSelected&&intersectionToSelected)*/){
        /*if(intersectionFromSelected&&intersectionToSelected){
            foundPath=find_path_between_intersections(intersectionFrom,intersectionTo);
            foundDirectives=get_path_directives(foundPath);
        }*/
        // Highlight from and to intersection
        highlight_intersection(Point2D(getIntersectionPosition(intersectionFrom)),1);
        if(!isinf(POITo)){
            
            intersectionTo=POITo;

        }
        highlight_intersection(Point2D(getIntersectionPosition(intersectionTo)),0);
        
        // Draw the path
        for(unsigned i = 0; i < foundPath.size(); i++){
            set_color(0x0000FF);
            draw_a_street_segment(foundPath[i]);
        }
        
        // Draw the directive window
        draw_directive(foundDirectives);
        
        // Update the travel time
        double travel_time = compute_path_travel_time(foundPath);
        std::string s = "Travel time: "+to_string(travel_time)+" mins.";
        update_message(s);
    }
    
    if(pathBoxSearchBoxSwitch){
        searchBar.enable_search_box();
        searchBar.draw_search_box();
    }
    
    if(!pathBoxSearchBoxSwitch){
        pathBar.enable_path_box();
        pathBar.draw_path_box();
    }

}

unsigned getStreetType(const std::vector<unsigned>& streetSegmentVector) {

    unsigned street_segment_id = streetSegmentVector.at(0);

    std::pair<std::string, std::string> theTag;
    OSMID id = getStreetSegmentInfo(street_segment_id).wayOSMID;
    const OSMWay* theOSMWay = getWayByIndex(OSM_way[id]);
    unsigned numberOfTags = getTagCount(theOSMWay);

    for (unsigned i = 0; i < numberOfTags; i++) {
        theTag = getTagPair(theOSMWay, i);
        if (theTag.first.compare("highway") == 0) {
            if (theTag.second.compare("motorway") == 0)
                return MOTORWAYINTERSECTION;

            else if (theTag.second.compare("secondary") == 0 || theTag.second.compare("trunk") == 0 ||
                    theTag.second.compare("primary") == 0 || theTag.second.compare("tertiary") == 0)
                return MAJORINTERSECTION;

            else
                return ALLINTERSECTIONS;

            break;
        }
    }
    //this should never happen
    return 0;
}

// Returns different wruting style for names depending on zoom
unsigned getNameZoomLevel(unsigned streetType) {

    enum {
        ALL_LEVEL = 5,
        MAJOR_LEVEL = 8,
        MOTOR_LEVEL = 12
    };

    if (streetType == ALLINTERSECTIONS)
        return ALL_LEVEL;
    if (streetType == MAJORINTERSECTION)
        return MAJOR_LEVEL;
    else //(streetType == MOTORWAYINTERSECTION)
        return MOTOR_LEVEL;
}

void draw_all_names() {
    // Get current zoom level
    unsigned screen_level = current_level();

    // initialize KD tree    
    Tree3D namesTree;

    std::vector<unsigned> streetIDVector;
    std::vector<unsigned> streetSegmentVector;
    std::string streetName;
    unsigned streetType;

    // Initialize streetSegmentVector to all streets
    std::map<unsigned, unsigned> mapOfStreetIDs = get_streets_ids_of_screen();
    unsigned numStreets = mapOfStreetIDs.size();
    
    // Copy contents of map into vector
    for (auto& it : mapOfStreetIDs)
        streetIDVector.push_back(it.second);

    // Draw names for all streets
    for (unsigned i = 0; i < numStreets; i++) {
        unsigned streetID = streetIDVector.at(i);

        streetSegmentVector = find_street_street_segments(streetID);
        streetName = getStreetName(streetID);
        streetType = getStreetType(streetSegmentVector);

        //streetName = shorten_street_name(streetName);

        if ((streetName != "<unknown>") && (getNameZoomLevel(streetType) > screen_level))
            drawNamesOnStreet(streetSegmentVector, streetName, namesTree, streetType);
    }
}

void drawNamesOnStreet(const std::vector<unsigned>& streetSegmentVector, const std::string& streetName, Tree3D& namesTree, const unsigned& type) {

    // Get number of street segments in street
    unsigned numStSegments = streetSegmentVector.size();

    // vector of bools to indicate whether each street segment has already been visited
    std::vector<bool> visited;
    visited.resize(numStSegments);

    // Initialize visited to false
    for (unsigned i = 0; i < numStSegments; i++)
        visited[i] = false;

    // Get arbitrary intersection in street to start DFS
    unsigned firstIntersection = getStreetSegmentInfo(streetSegmentVector[0]).from;
    std::vector<unsigned> branchVector;

    // Recursive depth first search through the street segments
    visit_street_segments(streetSegmentVector, streetName, visited, firstIntersection, branchVector, namesTree, type);
}

// Return whether given street segment was visited
bool isVisited(unsigned currSegment, const std::vector<unsigned>& streetSegmentVector, const std::vector<bool>& visitedVector) {

    for (unsigned index = 0; index < streetSegmentVector.size(); index++)
        if (streetSegmentVector.at(index) == currSegment)
            return visitedVector.at(index);
    return false;
}

// Return point at other end of street segment
unsigned otherNode(unsigned segmentID, unsigned intersectionID) {
    StreetSegmentInfo segInfo = getStreetSegmentInfo(segmentID);
    return otherNode(segInfo,intersectionID);
}

// Recursive depth-first search (DFS) function that visits all the street segments 
// in a street
// Calls the drawName function at each new street segment

void visit_street_segments(const std::vector<unsigned>& streetSegmentVector,
        const std::string& streetName, std::vector<bool>& visitedVector,
        unsigned& intersectionID, std::vector<unsigned>& branchNodes,
        Tree3D& namesTree, const unsigned& type) {

    // Find child edges by finding all adjacent street segments
    // that have not been visited and have the same street ID
    std::vector<unsigned> adjEdges = find_intersection_street_segments(intersectionID);
    std::vector<unsigned> childEdges;

    // If current intersection is already in branch nodes vector, remove it
    // Branches are nodes at which there are more than one unvisited child edges
    std::vector<unsigned> temp;
    for (unsigned i = 0; i < branchNodes.size(); i++)
        if (branchNodes[i] != intersectionID)
            temp.push_back(branchNodes[i]);
    branchNodes = temp;

    // Check all adjacent edges and copy those that are child edges into another vector
    unsigned numAdj = adjEdges.size();
    for (unsigned i = 0; i < numAdj; i++) 
    {

        // Get segment ID
        unsigned currSegment = adjEdges.at(i);

        // Determine if is was already visited
        bool visited = isVisited(currSegment, streetSegmentVector, visitedVector);

        // Get name of street segment
        StreetSegmentInfo stSegInfo = getStreetSegmentInfo(currSegment);
        std::string segmentName = getStreetName(stSegInfo.streetID);

        // Determine if child edge, if not remove
        if (!visited && segmentName == streetName)
            childEdges.push_back(currSegment);
    }

    // If child edges vector is larger than one, add to branch nodes
    if (childEdges.size() >= 1)
        branchNodes.push_back(intersectionID);

    // If child edge is found, go there
    // This means visiting a new segment for the first (and normally only) time
    if (!childEdges.empty()) {
        // Set segment as visited
        for (unsigned i = 0; i < visitedVector.size(); i++)
            if (streetSegmentVector.at(i) == childEdges.at(0))
                visitedVector.at(i) = true;

        // Perform action, in this case print street name if some criteria are satisfied
        drawName(childEdges.at(0), streetName, namesTree, type);

        // Recursive call to node at the end of new segment
        intersectionID = otherNode(childEdges.at(0), intersectionID);
        visit_street_segments(streetSegmentVector, streetName, visitedVector, intersectionID, branchNodes, namesTree, type);
    }

    // If reached leaf, go to branch node
    if (!branchNodes.empty()) {
        intersectionID = branchNodes.at(0);
        visit_street_segments(streetSegmentVector, streetName, visitedVector, intersectionID, branchNodes, namesTree, type);
    }

    // If reached leaf and branch vector is empty, return
    return;
}


// Given a specific street segment street name
// Takes care of aligning the name properly

void drawName(const unsigned& streetSegment, const std::string& streetName, Tree3D& namesTree, const unsigned& type) {

    // Define minimum spacing between street segments
    
    const float INTER_DIFF_NAME_SPACING(0.175);
    const float INTER_SAME_NAME_SPACING(0.250);
    const float INTER_ZOOM_NAME_SPACING(0.085);
    
    // Determine a high zoom threshold where street names can be closer so that 
    // Residential streets can all be displayed
    const unsigned HIGH_ZOOM(1);
    
    // Get current zoom level
    unsigned screen_level = current_level();
    
    // Get screen width
    t_bound_box current_screen = get_visible_world();
    const float screenWidth = current_screen.get_width();

    // Determine center of current street segment
    StreetSegmentInfo streetSeg = getStreetSegmentInfo(streetSegment);
    Point2D fromPoint = Point2D(getIntersectionPosition(streetSeg.from));
    Point2D toPoint = Point2D(getIntersectionPosition(streetSeg.to));

    // Convert to lat/lon
    Point2D newNameLocation((fromPoint + toPoint) / 2);
    newNameLocation = newNameLocation;
    LatLon newGeoLocation = newNameLocation.get_latlon();

    // Get a 3D point
    unsigned streetID = getStreetSegmentInfo(streetSegment).streetID;
    Point3D newNameNode(newGeoLocation, streetID);

    //Calls the KDTree::find_nearest() to find nearest other name
    std::pair <Tree3D::const_iterator, double> found(namesTree.find_nearest(newNameNode, 1e12));
    Point3D foundPoint(*(found.first));
    LatLon foundPointGeo = foundPoint.get_latlon();

    // Update reference street ID in predicate struct to current streetID
    Predicate::streetID = streetID;

    // Find distance to closest other name
    float closestDistance = find_distance_between_two_points(newGeoLocation, foundPointGeo);

    //Calls the KDTree::find_nearest() to find nearest other name but with the SAME street ID
    std::pair <Tree3D::const_iterator, double> foundSameName(namesTree.find_nearest_if(newNameNode, 1e12, Predicate()));
    Point3D foundPointSameName(*(foundSameName.first));
    LatLon foundPointGeoSameName = foundPointSameName.get_latlon();

    // Find distance to closest other name
    float closestDistanceSameID = find_distance_between_two_points(newGeoLocation, foundPointGeoSameName);

    // Determine minimum distances between names
    float minDistance;
    float minDistanceSameID;
    
    // Use different spacing for different zoom levels
    if(screen_level > HIGH_ZOOM)
    {
        minDistance       = INTER_DIFF_NAME_SPACING*screenWidth*EARTH_RADIUS_IN_METERS*DEG_TO_RAD;
        minDistanceSameID = INTER_SAME_NAME_SPACING*screenWidth*EARTH_RADIUS_IN_METERS*DEG_TO_RAD;
    }
    else  
    {
        minDistance       = INTER_ZOOM_NAME_SPACING*screenWidth*EARTH_RADIUS_IN_METERS*DEG_TO_RAD;
        minDistanceSameID = INTER_SAME_NAME_SPACING*screenWidth*EARTH_RADIUS_IN_METERS*DEG_TO_RAD;
    }
    
    // Checks if name is not too close to an other name or if the tree is empty
    // in which case the distance is nan
    if((closestDistance > minDistance || isnan(closestDistance)) 
            && (closestDistanceSameID > minDistanceSameID || isnan(closestDistanceSameID)))
    {
        std::string shortStreetName = shorten_street_name(streetName);
        writeNameToScreen(streetSegment, shortStreetName, type);
        namesTree.insert(newNameNode);
    }
}

void draw_a_street_segment(unsigned streetSegmentID) {

    StreetSegmentInfo streetSegment = getStreetSegmentInfo(streetSegmentID);

    Point2D fromPoint(getIntersectionPosition(streetSegment.from));
    Point2D toPoint(getIntersectionPosition(streetSegment.to));

    unsigned numberOfCurvedPoints = streetSegment.curvePointCount;
    /* No curve point */
    if (numberOfCurvedPoints == 0) {
        drawline(fromPoint.x, fromPoint.y, toPoint.x, toPoint.y);
        return;
    }
    /* With curve points */
    for (unsigned indexOfCurvePoint = 0; indexOfCurvePoint < numberOfCurvedPoints - 1; indexOfCurvePoint++) {
        Point2D point1(getStreetSegmentCurvePoint(streetSegmentID, indexOfCurvePoint));
        Point2D point2(getStreetSegmentCurvePoint(streetSegmentID, indexOfCurvePoint + 1));

        drawline(point1.x, point1.y, point2.x, point2.y);
    }
    /* Draw the edges */
    Point2D point1(getStreetSegmentCurvePoint(streetSegmentID, 0));
    Point2D point2(getStreetSegmentCurvePoint(streetSegmentID, numberOfCurvedPoints - 1));

    drawline(fromPoint.x, fromPoint.y,
            point1.x, point1.y);

    drawline(toPoint.x, toPoint.y,
            point2.x, point2.y);

}

// Return predefined levels of zoom depending on current displayed area
int current_level(void) {

    t_bound_box current_screen = get_visible_world();
    float area = abs(current_screen.get_height() * current_screen.get_width());

    if (area > 5)
        return 11;
    else if (area > 1)
        return 10;
    else if (area > 0.5)
        return 9;
    else if (area > 0.1)
        return 8;
    else if (area > 0.05)
        return 7;
    else if (area > 0.01)
        return 6;
    else if (area > 0.005)
        return 5;
    else if (area > 0.001)
        return 4;
    else if (area > 0.0005)
        return 3;
    else if (area > 0.0001)
        return 2;
    else if (area > 0.00005)
        return 1;
    else
        return 0;
}

void draw_streets(void) {

    OSMID id;
    const OSMWay* theOSMWay;

    int screen_level = current_level();

    if (screen_level <= 5) {
        unsigned numberOfMinorRoads = streetSegMinorRoads.size();
        for (unsigned i = 0; i < numberOfMinorRoads; i++) {

            StreetSegmentInfo streetseg = getStreetSegmentInfo(streetSegMinorRoads[i]);
            id = streetseg.wayOSMID;
            theOSMWay = getWayByIndex(OSM_way[id]);

            set_street_segment_drawing_parameter(theOSMWay, MINORROADS, screen_level);

            draw_a_street_segment(streetSegMinorRoads[i]);

        }
    }

    if (screen_level <= 10) {
        unsigned numberOfMajorRoads = streetSegMajorRoads.size();
        for (unsigned i = 0; i < numberOfMajorRoads; i++) {

            StreetSegmentInfo streetseg = getStreetSegmentInfo(streetSegMajorRoads[i]);
            id = streetseg.wayOSMID;
            theOSMWay = getWayByIndex(OSM_way[id]);
            
            set_street_segment_drawing_parameter(theOSMWay, MAJORROADS, screen_level);
            
            draw_a_street_segment(streetSegMajorRoads[i]);

        }
    }

    if (true) {
        unsigned numberOfMotorWay = streetSegMotorWay.size();
        for (unsigned i = 0; i < numberOfMotorWay; i++) {

            StreetSegmentInfo streetseg = getStreetSegmentInfo(streetSegMotorWay[i]);
            id = streetseg.wayOSMID;
            theOSMWay = getWayByIndex(OSM_way[id]);

            set_street_segment_drawing_parameter(theOSMWay, MOTORWAY, screen_level);

            draw_a_street_segment(streetSegMotorWay[i]);

        }
    }
}

void set_street_segment_drawing_parameter(const OSMWay* theOSMWay, int type, int screen_level) {

    std::pair<std::string, std::string> theTag;
    float linewidth = 0;

    /* set color */
    if (type == MOTORWAY) {
        set_color(MOTORWAY_ORANGE);
        linewidth = 3;
    } else if (type == MAJORROADS) {
        if (screen_level > 8)
            set_color(GREY1);
        else if (screen_level > 6)
            set_color(GREY2);
        else
            set_color(WHITE_HEX);

        linewidth = 2;
    } else if (type == MINORROADS) {
        if (screen_level > 4)
            set_color(GREY1);
        else if (screen_level > 2)
            set_color(GREY2);
        else
            set_color(WHITE_HEX);

        linewidth = 1;
    } else
        assert(("This line should never be executed!!", 1 == 0));

    int zoom_thick_increase_rate = 4;
    /* linewidth will increase once screen_level is less than 2*/
    if (screen_level < 4) {
        t_bound_box current_screen = get_visible_world();
        float area = abs(current_screen.get_height() * current_screen.get_width());

        if (linewidth < linewidth * zoom_thick_increase_rate * log(0.0001 / area))
            linewidth = linewidth * zoom_thick_increase_rate * log(0.0001 / area);
    }

    setlinewidth((int) linewidth);
}

void update_message_to_current_city() {
    switch (current_city) {
        case TORONTO:
            update_message("TORONTO");
            break;
        case TORONTO_CANADA:
            update_message("TORONTO CANADA");
            break;
        case NEWYORK:
            update_message("NEWYORK");
            break;
        case CAIRO_EGYPT:
            update_message("CAIRO EGYPT");
            break;
        case HAMILTON_CANADA:
            update_message("HAMILTON CANADA");
            break;
        case MOSCOW:
            update_message("MOSCOW");
            break;
        case SAINT_HELENA:
            update_message("SAINT HELENA");
            break;
        case LONDON_ENGLAND:
            update_message("LONDON ENGLAND");
            break;
        default:
            assert(("This line should never be executed!", 1 == 0));

    }
}

void get_trafficIncidents() {
    // Remove all previous traffic incidents from data structures
    trafficIncidentTree.clear();
    trafficIncidentsVector.clear();
    
    // Generate text file
    stringstream command;
    command << "libstreetmap/src/TrafficAPI "
            << MIN_LAT << " " << MIN_LON << " " << MAX_LAT << " " << MAX_LON;
    
    // Call Python script
    std::system(&command.str()[0]);

    // Parse text file
    ifstream filestream("TrafficIncidents.txt");
    std::string input;

    // Read first line of text file: number of accidents
    int numIncidents;
    std::getline(filestream, input);
    numIncidents = stoi(input);

    // Parse the file line by line
    // the constructor for TrafficIncident takes care of parsing each line
    trafficIncidentsVector.clear();
    for (int incident = 0; incident < numIncidents; incident++) {
        std::getline(filestream, input);
        
        TrafficIncident newIncident(input, incident);
        Point3D newIncident3D(newIncident.getPos(), newIncident.getID());
        
        trafficIncidentsVector.push_back(newIncident);
        trafficIncidentTree.insert(newIncident3D);
    }
}

// If zoom permits, draw incidents
void draw_trafficIncidents() {

    int screen_level = current_level();
    if (screen_level < INCIDENT_LEVEL)
        for (unsigned incident = 0; incident < trafficIncidentsVector.size(); incident++)
            trafficIncidentsVector[incident].draw();
}


void act_on_traffic_button(void (*drawscreen_ptr) (void)) {

    if (!show_incidents) {
        get_trafficIncidents();
        show_incidents = true;
        drawscreen_ptr();
    }
}

void draw_POIs() {
    const int NumberOfPOITypes = 6;

    for (int POIType = 0; POIType < NumberOfPOITypes; POIType++) {
        unsigned numberOfPOIForPOIType = POITypeToPOIId[POIType].size();

        for (unsigned POIId = 0; POIId < numberOfPOIForPOIType; POIId++) {
            plot_POI_icon(positionOfPOIs[POITypeToPOIId[POIType][POIId]], POIType);
        }
    }
}

void set_POI_colour(int POIType) {

    switch (POIType) {
        case(SHOP):
            set_color(SHOP_C);
            break;
        case(COLLEGE):
            set_color(UNIVERSITY_NC);
            break;
        case(HOSPITAL_N):
            set_color(HOSPITAL_NC);
            break;
        case(BANK):
            set_color(BANK_C);
            break;
        case(LIBRARY):
            set_color(LIBRARY_C);
            break;
        default:
            break;
    }
}

void plot_POI_icon(Point2D basePoint, int POIType) {
    int currentLevel = current_level();
    const int highZoom = 1;
    const int mediumZoom = 2;
    const int lowZoom = 4;

    //manages the icon size
    float radius = 0.00005;
    const int iconScale = 4;
    t_bound_box current_screen = get_visible_world();
    float area = abs(current_screen.get_height() * current_screen.get_width());

    if (radius > radius * iconScale * log(0.0001 / area)) {
        radius = radius * iconScale * log(0.0001 / area);
    }

    settextrotation(0);
    setfontsize(10);
    //determines whether or not the icons show up
    if ((currentLevel <= lowZoom && POIType == HOSPITAL_N) ||
            (currentLevel <= lowZoom && POIType == COLLEGE)) {

        draw_outline(basePoint);

        if (POIType == HOSPITAL_N) {
            set_POI_colour(POIType);
            draw_hospital_icon(basePoint);
        }
        if (POIType == COLLEGE) {
            set_POI_colour(POIType);
            draw_college_icon(basePoint);
        }
    }
    if ((currentLevel <= mediumZoom && POIType == LIBRARY)) {

        draw_outline(basePoint);
        set_POI_colour(POIType);
        draw_library_icon(basePoint);
    }
    if ((currentLevel <= highZoom && POIType == BANK) ||
            (currentLevel <= highZoom && POIType == SHOP)) {

        draw_outline(basePoint);

        if (POIType == BANK) {
            set_POI_colour(POIType);
            draw_bank_icon(basePoint);
        } else if (POIType == SHOP) {
            set_POI_colour(POIType);
            draw_shop_icon(basePoint);
        }
    }
}

void draw_outline(Point2D center) {
    t_bound_box current_screen = get_visible_world();

    set_color(Black);
    fillarc(center.x, center.y, current_screen.get_height() / 90, 0,
            360);
    set_color(White);
    fillarc(center.x, center.y, current_screen.get_height() / 110, 0,
            360);
}

void draw_college_icon(Point2D center) {
    drawtext(center.x, center.y, "U", 1, 1);
}

void draw_shop_icon(Point2D center) {
    drawtext(center.x, center.y, "R", 1, 1);
}

void draw_hospital_icon(Point2D center) {
    setfontsize(10);
    drawtext(center.x, center.y, "H", 1, 1);
}

void draw_bank_icon(Point2D center) {
    drawtext(center.x, center.y, "$", 1, 1);
}

void draw_library_icon(Point2D center) {
    drawtext(center.x, center.y, "L", 1, 1);
}

void draw_warning_icon(Point2D center){
    // Get current screen width
    t_bound_box current_screen = get_visible_world();
    const float screenWidth = current_screen.get_width();
    
    // Icon width as a percentage of the screen
    const float iconWidth = 0.0075*screenWidth;
    const int numPoints = 3;
    
    // Define 3 corners of an equilateral triangle and its center
    t_point bottomLeft (center.x, center.y);
    t_point bottomRight(center.x, center.y);
    t_point top        (center.x, center.y);
    
    bottomLeft.x  -= iconWidth;
    bottomRight.x += iconWidth;
    
    bottomLeft.y  -= std::sqrt(3)/2*iconWidth;
    bottomRight.y -= std::sqrt(3)/2*iconWidth;
    top.y         += std::sqrt(3)/2*iconWidth;
    
    t_point pointArr[] = {bottomLeft, bottomRight, top}; 
    set_color(INCIDENT_COLOR);
    fillpoly(pointArr, numPoints); 
    setcolor(YELLOW);
    drawtext(center.x+iconWidth/8, center.y-iconWidth/4, "!", 10, 10);
}

void draw_a_feature(unsigned featureID) {
    // Declare an array of t_points the same size as 
    // the number of points in the feature
    int pointCount = (int) getFeaturePointCount(featureID);
    t_point pointArr[pointCount];

    // Convert LatLon to cartesian coordinates and add to point array.
    for (int currPoint = 0; currPoint < pointCount; currPoint++) {
        Point2D pointCart(getFeaturePoint(featureID, currPoint));
        pointArr[currPoint].x = pointCart.x;
        pointArr[currPoint].y = pointCart.y;
    }

    fillpoly(pointArr, pointCount);
}

void draw_features() {
    // Current zoom level and bool that indicates whether zoom is high enough
    int screen_level = current_level();
    bool shouldDraw(true);

    // Cycle through feature categories
    unsigned numCategories = features.size();
    for (unsigned currCategory = 0; currCategory < numCategories; currCategory++) {

        // Check if currCategory is empty to avoid an out of range read
        if (!(features[currCategory].empty())) {
            // take care of building types and feature types separately
            FeatureType featType = getFeatureType(features[currCategory][0]);
            FeatureCategory featCategory(featType);
            if (currCategory > 9) {
                FeatureCategory featCategory((int) currCategory);
                set_color(featCategory.draw_color);
            } else {
                set_color(featCategory.draw_color);
            }

            if ((featType == Building || currCategory > 9) && screen_level > BUILDING_LEVEL)
                shouldDraw = false;
        }

        // If zoom level is high enough and category is not empty , draw
        if (shouldDraw) {
            // Draw all features in a category
            unsigned numFeaturesInCat = features[currCategory].size();
            for (unsigned featureIndex = 0; featureIndex < numFeaturesInCat; featureIndex++)
                draw_a_feature(features[currCategory][featureIndex]);
        }
        shouldDraw = true;
    }
}


void act_on_mouse_button_press(float x, float y, t_event_buttonPressed event) {
    
    if(pathBoxSearchBoxSwitch && searchBar.clickedInside(x,y))
        searchBar.searchbox_onclick_handler(x, y);
    else if(!pathBoxSearchBoxSwitch && pathBar.clickInside(x,y))
        pathBar.pathbox_on_click_handler(x, y);
    else if (MapSelectorButtonPressed)
        mouse_press_map_selector_window_handler(x, y, event);
    else if(legendButtonPressed)
        mouse_press_legend_window_handler(x, y, event);
    else if(helpButtonPressed)
        mouse_press_help_window_handler(x, y, event);
    else if(!foundPath.empty())
        mouse_press_directive_window_handler(x,y,event);
    else
        mouse_press_map_handler(x,y,event);
    draw_screen();
}

std::vector<unsigned> find_intersections_within_range(LatLon centerLatLon, float range) {
    std::vector<unsigned> result;
    std::vector<Point3D> points;

    // to find the "KD-tree" range, find a second LatLon point range away from the center
    // to do this, keep the same latitude and solve for the longitude required to have
    // a distance of range away from the center
    // see comments at the top of Point3D.h to see why we need to do this
    LatLon maxDistLatLon;
    maxDistLatLon.lat = centerLatLon.lat;
    maxDistLatLon.lon = centerLatLon.lon - range /
            (cos(DEG_TO_RAD * maxDistLatLon.lat) * EARTH_RADIUS_IN_METERS * DEG_TO_RAD);

    // Convert to Point3D
    Point3D center(centerLatLon, 0);
    Point3D maxDist(maxDistLatLon, 0);
    Point2D A(maxDistLatLon);

    // Find direct squared distance between the two points
    // this is the distance used in nearest neighbor search
    // see comments at top of Point3D.h
    float kdRange(0);
    for (int i = 0; i < 3; i++)
        kdRange += (center[i] - maxDist[i])*(center[i] - maxDist[i]);
    kdRange = sqrt(kdRange);

    // Call the kdlibtree++ function
    points.resize(500000);

    intersectionsTree.find_within_range(center, kdRange, points.begin());

    unsigned numberOfIntersections = getNumberOfIntersections();
    // Get the ID's of all the points
    for (unsigned i = 0; i < 500000; i++)
        if (points[i].getKey() >= numberOfIntersections)
            break;
        else
            result.push_back(points[i].getKey());

    return result;
}

void act_on_name_button(void (*drawscreen_ptr) (void)) {

    if (NameButtonPressed)
        NameButtonPressed = false;
    else
        NameButtonPressed = true;

    drawscreen_ptr();
}


void writeNameToScreen(unsigned street_segment_id, std::string street_name, int type) {

    StreetSegmentInfo streetSeg = getStreetSegmentInfo(street_segment_id);

    Point2D fromPoint = Point2D(getIntersectionPosition(streetSeg.from));
    Point2D toPoint = Point2D(getIntersectionPosition(streetSeg.to));

    int angle = get_angle(fromPoint, toPoint);

    Point2D text_center((fromPoint + toPoint) / 2);

    //Rotate the names which are upside down
    if(angle<=-90)
        angle+=180;
    
    settextrotation(angle);
    switch (type) {
        case ALLINTERSECTIONS:
            setfontsize(8);
            break;
        case MAJORINTERSECTION:
            setfontsize(9);
            break;
        case MOTORWAYINTERSECTION:
            setfontsize(9);
            break;
        default:
            assert(("This line should never be executed!", 1 == 0));
    }

    setcolor(BLACK);

    drawtext(text_center.x, text_center.y, street_name, 1, 1);
    settextrotation(0);
}

// Get angle of line with relation to screen coordinates
int get_angle(Point2D point1, Point2D point2) {
    float angle;
    angle = atan((point2.y - point1.y) / (point2.x - point1.x)) / DEG_TO_RAD;

    if(point2.x<point1.x)
        angle-=180;
    return (int) angle;
}

std::map<unsigned, unsigned> get_streets_ids_of_screen() {

    std::map<unsigned, unsigned> result;
    t_bound_box current_view = get_visible_world();
    Point2D center(current_view.get_center().x, current_view.get_center().y);

    float region_radius = current_view.get_width()*58000;

    std::vector<unsigned> all_intersections = find_intersections_within_range(center.get_latlon(), region_radius);
    auto all_intersections_size = all_intersections.size();

    /* go through every intersection found in the block */
    for (unsigned j = 0; j < all_intersections_size; j++) {

        auto intersectionStreetSegmentCount = getIntersectionStreetSegmentCount(all_intersections[j]);

        /* go through every street segment connected to that intersection */
        for (unsigned k = 0; k < intersectionStreetSegmentCount; k++) {

            StreetSegmentInfo s = getStreetSegmentInfo(getIntersectionStreetSegment(all_intersections[j], k));
            std::string streetName = getStreetName(s.streetID);

            std::map<unsigned, unsigned>::iterator it;
            it = result.find(s.streetID);

            /* if the street name is not there, add the name */
            if (it == result.end()) {
                result.emplace(s.streetID, s.streetID);
            }
        }
    }
    return result;
}

void delay(long milliseconds) {
    // if you would like to use this function in your
    // own code you will need to #include <chrono> and
    // <thread>
    std::chrono::milliseconds duration(milliseconds);
    std::this_thread::sleep_for(duration);
}

// Input text
void act_on_key_press(char c, int keysym) {

    assert(("input_box_1_enabled & input_box_2_enabled should always be false"
            , !(input_box_1_enabled && input_box_2_enabled)));
    
    //handles shifting, honestly I don't even know how this works but it does
    if (keysym == XK_Shift_L || keysym == XK_Shift_R) {
        return;
    }

    if(pathBoxSearchBoxSwitch){
        searchBar.user_input(c, keysym);
    }
    else if(!pathBoxSearchBoxSwitch){
        pathBar.user_input(c, keysym);
    }
}

// Use short versions of street, avenue, etc.
std::string shorten_street_name(std::string name) {
    std::string::size_type a;
    if ((a = name.find("Street")) != string::npos)
        return name.replace(a, 6, "St");
    else if ((a = name.find("Avenue")) != string::npos)
        return name.replace(a, 6, "Ave");
    else if ((a = name.find("Road")) != string::npos)
        return name.replace(a, 4, "Rd");
    else if ((a = name.find("Drive")) != string::npos)
        return name.replace(a, 5, "Dr");
    else if ((a = name.find("Boulevard")) != string::npos)
        return name.replace(a, 9, "Blvd");
    else
        return name;
}

// Determine if point is inside box
bool inside_box(float x, float y, t_bound_box box) {

    if (x <= box.bottom_left().x + box.get_width() &&
            x >= box.bottom_left().x &&
            y <= box.bottom_left().y + box.get_height() &&
            y >= box.bottom_left().y)
        return true;
    else
        return false;

}

void mouse_press_map_handler(float x, float y, t_event_buttonPressed event) {
    
    // Get screen width
    t_bound_box current_screen = get_visible_world();
    const float screenWidth = current_screen.get_width()*EARTH_RADIUS_IN_METERS*DEG_TO_RAD;

    int zoom_level = current_level();
    
    // Get current location
    float lon = x / cos(AVG_LAT * DEG_TO_RAD);
    float lat = y;

    // Find closest intersection
    unsigned closest_intersection_id = find_closest_intersection(LatLon(lat, lon));
    LatLon closest_intersection_latlon = getIntersectionPosition(closest_intersection_id);

    // Find closest POI
    unsigned closest_POI_id = find_closest_point_of_interest(LatLon(lat, lon));
    LatLon closest_POI_latlon = getPointOfInterestPosition(closest_POI_id);
    
    // Find closest traffic incident
    unsigned closest_traffic_incident_id = find_closest_traffic_incident(LatLon(lat, lon));    
    LatLon closest_traffic_incident_latlon;
    if(!trafficIncidentsVector.empty())
        closest_traffic_incident_latlon = trafficIncidentsVector.at(closest_traffic_incident_id).getPos();

    // calculate distances with closest elements
    // If no traffic incidents are displayed, assign a very large distance
    float dis_intersection = find_distance_between_two_points(closest_intersection_latlon, LatLon(lat, lon));
    float dis_poi = find_distance_between_two_points(closest_POI_latlon, LatLon(lat, lon));    
    
    float dis_traffic;    
    if(!trafficIncidentsVector.empty())
        dis_traffic = find_distance_between_two_points(closest_traffic_incident_latlon, LatLon(lat, lon));
    else
        dis_traffic = EARTH_RADIUS_IN_METERS;

    // Find closest element
    float minimum = std::min(dis_intersection, dis_poi);
    minimum = std::min(minimum, dis_traffic);
    
    // Check if traffic icon was clicked
    if(minimum == dis_traffic && minimum < screenWidth*0.01 && zoom_level<4)
        //update the message at the bottom
        update_message(trafficIncidentsVector.at(closest_traffic_incident_id).getMessage());
    // Check if POI was clicked
    else if (minimum == dis_poi && minimum < 10)
    {
        //update the message at the bottom
        update_message("POI: " + getPointOfInterestName(closest_POI_id));
        
        LatLon closestPOILocation = getPointOfInterestPosition(closest_POI_id);
        unsigned closest_intersection_id = find_closest_intersection(closestPOILocation);
        
        if(pathBoxSearchBoxSwitch){
            searchBar.set_input(getIntersectionName(closest_intersection_id));
            intersectionFrom=closest_intersection_id;
        }
        else
            if(pathBar.getToBox()->isPressed()){
                intersectionTo=closest_intersection_id;
                pathBar.getToBox()->set_input(getPointOfInterestName(closest_POI_id));
            }
            else{
                pathBar.getFromBox()->set_input(getPointOfInterestName(closest_POI_id));
                intersectionFrom=closest_intersection_id;
            }
    }
    
    // If nothing was clicked, restore default message
    // Check if intersection was clicked
    else if (minimum == dis_intersection || zoom_level>3) 
    {
        if(pathBoxSearchBoxSwitch)
            searchBar.set_input(getIntersectionName(closest_intersection_id));
        else
            if(pathBar.getToBox()->isPressed())
                pathBar.getToBox()->set_input(getIntersectionName(closest_intersection_id));
            else
                pathBar.getFromBox()->set_input(getIntersectionName(closest_intersection_id));
//        if(intersectionFromSelected&&intersectionToSelected){
//            intersectionFrom=closest_intersection_id;
//            intersectionToSelected=false;
//            foundPath.clear();
//            foundDirectives.clear();
//        }
//        else if(!intersectionFromSelected){
//            intersectionFromSelected=true;
//            intersectionFrom=closest_intersection_id;
//            foundPath.clear();
//            foundDirectives.clear();
//        }
//        else if(!intersectionToSelected){
//            intersectionToSelected=true;
//            intersectionTo=closest_intersection_id;
//        }
        
        // update the global variables
        Point2D temp(closest_intersection_latlon);
        intersectionClicked = true;
        intersectionPoint = temp.get_latlon();
        
        //update the message at the bottom
        update_message("Intersection: " +to_string(closest_intersection_id)+" "+ getIntersectionName(closest_intersection_id));
        draw_screen();        
    } 
    else 
    {
        update_message_to_current_city();
        if (intersectionClicked) 
        {
            intersectionClicked = false;
            draw_screen();
        }
    }
}

// Highlight intersection as a result from the find button
void highlight_intersection(Point2D intersection, int color) {

    t_bound_box current_view = get_visible_world();

    t_point point1(intersection.x - current_view.get_height() / 70, intersection.y - current_view.get_height() / 80 + current_view.get_height() / 25);
    t_point point2(intersection.x + current_view.get_height() / 70, intersection.y - current_view.get_height() / 80 + current_view.get_height() / 25);
    t_point point3(intersection.x, intersection.y);

    t_point pointarr[] = {point1, point2, point3};


    if(color==0)
        setcolor(GREEN);
    else if(color==1)
        setcolor(RED);
    else if(color==2)
        setcolor(BLUE);
    else if(color==3)
        setcolor(CYAN);
    else if(color==4)
        setcolor(ORANGE);
    
    fillpoly(pointarr,3);

    fillarc(intersection.x, intersection.y + current_view.get_height() / 25, current_view.get_height() / 50, 0, 360);

    setcolor(BLACK);
    fillarc(intersection.x, intersection.y + current_view.get_height() / 25, current_view.get_height() / 100, 0, 360);
}
void highlight_intersection(unsigned intersectionID, int color){
    highlight_intersection(Point2D(getIntersectionPosition(intersectionID)),color);
}
// Put a circle above an intersection if it was highlighted
void highlightIntersectionOnclick(LatLon intersection) {
    Point2D intersectionPoint(intersection);

    t_bound_box current_screen = get_visible_world();

    setcolor(DARKGREY);
    fillarc(intersectionPoint.x, intersectionPoint.y, current_screen.get_width() / 100, 0, 360);
}

void draw_found_stuff() {
    for (unsigned i = 0; i < found_stuff.size(); i++)
        highlight_intersection(found_stuff[i],1);
}

void act_on_map_selector_button(void (*drawscreen_ptr) (void)) {

    clearAllWindow();
    
    map_selected = 0;
    MapSelectorButtonPressed = true;

    draw_screen();
}

void act_on_clear_button(void (*drawscreen_ptr) (void)) {

    clearAllWindow();
    
    draw_screen();
}

void clearALL(){
    
    clearAllWindow();
    
    NameButtonPressed = false; //Used to indicate if the name button is pressed
    FindButtonPressed = false; //Used to indicate if the search button is pressed
    input_box_1_enabled = false;
    input_box_2_enabled = false;
    showing_results_of_find_button = false;
    showing_found_stuff = false;
    show_incidents = false;

    searchBar.clear_intersection_icons();

    input1.clear();
    input2.clear();
    found_stuff.clear();
}

void clearAllWindow(){
    
    helpButtonPressed =false;
    window_size = t_bound_box(0,0,0,0);
    foundPath.clear();
    foundDirectives.clear();
    searchBar.clear_intersection_icons();
    MapSelectorButtonPressed = false;
    intersectionClicked = false;
    legendButtonPressed = false;
    intersectionFromSelected=false;
    intersectionToSelected=false;
}
void draw_one_way_arrows(){
    
    t_bound_box current_screen = get_visible_world();
    float area = abs(current_screen.get_height() * current_screen.get_width());
    
    if(area>0.0001)
        return;
    
    std::map<unsigned, unsigned> one_way_street_segment_id = get_one_way_street_segemnts_id_from_screen();
    
    for(auto i = one_way_street_segment_id.begin();i!=one_way_street_segment_id.end();i++){
        setcolor(DARKGREY);
        Point2D fromPoint(getIntersectionPosition(getStreetSegmentInfo(i->first).from));
        Point2D toPoint(getIntersectionPosition(getStreetSegmentInfo(i->first).to));
        Point2D drawPoint((fromPoint+toPoint)/2);
        draw_an_arrow(drawPoint,get_angle(fromPoint,toPoint));
    }
    
}

std::map<unsigned, unsigned> get_one_way_street_segemnts_id_from_screen(){
    
    std::map<unsigned, unsigned> result;
    t_bound_box current_view = get_visible_world();
    Point2D center(current_view.get_center().x, current_view.get_center().y);

    float region_radius = current_view.get_width()*58000;

    std::vector<unsigned> all_intersections = find_intersections_within_range(center.get_latlon(), region_radius);
    auto all_intersections_size = all_intersections.size();

    /* go through every intersection found in the block */
    for (unsigned j = 0; j < all_intersections_size; j++) {

        auto intersectionStreetSegmentCount = getIntersectionStreetSegmentCount(all_intersections[j]);

        /* go through every street segment connected to that intersection */
        for (unsigned k = 0; k < intersectionStreetSegmentCount; k++) {
            unsigned street_segment_id = getIntersectionStreetSegment(all_intersections[j], k);
            StreetSegmentInfo s = getStreetSegmentInfo(street_segment_id);
            double distance = find_distance_between_two_points(getIntersectionPosition(s.from),
                    getIntersectionPosition(s.to));
            
            if(s.oneWay && (s.curvePointCount<3) && distance > 40){
                std::map<unsigned, unsigned>::iterator it;
                it = result.find(street_segment_id);

                /* if the street name is not there, add the name */
                if (it == result.end()) {
                    result.emplace(street_segment_id, street_segment_id);
                }
            }
        }
    }
    return result;
}

void draw_an_arrow(Point2D origin,int degree){
    t_bound_box current_view = get_visible_world();
    float baselength = current_view.get_height()/350;
    
    float cosine = cos(degree*DEG_TO_RAD);
    float sine = sin(degree*DEG_TO_RAD);
    
    t_point pointarr[7];
    
    pointarr[0].x = origin.x-baselength*sine;
    pointarr[0].y = origin.y+baselength*cosine;
    
    pointarr[1].x = pointarr[0].x+4*baselength*cosine;
    pointarr[1].y = pointarr[0].y+4*baselength*sine;
    
    pointarr[2].x = pointarr[1].x-baselength*sine;
    pointarr[2].y = pointarr[1].y+baselength*cosine;
    
    pointarr[3].x = origin.x+7*baselength*cosine;
    pointarr[3].y = origin.y+7*baselength*sine;
    
    pointarr[6].x = origin.x+baselength*sine;
    pointarr[6].y = origin.y-baselength*cosine;
    
    pointarr[5].x = pointarr[6].x+4*baselength*cosine;
    pointarr[5].y = pointarr[6].y+4*baselength*sine;
    
    pointarr[4].x = pointarr[5].x+baselength*sine;
    pointarr[4].y = pointarr[5].y-baselength*cosine;
    
    setcolor(LIGHTGREY);
    fillpoly(pointarr,7);
    
}

void drawpoly(t_point *points, int npoints){
    if(npoints<3)
        return;
    
    for(int i = 1; i < npoints; i++)
        drawline(points[i-1],points[i]);
    
    drawline(points[npoints-1],points[0]);
}

void act_on_legend_button(void (*drawscreen_ptr) (void)){
    
    clearAllWindow();
    
    legendButtonPressed = true;
    
    draw_screen();
}

void act_on_help_button(void (*drawscreen_ptr) (void)){
    
    clearAllWindow();
    
    helpButtonPressed = true;
    
    draw_screen();
}

bool path_is_legal(unsigned start_intersection, unsigned end_intersection, const std::vector<unsigned>& path) {
    //'start_intersection' is the intersection id of the starting intersection
    //'end_intersection' is the intersection id of the ending intersection
    //'path' is a vector street segment id's

    if (path.size() < 1) {
        return false; //If it is a valid path it must contain at-least one segment

    } else {
        //General case
        //To verify the path by walking along each segment checking:
        //  * That we are going a legal direction (i.e. not against one-ways)
        //  * That each segment is connected to the previous intersection
        //  * That the final segment is connected to the end_intersection
        //We start our intersection at the start_intersection
        assert(path.size() >= 1);

        unsigned curr_intersection = start_intersection;
        for (unsigned i = 0; i < path.size(); i++) {
            cout<<"i: "<<i<<endl;
            
            StreetSegmentInfo seg_info = getStreetSegmentInfo(path[i]);


            //Are we moving forward or back along the segment?
            bool seg_traverse_forward;
            if (seg_info.to == curr_intersection) {
                //Moving backwards
                seg_traverse_forward = false;

            } else if (seg_info.from == curr_intersection) {
                //Moving forwards
                seg_traverse_forward = true;

            } else {
                //This segment isn't connected to the current intersection
                return false;
            }

            //Are we going the wrong way along the segment?
            if (!seg_traverse_forward && seg_info.oneWay) {
                return false;
            }

            //Advance to the next intersection
            curr_intersection = (seg_traverse_forward) ? seg_info.to : seg_info.from;

        }

        //We should be at the end intersection
        if (curr_intersection != end_intersection) {
            return false;
        }
    }

    //Everything validated
    return true;
}

void draw_directive(std::vector<string> directives){
    int window_length = directives.size();
    if(window_length>9)
        window_length=9;
        
    t_bound_box current_view = get_visible_world();
    
    window_size = t_bound_box(current_view.bottom_left().x+41*current_view.get_width()/60,
                current_view.bottom_left().y+current_view.get_height()*(9-window_length)/10,
                current_view.bottom_left().x+current_view.get_width(),
                current_view.bottom_left().y+current_view.get_height());
    
    close_box_size = t_bound_box(current_view.bottom_left().x+current_view.get_width()*39/40,
                current_view.bottom_left().y+current_view.get_height()*39/40,
                current_view.bottom_left().x+current_view.get_width(),
                current_view.bottom_left().y+current_view.get_height());
    
    setcolor(WHITE);
    fillrect(window_size);
    
    setcolor(RED);
    fillrect(close_box_size);
    
    setlinewidth(2);
    setcolor(WHITE);
    drawline(close_box_size.bottom_left().x+close_box_size.get_width()*1/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*1/4,
            close_box_size.bottom_left().x+close_box_size.get_width()*3/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*3/4);
    
    drawline(close_box_size.bottom_left().x+close_box_size.get_width()*1/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*3/4,
            close_box_size.bottom_left().x+close_box_size.get_width()*3/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*1/4);

    setcolor(BLUE);
    setfontsize(10); 
    drawtext(window_size.get_xcenter(),current_view.get_ycenter()
                +9*current_view.get_height()/20,
                "Directives",window_size.get_width(),1);
    
    if(directives.size()>10){
        if((unsigned)directiveVectorOffset>directives.size()-10)
            directiveVectorOffset=directives.size()-10;
    }
    else
        directiveVectorOffset=0;
            
    
    settextrotation(0);
    setfontsize(8);   
    for(int i=0;i<window_length;i++){
        setcolor(LIGHTGREY);
        setlinewidth(2);
        drawrect(window_size.get_xcenter()-window_size.get_width()/2,
                current_view.get_ycenter()+7*current_view.get_height()/20-i*current_view.get_height()/10-current_view.get_height()/20,
                window_size.get_xcenter()+window_size.get_width()/2,
                current_view.get_ycenter()+7*current_view.get_height()/20-i*current_view.get_height()/10+current_view.get_height()/20);
        setcolor(BLACK);
        //if there are too many letters
     
        drawtext(window_size.get_xcenter(),
                current_view.get_ycenter()+7*current_view.get_height()/20-i*current_view.get_height()/10,
                directives[i+directiveVectorOffset],window_size.get_width(),
                1);
    }
}

void mouse_press_directive_window_handler(float x, float y, t_event_buttonPressed event){
    if(inside_box(x,y,close_box_size)){
        foundPath.clear();
        foundDirectives.clear();
        intersectionFromSelected=intersectionToSelected=false;
        draw_screen();
    }
}

void draw_help(){
  
    t_bound_box current_view = get_visible_world();
    
    //create bounds for outer window
    window_size = t_bound_box(current_view.bottom_left().x+current_view.get_width()/6,
                current_view.bottom_left().y+current_view.get_height()/8,
                current_view.bottom_left().x+current_view.get_width()*5/6,
                current_view.bottom_left().y+current_view.get_height()*7/8);
    
    //create bounds for draw closed box
    close_box_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*18.5/20,
                window_size.bottom_left().y+window_size.get_height()*19.25/20,
                window_size.bottom_left().x+window_size.get_width(),
                window_size.bottom_left().y+window_size.get_height());
    
    //draw outer window
    setcolor(WHITE);
    fillrect(window_size);
    
    setcolor(DARKGREY);
    setlinewidth(1);
    drawrect(window_size);
    
    //draw close box
    setcolor(RED);
    fillrect(close_box_size);
    
    //draw 'x' for close box
    setlinewidth(2);
    setcolor(WHITE);
    settextrotation(0);
    
    drawline(close_box_size.bottom_left().x+close_box_size.get_width()*1/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*1/4,
            close_box_size.bottom_left().x+close_box_size.get_width()*3/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*3/4);
    
    drawline(close_box_size.bottom_left().x+close_box_size.get_width()*1/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*3/4,
            close_box_size.bottom_left().x+close_box_size.get_width()*3/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*1/4);
    
    double topMargin = 0.1*window_size.get_height();
    double leftMargin = 0.05*window_size.get_width();
    double tab = 0.1*window_size.get_width();   
    double lineHeight = 0.035*window_size.get_width();   
    
    double searchHeight     = 0.23*window_size.get_height();
    double pathHeight       = 0.4*window_size.get_height();
    
    //draw title
    setfontsize(20);
    set_color(Black);
    drawtext(window_size.bottom_left().x+window_size.get_width()/2, 
             window_size.bottom_left().y+window_size.get_height()/1.05, "Help", 1, 1);
    
    // Locations of different section titles
    Point2D searchTitle(window_size.bottom_left().x+leftMargin,
            window_size.bottom_left().y+window_size.get_height()-topMargin);    
    Point2D pathTitle(window_size.bottom_left().x+leftMargin, searchTitle.y-searchHeight);    
    Point2D buttonsTitle(window_size.bottom_left().x+leftMargin,pathTitle.y-pathHeight);
        
    setfontsize(12);
    float searchTitleWidth  = left_align_draw_text(searchTitle.x,searchTitle.y,"Search Bar:  ", 1, 1);
    float pathTitleWidth    = left_align_draw_text(pathTitle.x,pathTitle.y,"Path Finding:  ", 1, 1);
    float buttonsTitleWidth = left_align_draw_text(buttonsTitle.x,buttonsTitle.y,"Buttons:  ", 1, 1);
    
    // Locations of different section bodies
    Point2D searchBody(searchTitle.x+searchTitleWidth,searchTitle.y);    
    Point2D pathBody(pathTitle.x+pathTitleWidth, pathTitle.y);    
    Point2D buttonsBody(buttonsTitle.x+buttonsTitleWidth,buttonsTitle.y);
        
    setfontsize(9);
    // Search Bar section
    left_align_draw_text(searchBody.x,searchBody.y," The search bar appears by default in the upper left corner of the window. To use:", 1, 1);
    
    left_align_draw_text(searchBody.x-tab,searchBody.y-1*lineHeight,"1) Type in an intersection", 1, 1);
    left_align_draw_text(searchBody.x-tab,searchBody.y-2*lineHeight,"2) Click on suggested searches to complete your queries", 1, 1);
    left_align_draw_text(searchBody.x-tab,searchBody.y-3*lineHeight,"3) Press the magnifying glass button to launch your search", 1, 1);
    left_align_draw_text(searchBody.x-tab,searchBody.y-4*lineHeight,"Hint: Clicking on the map will automatically fill the search bars", 1, 1);
    
    
    // Path finding section
    left_align_draw_text(pathBody.x,pathBody.y-0*lineHeight," To open the path finding search bars, click on the arrow icon in the search bar. ", 1, 1);
    left_align_draw_text(pathTitle.x,pathBody.y-1*lineHeight," To perform a search:", 1, 1);
    
    left_align_draw_text(searchBody.x-tab,pathBody.y-2*lineHeight,"1) Type in a point of interest or intersection in the bottom search bar", 1, 1);
    left_align_draw_text(searchBody.x-tab,pathBody.y-3*lineHeight,"2) Type in an intersection as your starting point in the top search bar", 1, 1);
    left_align_draw_text(searchBody.x-tab,pathBody.y-4*lineHeight,"3) Press the go button to launch your search", 1, 1);
    left_align_draw_text(searchBody.x-tab,pathBody.y-5*lineHeight,"Hint: Pressing tab will auto-fill the search bar with the first query", 1, 1);

    left_align_draw_text(pathTitle.x,pathBody.y-6*lineHeight," To exit path finding, click on the red x in the top search bar.", 1, 1);
    left_align_draw_text(pathTitle.x,pathBody.y-7*lineHeight," The resulting itinerary in displayed in a scrollable window on the right side of the screen.", 1, 1);
    
    left_align_draw_text(buttonsBody.x,buttonsBody.y," Here are all the buttons we have ", 1, 1);
    
    left_align_draw_text(searchBody.x-tab,buttonsBody.y-1*lineHeight,"Clear: Clears current paths, markers and selected intersections on the map", 1, 1);
    left_align_draw_text(searchBody.x-tab,buttonsBody.y-2*lineHeight,"Show Names: Toggles display of street names on and off", 1, 1);
    left_align_draw_text(searchBody.x-tab,buttonsBody.y-3*lineHeight,"Traffic: Toggles display of real-time traffic incidents on the map", 1, 1);
    left_align_draw_text(searchBody.x-tab,buttonsBody.y-4*lineHeight,"Legend: Opens a legend window that shows all map icons", 1, 1);
    left_align_draw_text(searchBody.x-tab,buttonsBody.y-5*lineHeight,"Map Selector: Opens a window in which the user can select what map to open next", 1, 1);
}

void mouse_press_help_window_handler(float x, float y, t_event_buttonPressed event){
    if(inside_box(x,y,close_box_size)){
        helpButtonPressed = false;
        draw_screen();
    }
}

