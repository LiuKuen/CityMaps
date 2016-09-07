/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Graph.cpp
 * Author: pahlavil
 * 
 * Created on February 28, 2016, 1:20 AM
 */

#include "B_Graph.h"
#include "m1.h"

Graph::Graph(){}

Graph::Graph(Graph const& rhs)
{
    vertices = rhs.vertices;
}

void Graph::insert(unsigned intersectionID){
    GraphVertex newVertex(intersectionID);
    vertices.push_back(newVertex);
}
    
// Initialize Graph for single source shortest path search
void Graph::singleSourceInitialize(unsigned sourceID){
    for(unsigned i=0; i<size(); i++)
        vertices[i].setDestination();
    
    vertices[sourceID].setSource();
}

void Graph::clear(){
    vertices.clear();
}

bool Graph::empty(){
    return size()==0;
}    
unsigned Graph::size(){
    return vertices.size();
}

GraphVertex& Graph::operator[](unsigned intersectionID){
    return vertices.at(intersectionID);
}

void Graph::connectVertices(){
    
    std::vector<unsigned> streetSegmentVector;
    StreetSegmentInfo info;
    GraphVertex* otherVertex;
    unsigned segmentID;
    
    unsigned numVertex = this->size();    
    for(unsigned vertexID=0; vertexID<numVertex; vertexID++)
    {
        // Get edges
        streetSegmentVector = find_intersection_street_segments(vertexID);
        
        // Insert edges in adjacency list
        unsigned numSegments = streetSegmentVector.size();
        for(unsigned i=0; i<numSegments; i++)
        {
            segmentID = streetSegmentVector[i];
            info = getStreetSegmentInfo(segmentID);
            if(!info.oneWay||info.from==vertices[vertexID].getID()){
                unsigned otherNodeIndex = otherNode(info,vertices[vertexID].getID());
                otherVertex = &vertices[otherNodeIndex];
                vertices[vertexID].addEdge(otherVertex,segmentID);
            }
        }
    }
}

GraphVertex* Graph::shortestPath(unsigned intersectionFrom, unsigned intersectionTo){
    
    // Set static variables
    GraphVertex::destinationID  = intersectionTo;
    GraphVertex::sourceID       = intersectionFrom;
    GraphVertex::heap_t Q;
    
    singleSourceInitialize(intersectionFrom);
    
    // Push source vertex
    GraphVertex::handle_t handle = Q.push(&vertices[intersectionFrom]);
    (*handle)->setHandle(handle);
    
    while(!Q.empty())
    {
        // remove vertex with smallest upper bound
        GraphVertex* u = Q.top();        
        Q.pop();    
        
        // update the upper bounds of all members of its adjacency list
        u->relaxAdj(Q);
        
        // return pointer to destination if it is found
        if(u->getID()==intersectionTo)
            return u;
    }
    
    // If no path is possible, return NULL
    return NULL;
}

GraphVertex* Graph::shortestPath(unsigned intersectionFrom, std::vector<unsigned> intersectionsTo){
    
    // Set static variables
    GraphVertex::destinationID  = -1;
    GraphVertex::sourceID       = intersectionFrom;
    
    // Initialize Q
    GraphVertex::heap_t Q;    
    singleSourceInitialize(intersectionFrom);
    
    // Initialize source
    GraphVertex::handle_t handle = Q.push(&vertices[intersectionFrom]);
    (*handle)->setHandle(handle);
    
    while(!Q.empty())
    {   
        // remove vertex with smallest upper bound
        GraphVertex* u = Q.top();        
        Q.pop();    
        
        // update the upper bounds of all members of its adjacency list
        u->relaxAdj(Q);
        
        // return if any POI is found, it is necessarily the closest
        auto index = find(intersectionsTo.begin(), intersectionsTo.end(), u->getID());        
        if( index != intersectionsTo.end() ){
            POITo=u->getID();
            return u;
        }
    }
    
    // if no path is found, return NULL
    return NULL;
}

void Graph::shortestPaths(unsigned& intersectionFrom, 
                          std::vector<unsigned>& intersectionsTo,
                          std::vector<GraphVertex*>&  destinationPtrs)
{    
    destinationPtrs.clear();
    
    // resize results vector
    destinationPtrs.resize(intersectionsTo.size());
    
    for(int i=0;i<destinationPtrs.size();i++)
        destinationPtrs[i]=NULL;
    
    unsigned numFound(0);
    
    // Set static variables
    GraphVertex::destinationID  = -1;
    GraphVertex::sourceID       = intersectionFrom;
    
    // Initialize Q
    GraphVertex::heap_t Q;    
    singleSourceInitialize(intersectionFrom);
    
    // Initialize source
    GraphVertex::handle_t handle = Q.push(&vertices[intersectionFrom]);
    (*handle)->setHandle(handle);
    
    while(!Q.empty())
    {   
        // remove vertex with smallest upper bound
        GraphVertex* u = Q.top();
        Q.pop();    
        
        // update the upper bounds of all members of its adjacency list
        u->relaxAdj(Q);
        
        // return if any POI is found, it is necessarily the closest
        auto it = find(intersectionsTo.begin(), intersectionsTo.end(), u->getID());        
        if( it != intersectionsTo.end() )
        {
            // put pointer at correct index in destinationPtrs
            unsigned index = it - intersectionsTo.begin();
            destinationPtrs[index] = u;
            numFound++;
            
            // check if all were found
            if(numFound==intersectionsTo.size())
                return;
        }
    }
    
    
    //std::exit(EXIT_FAILURE);
}
    
bool Graph::checkHandles(){
    for(unsigned i=0; i<intersectionGraph.size(); i++)
    if(!intersectionGraph[i].goodHandle())
        //intersectionGraph[i].print();
        return false;
    
    return true;
}

void Graph::getPath(std::vector<unsigned> &path, unsigned intersectionFrom, 
        GraphVertex* destinationVertex){
    
    path.clear();
            
    if(!destinationVertex)
        return;
    
    for(;destinationVertex->getID()!=intersectionFrom;destinationVertex=
            destinationVertex->getPredecessor())
        path.push_back(destinationVertex->getPathID());
    
    std::reverse(path.begin(),path.end());
}

void Graph::getPath_append(std::vector<unsigned> &path, unsigned intersectionFrom, 
        GraphVertex* destinationVertex){
    
            
    if(!destinationVertex)
        return;
    
    for(;destinationVertex->getID()!=intersectionFrom;destinationVertex=
            destinationVertex->getPredecessor())
        path.push_back(destinationVertex->getPathID());
    
    std::reverse(path.begin(),path.end());
}
