/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DistanceMatrix.h
 * Author: pahlavil
 *
 * Created on March 17, 2016, 8:57 PM
 */

//this matrix contains the distance between pickUp, dropOff, and depot intersection ids
//ie) if pickUp 'a' has an intersection id of 421 and drop off 'a' has an intersection id
//    of 523 then the distance_matrix will contain the distance it takes to get from 421 to 523.
//
//1) The distance_matrix will not calculate the distance depots.
//2) The distance_matrix are ordered by:
//      1) depots
//      2) pickUp
//      3) dropOff
//   for the indices. Meaning that if there are 'n' depots, 'm' pickUps, and
//   'o' dropOffs, the first 'n' indices will be depots, the next 'm' indices
//   will be pickUps and the last 'o' indices will be dropOffs. This pattern
//   applies for both the rows and the columns. In the end it will be a 
//   n+m+o  by n+m+o matrix.

#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include <vector>
#include "Constants.h"
#include "B_Graph.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <unordered_map>

class DistanceMatrix {
public: 
    // constructor and destructor
    DistanceMatrix();
    ~DistanceMatrix();
    
    // STL like functions
    unsigned size();
    void clear();
    void resize(unsigned);
    
    // initialize matrix based on vector of delivery info
    void init(std::vector<DeliveryInfo> const& deliveries, std::vector<unsigned> const& depots);
    
    void init_debug(std::vector<DeliveryInfo> const& deliveries, std::vector<unsigned> const& depots);
    
    // get distance between two nodes given their node ID's
    double operator()(unsigned const& startID,unsigned const& endID) const;
    
    // return delivery type (i.e. pickup or drop off of a node)
    DeliveryType getDeliveryType(unsigned const& nodeID) const;
    
    // return node ID of corresponding pickup if drop off or of corresponding 
    // drop off if pickup
    unsigned otherDelivery(unsigned const& nodeID) const;

    // fill matrix with distances
    void fill();
    
    //this function is for testing purposes
    void print_matrix();
    
    //returns the path of two nodes
    void return_path(std::vector<unsigned>& path, unsigned const& start_node_id, unsigned const& end_node_id);
    
    // return the number of deliveries
    unsigned getNumOfDeliveries();
    
    // return the intersection id from a nodeID
    unsigned getIntersectionID(unsigned nodeID);
    
    // Access the closest depot
    unsigned closest_depot_from(unsigned const& nodeID) const;
    unsigned closest_depot_to(unsigned const& nodeID) const;
    
private:
    
    // square matrix that holds the distances between intersections
    // rows are starting points and columns are destinations
    boost::numeric::ublas::matrix<double> matrix;
    
    // square matrix that holds the path between intersections
    //boost::numeric::ublas::matrix<std::vector<unsigned>> path_matrix;
    std::vector<std::vector<std::vector<unsigned>>> path_matrix;
    
    // number of each type of element in the matrix
    unsigned numOfDepots;
    unsigned numOfDeliveries;
    
    // holds the nodeID's of all the depots
    std::vector<unsigned> depot_vector;
    
    // holds the nodeId for the closest depot to/from each 
    std::vector<unsigned> c_depot_to_vector;
    std::vector<unsigned> c_depot_from_vector;
    
    // this vector returns the location of the intersection of a node 
    // in the distance matrix
public:
    std::vector<unsigned> nodeID_to_matrixID;
private:
    // this vector returns the intersection ID corresponding to an index in
    // the distance matrix
    std::vector<unsigned> matrixID_to_intersectionID;

    // fills the vector with the each time
    void fill_helper(unsigned threadNumber, 
                     unsigned numberOfIterations,
                     unsigned remainder,
                     std::vector<unsigned>& destinationIDs, 
                     Graph& localGraph); 
        
    unsigned closest_depot_from_internal(unsigned const& nodeID) const;
    unsigned closest_depot_to_internal  (unsigned const& nodeID) const;
};

#endif /* DISTANCEMATRIX_H */

