#include <m3.h>

std::vector<unsigned> find_path_between_intersections(unsigned 
                   intersect_id_start, unsigned intersect_id_end){
    
    GraphVertex* destination = intersectionGraph.shortestPath(intersect_id_start,intersect_id_end);
    std::vector<unsigned> path;
    intersectionGraph.getPath(path,intersect_id_start,destination);
    
    return path;
}

void find_path_between_intersections(unsigned intersect_id_start, 
        unsigned intersect_id_end, std::vector<unsigned>& path){
    
    GraphVertex* destination = intersectionGraph.shortestPath(intersect_id_start,intersect_id_end);
    intersectionGraph.getPath(path,intersect_id_start,destination); 
}
void find_path_between_intersections_append(unsigned intersect_id_start, 
        unsigned intersect_id_end, std::vector<unsigned>& path){
    
    GraphVertex* destination = intersectionGraph.shortestPath(intersect_id_start,intersect_id_end);
    intersectionGraph.getPath_append(path,intersect_id_start,destination);
    
}
double compute_path_travel_time(const std::vector<unsigned>& path){
    double result=0;
    for(unsigned i =0;i<path.size();i++){
        result+=find_street_segment_travel_time(path[i]);
        if(i>0 && (getStreetSegmentInfo(path[i]).streetID!=getStreetSegmentInfo(path[i-1]).streetID))
            result+=0.25;
    }
    return result;
}
double compute_path_travel_time(const std::list<unsigned>& path){
    double result=0;
//    for(unsigned i =0;i<path.size();i++){
//        result+=find_street_segment_travel_time(path[i]);
//        if(i>0 && (getStreetSegmentInfo(path[i]).streetID!=getStreetSegmentInfo(path[i-1]).streetID))
//            result+=0.25;
//    }
    
    for (std::list<unsigned>::const_iterator iterator = path.begin(), end = path.end(); iterator != end; ++iterator) {
        
        result+=find_street_segment_travel_time(*iterator);
        std::list<unsigned>::const_iterator prevIterator = iterator;
        prevIterator--;
        if(iterator != path.begin() && (getStreetSegmentInfo(*iterator).streetID!=getStreetSegmentInfo(*prevIterator).streetID))
            result+=0.25;
    }
    
    return result;
}

std::vector<unsigned> find_path_to_point_of_interest (unsigned 
                   intersect_id_start, std::string point_of_interest_name){
    
    // Return value if unsuccessful
    std::vector<unsigned> empty;
    
    // Get vector of all the matching POI ID's
    std::vector<unsigned> POI_IDs;    
    auto range = nameToPoiID.equal_range(point_of_interest_name);
    for (auto it = range.first; it != range.second; ++it) {
        POI_IDs.push_back(it->second);
    }  
    
    for(unsigned i=0; i<POI_IDs.size(); i++){
        POI_IDs[i] = find_closest_intersection(getPointOfInterestPosition(POI_IDs[i]));
    }
    
    // If no matching POI's, return empty vector
    if(POI_IDs.empty())
        return empty;
    
    // Get all the destination pointers
    GraphVertex* result = intersectionGraph.shortestPath(intersect_id_start, POI_IDs);
    
    intersectionGraph.getPath(empty,intersect_id_start,result);
            
    return empty;    
}

std::vector<std::string> get_path_directives(std::vector<unsigned> path){
    std::vector<std::string> directives;    
    TwoDimVector sortedPath = sortPath(path);
    std::string newDirective;
    
    // Push directives in the directive list for each new street travelled on
    for(unsigned directiveIndex=0; directiveIndex<sortedPath.size(); directiveIndex++)
    {        
        if(directiveIndex!=0)
        {
            unsigned previousSize = sortedPath[directiveIndex-1].size();
            unsigned previousSegment = sortedPath[directiveIndex-1][previousSize-1];
            unsigned currentSegment  = sortedPath[directiveIndex][0];
            
            newDirective = turnDirective(previousSegment, currentSegment);
            if(newDirective.length()>40){
                std::size_t found = newDirective.find(" ",newDirective.length()/2,1);
                newDirective.insert(found,"\n");
            }
            directives.push_back(newDirective);
        }       
        
        newDirective = straightDirective(sortedPath[directiveIndex]);
        directives.push_back(newDirective);
    }
    
//    for(unsigned directiveIndex=0; directiveIndex<directives.size(); directiveIndex++)
//        std::cout << directives[directiveIndex] << std::endl;

    return directives;
}

TwoDimVector sortPath(std::vector<unsigned> path){
    
    TwoDimVector sortedPath;
    unsigned currStreetID;
    unsigned segmentID;
    int      numDirectives(0);
    int      directiveStreetID(-1);
    StreetSegmentInfo currInfo;
    
    // Sort the street segments by street ID
    for(unsigned segmentIndex=0; segmentIndex<path.size(); segmentIndex++)
    {
        currInfo     = getStreetSegmentInfo(path[segmentIndex]);
        currStreetID = currInfo.streetID;
        segmentID    = path[segmentIndex];
        
        // Check if reached a turn
        if((int)currStreetID != directiveStreetID)
        {
            numDirectives++;
            directiveStreetID = currStreetID;
            
            // Push the segmentID to a new street in the directives
            std::vector<unsigned> newStreet = {segmentID};            
            sortedPath.push_back(newStreet);
        }
        else
            // Push the segmentID to the same street in the directives
            sortedPath[numDirectives-1].push_back(segmentID);
    }
    
    return sortedPath;
}

std::string turnDirective(unsigned const& prevSegmentID, unsigned const& nextSegmentID){
    
    // Initialize message
    std::string message;
    std::stringstream messageStream(message);
    
    // Get Street Name that the driver is turning onto
    unsigned streetID = getStreetSegmentInfo(nextSegmentID).streetID;
    std::string streetName = getStreetName(streetID);
    
    // Get locations of intersection and endpoints of the previous and next 
    // street segments    
    unsigned turnIntersectionID = inCommon(prevSegmentID, nextSegmentID);
    Point2D turnLocation(getIntersectionPosition(turnIntersectionID));    
    Point2D nextSegment (getIntersectionPosition(otherNode(prevSegmentID,turnIntersectionID)));
    Point2D prevSegment (getIntersectionPosition(otherNode(nextSegmentID,turnIntersectionID)));
    
    // Get vectors in the directions traveled along the previous and next 
    // street segments  
    Point2D prevVec  = turnLocation - prevSegment;
    Point2D nextVec  = turnLocation - nextSegment;
    
    // Get principal value of the oriented angle between next vector and 
    // current vector
    double angle = atan2(prevVec.x,prevVec.y) - atan2(nextVec.x,nextVec.y);
    
    messageStream << "Turn ";
    
    if(angle > 0)
        messageStream << "left ";
    else
        messageStream << "right ";
    
    messageStream << "on " << streetName;
    
    return messageStream.str();
}

std::string straightDirective(std::vector<unsigned> currentSegments){
    double streetLength;
    unsigned segmentID;
    
    // Get Street Name
    unsigned streetID = getStreetSegmentInfo(currentSegments[0]).streetID;
    std::string streetName = getStreetName(streetID);
        
    // Initialize message
    std::string message;
    std::stringstream messageStream(message);
    
    // Calculate distance traveled on the street
    streetLength = 0;
    for(unsigned segmentIndex=0; segmentIndex<currentSegments.size(); segmentIndex++)
    {
        segmentID = currentSegments[segmentIndex];
        streetLength += find_street_segment_length(segmentID);
    }

    // Write message
    messageStream.str(message);
    messageStream << "Drive ";        
    
    if(streetLength > 1000)
        // round to nearest 100m and print as km
        messageStream << (float)((int)(streetLength/100))/10 << " km ";
    else
        // round to nearest 10m
        messageStream << ((int)(streetLength/10))*10 << " m ";
    
    messageStream << "on " << streetName; 
    
    //return new directive
    return messageStream.str();
}
