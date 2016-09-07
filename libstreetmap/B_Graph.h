/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Graph.h
 * Author: pahlavil
 *
 * Created on February 28, 2016, 1:20 AM
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "B_GraphVertex.h"

// The vector is an itinerary and the float is its summed weight
typedef std::pair<std::vector<unsigned>,float> Path;

class Graph {
private:
    // Vector that holds all the vertices and where the 
    // index is the intersection ID
    std::vector<GraphVertex> vertices;
    
    // Initialize Graph for single source shortest path search
    void singleSourceInitialize(unsigned intersectionID);
public:
    // Constructor
    Graph();
    Graph(Graph const& rhs);
    
    // Insert and remove vertices
    void insert(unsigned intersectionID);
    void clear();
    
    // Number of vertices info
    bool empty();    
    unsigned size();
    
    // Get a vertex in the graph given the intersection ID
    GraphVertex& operator[](unsigned intersectionID);
    
    // Link all vertices to each other
    // This is not done at construction since all the vertices 
    // must already be declared as GraphVertex before they are connected
    void connectVertices();
    
    // Return shortest path(s) from intersection1 to intersection2 
    // using Dijkstra's algorithm
    GraphVertex* shortestPath (unsigned intersection1, unsigned intersection2);   
    GraphVertex* shortestPath (unsigned intersectionFrom, std::vector<unsigned> intersectionsTo);
    void shortestPaths(unsigned& intersectionFrom, 
        std::vector<unsigned>& intersectionsTo, 
        std::vector<GraphVertex*>&  destinationPtrs);
    
    void getPath(std::vector<unsigned> &path, unsigned intersectionFrom, GraphVertex* destinationVertex);
    void getPath_append(std::vector<unsigned> &path, unsigned intersectionFrom, GraphVertex* destinationVertex);
    
    bool checkHandles();
};

#endif /* GRAPH_H */

