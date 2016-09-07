/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DistanceMatrix.cpp
 * Author: pahlavil
 * 
 * Created on March 17, 2016, 8:57 PM
 */

#include "GA_DistanceMatrix.h"
#include "m3.h"
#include <thread>

// constructor and destructor

DistanceMatrix::DistanceMatrix()
{
    resize(0);
}

DistanceMatrix::~DistanceMatrix()
{
    clear();
}

// STL like functions
unsigned DistanceMatrix::size()
{
    // return either size1 or size2 of matrix
    // doesn't matter which one since matrix is square
    return matrix.size1();
}

void DistanceMatrix::clear()
{
    matrix.clear();
    path_matrix.clear();
    nodeID_to_matrixID.clear();
    matrixID_to_intersectionID.clear();
    depot_vector.clear();
    c_depot_to_vector.clear();
    c_depot_from_vector.clear();
}
    
void DistanceMatrix::resize(unsigned size)
{    
    // resize matrix without preserving elements
    matrix.resize(size,size,false);
    path_matrix.resize(size);
}

void DistanceMatrix::init(std::vector<DeliveryInfo> const& deliveries, 
        std::vector<unsigned> const& depots)
{

    // separate DeliveryInfo into a vector pickups and a vector of drop offs
    std::vector<unsigned> pickups (deliveries.size());
    std::vector<unsigned> dropoffs(deliveries.size());
    
    for(unsigned i=0; i<deliveries.size(); i++)
    {
        pickups [i] = deliveries[i].pickUp;
        dropoffs[i] = deliveries[i].dropOff;
    }   
    
    // update number of each type in matrix
    numOfDepots   = depots.size();
    numOfDeliveries  = pickups.size();
    
    // temporarily fill the nodeID_to_matrixID vector with intersection ID's
    // they will then get replaced by matrix ID's    
    nodeID_to_matrixID.insert
        (nodeID_to_matrixID.end(), pickups.begin(),  pickups.end());
    nodeID_to_matrixID.insert
        (nodeID_to_matrixID.end(), dropoffs.begin(), dropoffs.end());
    nodeID_to_matrixID.insert
        (nodeID_to_matrixID.end(), depots.begin(),   depots.end());
    
    // fill the depot_vector
    for(unsigned i=0; i<numOfDepots; i++)
        depot_vector.push_back(i + numOfDeliveries*2);
    
    // fill the matrixID to intersection ID's vector and 
    // replace the intersection ID's with matrix ID's in the nodeID_to_matrixID vector 
    unsigned matrixID(1000000000);
    
    for(unsigned k=0; k<3; k++)
    {
        // cycle through pickups, drop-offs and depots
        std::vector<unsigned>::const_iterator start, end;
        
        if      (k==0)
        {
            start = pickups.begin();
            end   = pickups.end();
        }
        else if (k==1)
        {
            start = dropoffs.begin();
            end   = dropoffs.end();
        }
        else if (k==2)
        {
            start = depots.begin();
            end   = depots.end();
        }
        
        for(auto i=start; i!=end; i++)
        {
            unsigned currIntersectionID = *i;

            // check if the intersection is already in the matrix
            auto it = find(matrixID_to_intersectionID.begin(),
                    matrixID_to_intersectionID.end(),currIntersectionID);

            // if the intersection is not already in the matrix add it and replace 
            // all occurrences of the intersection in nodeID_to_matrixID with the new matrixID (i)
            if(it==matrixID_to_intersectionID.end())
            {
                matrixID_to_intersectionID.push_back(currIntersectionID);
                std::replace(nodeID_to_matrixID.begin(),nodeID_to_matrixID.end(),
                        currIntersectionID,matrixID);
                matrixID++;
            }

        }
    }
    for(int i=0;i<nodeID_to_matrixID.size();i++)
        nodeID_to_matrixID[i]-=1000000000;
    
    //print_matrix();
    
    // fill the entries of the matrix with distances
    resize(matrixID_to_intersectionID.size());
    fill();
    
    // fill the two depot vectors
    for(unsigned i=0; i<numOfDeliveries*2; i++){
        c_depot_to_vector.push_back(closest_depot_to_internal(i));
        c_depot_from_vector.push_back(closest_depot_from_internal(i));
    }
    
    
}
void DistanceMatrix::init_debug(const std::vector<DeliveryInfo>& deliveries, 
        const std::vector<unsigned>& depots)
{
    

    // separate DeliveryInfo into a vector pickups and a vector of drop offs
    std::vector<unsigned> pickups (deliveries.size());
    std::vector<unsigned> dropoffs(deliveries.size());
    
    for(unsigned i=0; i<deliveries.size(); i++)
    {
        pickups [i] = deliveries[i].pickUp;
        dropoffs[i] = deliveries[i].dropOff;
    }   
    
    // update number of each type in matrix
    numOfDepots   = depots.size();
    numOfDeliveries  = pickups.size();
    
    // temporarily fill the nodeID_to_matrixID vector with intersection ID's
    // they will then get replaced by matrix ID's    
    nodeID_to_matrixID.insert
        (nodeID_to_matrixID.end(), pickups.begin(),  pickups.end());
    nodeID_to_matrixID.insert
        (nodeID_to_matrixID.end(), dropoffs.begin(), dropoffs.end());
    nodeID_to_matrixID.insert
        (nodeID_to_matrixID.end(), depots.begin(),   depots.end());
    
    // fill the depot_vector
    for(unsigned i=0; i<numOfDepots; i++)
        depot_vector.push_back(i + numOfDeliveries*2);
    
    // fill the matrixID to intersection ID's vector and 
    // replace the intersection ID's with matrix ID's in the nodeID_to_matrixID vector 
    unsigned matrixID(1000000000);
    
    for(unsigned k=0; k<3; k++)
    {
        // cycle through pickups, drop-offs and depots
        std::vector<unsigned>::const_iterator start, end;
        
        if      (k==0)
        {
            start = pickups.begin();
            end   = pickups.end();
        }
        else if (k==1)
        {
            start = dropoffs.begin();
            end   = dropoffs.end();
        }
        else if (k==2)
        {
            start = depots.begin();
            end   = depots.end();
        }
        
        for(auto i=start; i!=end; i++)
        {
            unsigned currIntersectionID = *i;

            // check if the intersection is already in the matrix
            auto it = find(matrixID_to_intersectionID.begin(),
                    matrixID_to_intersectionID.end(),currIntersectionID);

            // if the intersection is not already in the matrix add it and replace 
            // all occurrences of the intersection in nodeID_to_matrixID with the new matrixID (i)
            if(it==matrixID_to_intersectionID.end())
            {
                matrixID_to_intersectionID.push_back(currIntersectionID);
                std::replace(nodeID_to_matrixID.begin(),nodeID_to_matrixID.end(),
                        currIntersectionID,matrixID);
                matrixID++;
            }

        }
    }
    for(int i=0;i<nodeID_to_matrixID.size();i++)
        nodeID_to_matrixID[i]-=1000000000;
    
    //print_matrix();
    
    // fill the entries of the matrix with distances
    resize(matrixID_to_intersectionID.size());
    for(int i=0;i<size();i++)
        for(int j=0;j<size();j++)
        {
            matrix(i, j) = 1;
            std::vector<unsigned> path;
            path_matrix[i].push_back(path);
        }
            
    
    // fill the two depot vectors
    for(unsigned i=0; i<numOfDeliveries*2; i++){
        c_depot_to_vector.push_back(closest_depot_to_internal(i));
        c_depot_from_vector.push_back(closest_depot_from_internal(i));
    }
    
}
// fill matrix with distances
void DistanceMatrix::fill(){
    //evenly divide the matrix into different sizes where each thread completes 
    //their own "section" of the matrix
    
    // number of rows filled with distances
    //we use 
    unsigned current_row(0);
    
    //matrix is a square
    unsigned size_of_matrix = size(); 
    
    if(size_of_matrix <= NUMBER_OF_CORES){
        std::thread current_threads[size_of_matrix];
        
        for(unsigned i = 0; i < size_of_matrix; i++){
            current_threads[i] = std::thread(&DistanceMatrix::fill_helper, 
                                             this,
                                             i, //threadNumber
                                             1, //numberOfIterations
                                             0, //remainder
                                             std::ref(matrixID_to_intersectionID),
                                             std::ref(multiIntersectionGraph[i]));
        }
        for(unsigned i = 0; i < size_of_matrix; i++){
            if(current_threads[i].joinable())
                current_threads[i].join();
        }
    }
    else{
        unsigned remainder = size_of_matrix % NUMBER_OF_CORES;  
        unsigned number_of_interations_per_thread = size_of_matrix / NUMBER_OF_CORES;
        std::thread current_threads[NUMBER_OF_CORES];
        
        //make each thread start completing their own "section" of the matrix
        //ie) if the size of the matrix was 10x10, there will be 5 threads and each
        //thread has a section (row) of 2 
    
        for(unsigned i = 0; i < NUMBER_OF_CORES; i++){
            current_threads[i] = std::thread(&DistanceMatrix::fill_helper, 
                                             this,
                                             i, 
                                             number_of_interations_per_thread,
                                             remainder,
                                             std::ref(matrixID_to_intersectionID),
                                             std::ref(multiIntersectionGraph[i]));
        }
        
        //join all the threads
        for(unsigned i = 0; i < NUMBER_OF_CORES; i++){
            if(current_threads[i].joinable())
                current_threads[i].join();
        }
    }
}

//choose_data determines which data-structure to take from the vector
//numberOfIterations describe the number of rows that each thread needs to complete
//work on average
void DistanceMatrix::fill_helper(unsigned threadNumber, 
                                 unsigned numberOfIterations, 
                                 unsigned remainder,
                                 std::vector<unsigned>& destinationIDs, 
                                 Graph& local_graph){
    //note if remainder is not zero some threads may have to do some extra work
    //iterations tell us the exact amount of for a specific thread rather than the average
    unsigned iterations = 0;
    unsigned starting_row = 0;
    if(remainder != 0){
        if(threadNumber < remainder){
            iterations = numberOfIterations + 1;
            starting_row = threadNumber*numberOfIterations + threadNumber;
        }
        else if(threadNumber >= remainder){
            iterations = numberOfIterations;
            starting_row = threadNumber*numberOfIterations + remainder;
        }
    }
    else{
        iterations = numberOfIterations;
        starting_row = threadNumber*numberOfIterations;
    }
    
    //starting row describes which row each thread starts off at
    
    for(unsigned i = 0; i < iterations; i++){
            
        unsigned startID = destinationIDs[starting_row + i];

        std::vector<GraphVertex*> destinationPtrs;
        local_graph.shortestPaths(startID, destinationIDs, destinationPtrs);

        for(unsigned column = 0; column < destinationIDs.size(); column++)
        {
            // get path
            std::vector<unsigned> path;
            local_graph.getPath(path, startID, destinationPtrs[column]);

            // compute travel time and store it in matrix
            double path_length;

            if(destinationPtrs[column]==NULL){    // No path exists
                path_length = std::numeric_limits<double>::infinity();
            }
            else// Path exists
                path_length = compute_path_travel_time(path);

            // store in matrix
            matrix(starting_row + i, column) = path_length;
            path_matrix[starting_row + i].push_back(path);
        }
    }
}

double DistanceMatrix::operator()(unsigned const& start_node_id,unsigned const& end_node_id) const
{
    unsigned start_matrix_id = nodeID_to_matrixID[start_node_id];
    unsigned end_matrix_id   = nodeID_to_matrixID[end_node_id];
    return matrix(start_matrix_id,end_matrix_id);
}

// return delivery type (i.e. pickup or drop off of a node)
DeliveryType DistanceMatrix::getDeliveryType(unsigned const& nodeID) const
{
    if(nodeID<numOfDeliveries)
        return Pickup;
    else if(nodeID<(2*numOfDeliveries))
        return Dropoff;
    else
        return Depot;
}

// return node ID of corresponding pickup if drop off or of corresponding 
// drop off if pickup
unsigned DistanceMatrix::otherDelivery(unsigned const& nodeID) const
{
    DeliveryType type = getDeliveryType(nodeID);
    
    if(type==Pickup)
        return (nodeID+numOfDeliveries);
    else
        return (nodeID-numOfDeliveries);
}
    
unsigned DistanceMatrix::closest_depot_from_internal(unsigned const& nodeID) const
{
    unsigned current_depot,closest_depot=depot_vector[0];
    
    double current_distance, shortest_distance = std::numeric_limits<double>::infinity();
    
    for(unsigned i=0; i<numOfDepots; i++)
    {
        current_depot = depot_vector[i];
        current_distance = matrix(nodeID_to_matrixID[nodeID],
                nodeID_to_matrixID[current_depot]);
        if(current_distance<shortest_distance)
        {
            shortest_distance = current_distance;
            closest_depot = current_depot;
        }
    }
    
    return closest_depot;
}

unsigned DistanceMatrix::closest_depot_to_internal(unsigned const& nodeID) const
{
    unsigned current_depot,closest_depot=depot_vector[0];
   
    
    double current_distance, shortest_distance = std::numeric_limits<double>::infinity();
    
    for(unsigned i=0; i<numOfDepots; i++)
    {
        current_depot = depot_vector[i];
        current_distance = matrix(nodeID_to_matrixID[current_depot],
                nodeID_to_matrixID[nodeID]);
        if(current_distance < shortest_distance)
        {
            shortest_distance = current_distance;
            closest_depot = current_depot;
        }
    }
    
    return closest_depot;
}

void DistanceMatrix::print_matrix(){
    
    
    std::cout << "Number of depots: " << numOfDepots << std::endl;
    std::cout << "Number of deliveries: " << numOfDeliveries << std::endl;
    
    std::cout << "nodeID_to_matrixID" << std::endl;
    for (unsigned i=0; i<nodeID_to_matrixID.size(); i++)
    {
        std::cout << i << " : " << nodeID_to_matrixID[i] << std::endl;
    }
    
    std::cout << "matrixID_to_intersectionID" << std::endl;
    for (unsigned i=0; i<matrixID_to_intersectionID.size(); i++)
    {
        std::cout << i << " : " << matrixID_to_intersectionID[i] << std::endl;
    }
    
//    for(unsigned i = 0; i < size(); i++){
//        for(unsigned k = 0; k < size(); k++){
//            cout << matrix(i,k) << " ";
//        }
//        cout << endl;
//    }
}

void DistanceMatrix::return_path(std::vector<unsigned>& result, unsigned const& start_node_id, unsigned const& end_node_id){
    
    unsigned start_matrix_id = nodeID_to_matrixID[start_node_id];
    unsigned end_matrix_id   = nodeID_to_matrixID[end_node_id];
    result.insert(result.end(),path_matrix[start_matrix_id][end_matrix_id].begin(),
            path_matrix[start_matrix_id][end_matrix_id].end());
}

unsigned DistanceMatrix::getNumOfDeliveries(){
    return numOfDeliveries;
}

unsigned DistanceMatrix::getIntersectionID(unsigned nodeID){
    return matrixID_to_intersectionID[nodeID_to_matrixID[nodeID]];
}

unsigned DistanceMatrix::closest_depot_from(unsigned const& nodeID) const{
    return c_depot_from_vector[nodeID];
}
unsigned DistanceMatrix::closest_depot_to(unsigned const& nodeID) const{
    return c_depot_to_vector[nodeID];
}
