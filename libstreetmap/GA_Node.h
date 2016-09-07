/* 
 * File:   Node.h
 * Author: zhangba8
 *
 * Node is used to represent all pick-ups and drop-offs.
 * Each node includes an id, which is used in the distance matrix.
 * It also includes a vector of boolean to represent the order of the pick-ups in the path
 * 
 * Created on March 18, 2016, 5:42 PM
 */

#ifndef NODE_H
#define NODE_H

#include <vector>
#include <limits>

class DeliveryNode{
private:
    // nodID is a unique id for every node in all deliveries
    unsigned nodeID;
    
    // Vector of bits used to remember the order of pick-ups at the current location
    std::vector<bool> bitVec;
    
public:
    /* Constructor */
    DeliveryNode();
    DeliveryNode(int _nodeID);
    DeliveryNode(int _nodeID,std::vector<bool>& _bitVec);
    
    /* Setter */
    void setBit(int index);
    
    /* Getter */
    bool getBit(int index);
    std::vector<bool>& getBitVec();
    unsigned getNodeID() const;
    void setNodeID(unsigned _nodeID){
        nodeID=_nodeID;
    }
    
    
    bool isPickUp() const;
};




#endif /* NODE_H */

