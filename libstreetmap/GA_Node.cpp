#include "GA_Node.h"
#include "GA_Tour.h"

DeliveryNode::DeliveryNode(){}
//    matrixId=std::numeric_limits<int>::max();
//    intersectionId=std::numeric_limits<unsigned>::max();
//}
DeliveryNode::DeliveryNode(int _nodeID){
    nodeID=_nodeID;
}

DeliveryNode::DeliveryNode(int _nodeID,std::vector<bool>& _bitVec){
    nodeID=_nodeID;
    bitVec=_bitVec;
}
bool DeliveryNode::getBit(int index) {
    return bitVec[index];
}

void DeliveryNode::setBit(int index){
    bitVec[index]=1;
}

std::vector<bool>& DeliveryNode::getBitVec(){
    return bitVec;
}

unsigned DeliveryNode::getNodeID() const{
    return nodeID;
}
    
bool DeliveryNode::isPickUp() const{
    if(nodeID >= Tour::pick_ups.size())
        return false;
}
