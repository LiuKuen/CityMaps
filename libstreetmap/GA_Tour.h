/* 
 * File:   Tour.h
 * Author: zhangba8
 *
 * Tour is use to represent one delivery path.
 * 
 * Created on March 18, 2016, 6:10 PM
 */

#ifndef TOUR_H
#define TOUR_H

#include "GA_Node.h"
#include <list>
#include <algorithm>
#include <iostream>
#include "Global_var.h"

typedef std::pair<unsigned,double> unsignedDoublePair;

class SimpleTour{
public:
    std::vector<unsigned> nodes;
    double time;
    
    void setTime();
    
    void push_back(unsigned);
    void reserve(unsigned);
};



class Comparetor
{
public:
    // Compare two unsignedDoublePair structs.
    bool operator()(const unsignedDoublePair& x, const unsignedDoublePair& y) const
    {
        return x.second > y.second;
    }
};

class Tour{
private:
    // A list of nodes to represent a tour
    std::vector<DeliveryNode> nodes;
    double time;
    // Start and end depot of the tour
    unsigned startDepot;
    unsigned endDepot;
    
public:
    // Return a random tour; notice that depots are not included in the tour
    static void getRandomTour(Tour&);
    static void getRandomTour(Tour&,SimpleTour&);
    static void getGreedyTour(Tour&);
    static void getGreedyTour(Tour&, unsigned start_node);
    static void getATour(Tour& tour,std::vector<unsigned> nodes);
    
    // Vector to store all pick-ups and drop-offs
    static std::vector<unsigned> pick_ups;
    static std::vector<unsigned> drop_offs;
    // shuffled_pick_ups_id stores 0-the number of pick_ups-1; it is used to shuffle
    static std::vector<unsigned> shuffled_delivery_id;
    
    /* Accessor */
    DeliveryNode getNode(int index);
    DeliveryNode* getNodePtr(int index);
    std::vector<DeliveryNode>& getNodes();
    double getTime();
    bool isVaild();
    unsigned getStartDepot();
    unsigned getEndDepot();
    DeliveryNode getNodeAt(unsigned index);
    unsigned getNodeIDAt(unsigned index) const ;
    
    /* Setter */
    void setTime();
    void set_nodes(std::vector<unsigned> const&);
    
    // Cross over function. Return a new tour
    static void insert_gene(Tour const& mother, std::vector<unsigned> const& gene,
        unsigned  gene_index_in_mother, Tour& child);
    static void crossover(Tour const& mother, Tour const& father, 
        unsigned gene_size, unsigned  gene_index,Tour& child);
    void extract_gene(
        unsigned& gene_size, unsigned const& gene_index,std::vector<unsigned>&) const;
    
    void swap_nodes(unsigned small_index, unsigned large_index);
    void two_opt(unsigned small_index, unsigned large_index);
    
    unsigned size() const
    {
        return nodes.size();
    }
    
    void print()
    {
        std::cout << "Size: " << nodes.size() << std::endl;
        
        std::cout << " Nodes: " << nodes.size() << std::endl;
        for(unsigned i=0; i<nodes.size(); i++)
        {
            std::cout << "  Node: " << i << std::endl;
            std::cout << "      NodeID: " << nodes[i].getNodeID() << std::endl;
            std::cout << "      Other delivery: " << distanceMatrix.otherDelivery(nodes[i].getNodeID()) << std::endl;
            
            DeliveryType type = distanceMatrix.getDeliveryType(nodes[i].getNodeID());
            if     (type==Pickup)
                std::cout << "      Pickup: " << nodes[i].getNodeID() << std::endl;
            else if(type==Dropoff)
                std::cout << "      Drop-off: " << nodes[i].getNodeID()-nodes.size()/2 << std::endl;
            else if(type==Depot)
                std::cout << "      Depot" << std::endl;;
                
        }
    }
    
    /* Tester */
    bool isTourValid();
    bool isRepeated();
};
    
#endif /* TOUR_H */

