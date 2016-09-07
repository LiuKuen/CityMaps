/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GraphNode.h
 * Author: pahlavil
 *
 * Created on February 28, 2016, 1:22 AM
 */

#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <vector>
#include <boost/heap/fibonacci_heap.hpp>
#include <string>

class GraphVertex {
public:
    // Sign is reversed because Boost uses max-heap
    struct VertexCompare{
        bool operator()(GraphVertex* const& lhs, GraphVertex* const& rhs) const
        {
            return *lhs > *rhs;
        }
    };
    
    // Small struct that holds a weighted directed edge
    struct Edge{
        unsigned segmentID;
        float weight;
        GraphVertex* vertex;
        unsigned streetID;
        std::string name;
        
        Edge();
        Edge(unsigned _segmentID,GraphVertex* _vertex);
        void operator=(const Edge& rhs);
        void print();
    };   
    
    // Current source and destination ID's
    static int destinationID;
    static int sourceID; 
    
    
    typedef boost::heap::fibonacci_heap<GraphVertex*,boost::heap::compare<VertexCompare>> heap_t; 
    typedef heap_t::handle_type handle_t;
private: 
    
    // Intersection ID and index in the graph
    unsigned intersectionID;
    
    // Heuristic estimate
    double heuristic;
    
    // Adjacency list
    std::vector<Edge> adj;
    
    // Predecessor in shortest path search
    GraphVertex* predecessor;
    
    // Edge from predecessor to this
    Edge* path;
    
    // Upper bound of the distance estimate from 
    // the source in shortest path search
    float upperBound;
    
    // min-heap handle
    handle_t handle;
    
    // check if was inserted in Q
    bool visited;
public: 
    
    // Constructors and destructor
    GraphVertex();
    GraphVertex(unsigned _intersection_ID);
    GraphVertex(const GraphVertex& rhs);
    ~GraphVertex();
    
    // Assignment operator
    void operator=(const GraphVertex& rhs);
    friend bool operator>(GraphVertex const& lhs, GraphVertex const& rhs);
    
    // Add edge to adjacency list
    void addEdge(GraphVertex* other, unsigned segmentID);
    
    // Relax all edges in adjacency list and return false when no path found
    void relaxAdj(GraphVertex::heap_t& Q);
    
    // Accessors
    unsigned size();
    unsigned getID(){return intersectionID;}
    GraphVertex* getPredecessor(){return predecessor;}
    unsigned getPathID(){return path->segmentID;}
    
    void setHandle(handle_t _handle){
        handle = _handle;
    }
    
    void setSource(){
        upperBound = 0;
        visited = true;
    }
    
    void setDestination(){
        upperBound = std::numeric_limits<float>::infinity();
        predecessor = NULL;
        visited = false;
    }
    
    void setVisited(){
        visited = true;
    }
    
    // Print for debugging
    void print();
    
    // Checks if the handle is correct
    bool goodHandle();

    // Calculate and update heuristic estimate
    void setHeuristic();
        
};

#endif /* GRAPHNODE_H */

