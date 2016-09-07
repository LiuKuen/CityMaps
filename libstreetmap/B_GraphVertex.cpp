/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GraphNode.cpp
 * Author: pahlavil
 * 
 * Created on February 28, 2016, 1:22 AM
 */

#include "B_GraphVertex.h"
#include "m1.h"
#include "m2.h"

GraphVertex::Edge::Edge(){}

GraphVertex::Edge::Edge(unsigned _segmentID,GraphVertex* _vertex){
    segmentID = _segmentID;
    weight = find_street_segment_travel_time(segmentID);
    vertex = _vertex;
    name = getStreetName(getStreetSegmentInfo(segmentID).streetID);
    streetID = getStreetSegmentInfo(segmentID).streetID;
}

void GraphVertex::Edge::operator=(const Edge& rhs){
    segmentID = rhs.segmentID;
    weight = rhs.weight;
    vertex = rhs.vertex;
}

void GraphVertex::Edge::print(){
    unsigned streetID = getStreetSegmentInfo(segmentID).streetID;
    std::string streetName = getStreetName(streetID);
    
    std::cout << "  Weight: " << weight << std::endl;
    std::cout << "  Along:  " << streetName << std::endl;
    std::cout << "  To:     " << vertex->intersectionID << std::endl;
    std::cout << "  Segment ID: " << segmentID << std::endl;
}

unsigned GraphVertex::size(){
    return adj.size();
}

GraphVertex::GraphVertex() {
    intersectionID = std::numeric_limits<unsigned>::infinity();    
    predecessor = NULL;
    upperBound = std::numeric_limits<float>::infinity();
}

GraphVertex::GraphVertex(unsigned _intersectionID) {
    intersectionID = _intersectionID;    
    predecessor = NULL;
    upperBound = std::numeric_limits<float>::infinity();
}

GraphVertex::GraphVertex(const GraphVertex& rhs){
    intersectionID = rhs.intersectionID;
    predecessor = rhs.predecessor;
    upperBound = rhs.upperBound;
    handle = rhs.handle;
    adj = rhs.adj;
}

void GraphVertex::operator=(const GraphVertex& rhs){
    intersectionID = rhs.intersectionID;
    predecessor = rhs.predecessor;
    upperBound = rhs.upperBound;
    handle = rhs.handle;
    adj = rhs.adj;
}

// used for ordering in the heap
bool operator>(GraphVertex const& lhs, GraphVertex const& rhs)
{
    return lhs.heuristic > rhs.heuristic;
}

GraphVertex::~GraphVertex() {}

void GraphVertex::relaxAdj(GraphVertex::heap_t& Q){
    
    unsigned numEdges = adj.size();
    float penalty=0;
    
//#if(!EXERCISE)
//    if(this->upperBound!=0){
//        setlinewidth(2);
//        draw_a_street_segment(this->path->segmentID);
//        //delay(1);
//        flushinput();
//    }
//#endif
    
    // Go through all edges in adjacency list
    for(unsigned i=0; i<numEdges; i++)
    {
        // If can reduce upper bound estimate on *this, reduce it 
        // and add this as a predecessor to currVertex        
        if(this->upperBound!=0 && path->streetID != adj[i].streetID)
            //15s penalty
            penalty=0.25;
        else
            penalty=0;
        
        GraphVertex* curr = adj[i].vertex;
            
        // If can reduce upper bound estimate on *this, reduce it 
        // and add this as a predecessor to currVertex        
        if(curr->upperBound > (upperBound + adj[i].weight + penalty))
        {                    
            
            handle_t handleVar;

            // if was never visited, add to heap
            if(!curr->visited)
            {
                handleVar = Q.push(adj[i].vertex);
                curr->handle  = handleVar;
                curr->visited = true;
            }            
            
            // update member variables
            curr->upperBound    = upperBound + adj[i].weight + penalty;
            curr->predecessor   = this;
            curr->path          = &adj[i];
            curr->setHeuristic();
             
            // update in heap
            handleVar = curr->handle;
            Q.decrease(handleVar);
            
        }
        
    }
}

void GraphVertex::addEdge(GraphVertex* otherVertex, unsigned segmentID){
    Edge newEdge(segmentID,otherVertex);
    adj.push_back(newEdge);
}

void GraphVertex::print(){
    std::cout << "*****************************************" << std::endl;
    std::cout << "Vertex:       " << intersectionID << std::endl;       
    std::cout << "Size:         " << this->size() << std::endl;
    std::cout << "d:            " << upperBound << std::endl;
    std::cout << "predecessor:  ";
    
    if(predecessor==NULL)
        std::cout << "NULL" << std::endl;
    else
        std::cout << predecessor->intersectionID << std::endl;
    
      
    for(unsigned edge=0; edge<adj.size(); edge++)
    {
        adj[edge].print();
    }
    std::cout << "*****************************************" << std::endl;
}


bool GraphVertex::goodHandle(){
    return *handle==this;
}

// GraphVertex static members
int GraphVertex::destinationID;
int GraphVertex::sourceID;

void GraphVertex::setHeuristic(){
    
    static const float MAX_SPEED = 33.4;
    static const float SEC_PER_MIN = 60;
    
    if(destinationID == -1)
    {
        heuristic =  upperBound;
        return;
    }
    
    // Get (x,y) coordinates of the three points
    LatLon  targetLatLon  = getIntersectionPosition(destinationID);
    LatLon  currentLatLon = getIntersectionPosition(intersectionID);
    
    // Distance to destination
    double distanceToTarget = find_distance_between_two_points(currentLatLon, targetLatLon);
    double timeToTarget = distanceToTarget/MAX_SPEED/SEC_PER_MIN;
    
    heuristic = upperBound + timeToTarget;
}
