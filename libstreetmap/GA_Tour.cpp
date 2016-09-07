#include "GA_Tour.h"
#include "GA_Population.h"
#include <unordered_set>


std::vector<unsigned> Tour::pick_ups;
std::vector<unsigned> Tour::drop_offs;
std::vector<unsigned> Tour::shuffled_delivery_id;

void Tour::getRandomTour(Tour& tour){
    std::random_shuffle ( shuffled_delivery_id.begin(), 
            shuffled_delivery_id.end());
    
    /* The tour is constructed in the forward order */
    // Push the last node and pop it up
    tour.nodes.push_back(DeliveryNode(shuffled_delivery_id[0]));
    tour.nodes.back().getBitVec().resize(shuffled_delivery_id.size());
    tour.nodes.back().setBit(shuffled_delivery_id[0]);
    
    // Push the rest nodes
    for(int i=1;i<shuffled_delivery_id.size();i++){
        tour.nodes.push_back(DeliveryNode(shuffled_delivery_id[i],
                tour.nodes.back().getBitVec()));
        tour.nodes.back().setBit(shuffled_delivery_id[i]);
    }
    
    
    /* Drop offs are inserted in the node randomly; if the location is not valid,
     * try again until it is valid */
    for(int drop_offs_nodeID=shuffled_delivery_id.size();
            drop_offs_nodeID-shuffled_delivery_id.size()<shuffled_delivery_id.size();
            drop_offs_nodeID++){
        
        while(true){
            int randomLocation=rand() % tour.nodes.size();
            
            // End of the current tour; insert the node right away
            if(randomLocation==tour.nodes.size()-1){
                tour.nodes.push_back(DeliveryNode(drop_offs_nodeID,tour.nodes.back().getBitVec()));
                
                break;
            }
            else{
                // drop_offs_nodeID equals the index of the corresponding pick up 
                // index in the distance matrix
                if(tour.nodes[randomLocation].getBit(drop_offs_nodeID-shuffled_delivery_id.size())){
                    std::vector<DeliveryNode>::iterator iterator = tour.nodes.begin()+randomLocation+1;
                    std::vector<DeliveryNode>::iterator prevIterator = iterator-1;
                        
                    tour.nodes.insert(iterator,DeliveryNode(drop_offs_nodeID,
                            (*prevIterator).getBitVec()));
                
                    break;
                }
            }
        }
    }
    tour.setTime();
    
}
void Tour::getRandomTour(Tour& tour,SimpleTour& theSimpleTour){
    
    std::random_shuffle ( shuffled_delivery_id.begin(), 
        shuffled_delivery_id.end());
    
    tour.nodes.reserve(shuffled_delivery_id.size() * 2);
    theSimpleTour.reserve(shuffled_delivery_id.size() * 2);
    
    /* The tour is constructed in the forward order */
    // Push the last node and pop it up
    theSimpleTour.push_back(shuffled_delivery_id[0]);
    
    tour.nodes.push_back(DeliveryNode(shuffled_delivery_id[0]));
    tour.nodes.back().getBitVec().resize(shuffled_delivery_id.size());
    tour.nodes.back().setBit(shuffled_delivery_id[0]);
    
    // Push the rest nodes
    for(int i=1;i<shuffled_delivery_id.size();i++){
        theSimpleTour.push_back(shuffled_delivery_id[i]);
        tour.nodes.push_back(DeliveryNode(shuffled_delivery_id[i],
                tour.nodes.back().getBitVec()));
        tour.nodes.back().setBit(shuffled_delivery_id[i]);
    }
    
    
    /* Drop offs are inserted in the node randomly; if the location is not valid,
     * try again until it is valid */
    for(int drop_offs_nodeID=shuffled_delivery_id.size();
            drop_offs_nodeID-shuffled_delivery_id.size()<shuffled_delivery_id.size();
            drop_offs_nodeID++){
        
        while(true){
            int randomLocation=rand() % tour.nodes.size();
            
            // End of the current tour; insert the node right away
            if(randomLocation==tour.nodes.size()-1){
                tour.nodes.push_back(DeliveryNode(drop_offs_nodeID,tour.nodes.back().getBitVec()));
                
                break;
            }
            else{
                // drop_offs_nodeID equals the index of the corresponding pick up 
                // index in the distance matrix
                if(tour.nodes[randomLocation].getBit(drop_offs_nodeID-shuffled_delivery_id.size())){
                    std::vector<DeliveryNode>::iterator iterator = tour.nodes.begin()+randomLocation+1;
                    std::vector<DeliveryNode>::iterator prevIterator = iterator-1;
                        
                    std::vector<unsigned>::iterator iterator2 = theSimpleTour.nodes.begin()+randomLocation+1;
                    
                    
                    tour.nodes.insert(iterator,DeliveryNode(drop_offs_nodeID,
                            (*prevIterator).getBitVec()));
                    theSimpleTour.nodes.insert(iterator2,drop_offs_nodeID);
                
                    break;
                }
            }
        }
    }
    tour.setTime();
    theSimpleTour.time = tour.getTime();
    
}

void Tour::getGreedyTour(Tour& tour, unsigned start_node)
{
    
    std::unordered_set<unsigned> nodes_to_travel;
    std::unordered_set<unsigned> nodes_that_can_be_visited;
    std::map<double,unsigned> closest_nodes;
    std::vector<unsigned> path;
    
    // push all the nodes to travel in a BST
    Tour random_tour;
    Tour::getRandomTour(random_tour);
    
    for(unsigned i=0; i<random_tour.size(); i++)
    {
        unsigned currID = random_tour.nodes[i].getNodeID();
        nodes_to_travel.insert(currID);
    }
    
    // store all the nodes that can currently be visited in a vector, right now 
    // only drop-offs can be visited
    for(unsigned i=0; i<random_tour.size(); i++)
    {
        unsigned currID = random_tour.nodes[i].getNodeID();
        DeliveryType type = distanceMatrix.getDeliveryType(currID);
        if(type==Pickup)
            nodes_that_can_be_visited.insert(currID);
    }
    
    // select the starting node
    unsigned present_node = start_node;
    path.push_back(present_node);
        
    // update the nodes that can be visited accordingly
    DeliveryType type = distanceMatrix.getDeliveryType(present_node);
    if(type==Pickup)
    {
        unsigned dropoff_id = distanceMatrix.otherDelivery(present_node);
        nodes_that_can_be_visited.insert(dropoff_id);
    }

    // remove present_nodes from nodes that must be visited
    nodes_to_travel.erase(present_node);
    nodes_that_can_be_visited.erase(present_node);
    
    // while there are still nodes left to travel, add to the path
    while(!nodes_to_travel.empty())
    {
        // order the nodes that can be visited right now in a BST based on their
        // distance from the present node
        closest_nodes.clear();
        
        for(const auto& nodeID: nodes_that_can_be_visited)
        {
            double distance = distanceMatrix(present_node,nodeID);
            closest_nodes.insert(std::pair<double,unsigned>(distance,nodeID));
        }
        
        // generate a random number from 0 to GREEDY_RANGE
        unsigned range = rand() % GREEDY_RANGE;
        
        // use the range to choose one of the closest nodes
        auto it=closest_nodes.begin();
        for (unsigned i=0; it!=closest_nodes.end() && i<=range; it++, i++)
            present_node = (*it).second;
        
        // push the node to the path
        path.push_back(present_node);
        
        // update the nodes that can be visited accordingly
        DeliveryType type = distanceMatrix.getDeliveryType(present_node);
        if(type==Pickup)
        {
            unsigned dropoff_id = distanceMatrix.otherDelivery(present_node);
            nodes_that_can_be_visited.insert(dropoff_id);
        }
        
        // remove present_nodes from nodes that must be visited
        nodes_to_travel.erase(present_node);
        nodes_that_can_be_visited.erase(present_node);
        
    }
    
    tour.set_nodes(path);
    
    tour.setTime();
}

void Tour::getATour(Tour& tour,std::vector<unsigned> nodes){
    for(int i=0;i<nodes.size();i++){
        tour.nodes.push_back(DeliveryNode(nodes[i]));
    }
}

void Tour::insert_gene(Tour const& mother, std::vector<unsigned> const& gene, 
        unsigned  gene_index_in_father, Tour& child)
{
    
    //cout<<"In insert_gene: \n";
    std::unordered_map<unsigned,unsigned> gene_hash_table;
    for(auto iterator = gene.begin();iterator!=gene.end();++iterator)
        gene_hash_table.insert(std::pair<unsigned,unsigned>(*iterator,*iterator));
    
    // copy all of the nodes of mother that are not present in the father gene in
    // two vectors depending on whether they appear before or after the gene
    std::vector<unsigned> before_gene;
    std::vector<unsigned> after_gene;
    
    for(unsigned mother_index=0; mother_index<mother.size(); mother_index++)
    {
        unsigned currMotherNode = mother.nodes[mother_index].getNodeID();
        
        std::unordered_map<unsigned,unsigned>::const_iterator got = 
            gene_hash_table.find(currMotherNode);

        if(got == gene_hash_table.end())
        {
            if(mother_index<gene_index_in_father)
                before_gene.push_back(currMotherNode);
            else
                after_gene.push_back(currMotherNode);
        }
    }
    
#if(DEBUG_PRINT)    
     std::cout << "Before Gene (size=" << before_gene.size() << "):";
    for(unsigned i=0; i<before_gene.size(); i++)
        std::cout << " " << before_gene[i].getNodeID();
    std::cout << std::endl;
    
      
     std::cout << "Gene (size=" << gene.size() << "):";
    for(unsigned i=0; i<gene.size(); i++)
        std::cout << " " << gene[i].getNodeID();
    std::cout << std::endl;
    
      
     std::cout << "After Gene (size=" << after_gene.size() << "):";
    for(unsigned i=0; i<after_gene.size(); i++)
        std::cout << " " << after_gene[i].getNodeID();
    std::cout << std::endl;
#endif
    
    
    std::vector<unsigned> right_before_gene;
    std::vector<unsigned> right_after_gene;
    
    
    for(auto iterator = before_gene.begin();iterator!=before_gene.end();)
    {
        // if pickup -> place right away
        if(distanceMatrix.getDeliveryType(*iterator)==Pickup)
        {
            ++iterator;
        }
        // if drop off -> check if pickup is in gene
        else
        {
            unsigned pickupID = distanceMatrix.otherDelivery(*iterator);
            
            std::unordered_map<unsigned,unsigned>::const_iterator got = 
                gene_hash_table.find(pickupID);
            
            // Corresponding pickup is in the gene
            if(got != gene_hash_table.end()){
                right_after_gene.push_back(*iterator);
                iterator=before_gene.erase(iterator);
            }
            // Corresponding pickup is not in the gene
            else
                ++iterator;
                
        }
    }
        

    // then do the after_gene vector
    
    for(auto iterator = after_gene.begin();iterator!=after_gene.end();)
    {        
        
        // if pickup -> check if drop off is in gene
        if(distanceMatrix.getDeliveryType(*iterator)==Pickup)
        {
            unsigned dropoffID = distanceMatrix.otherDelivery(*iterator);
            
            std::unordered_map<unsigned,unsigned>::const_iterator got = 
                gene_hash_table.find(dropoffID);
                
            // if drop off is in gene: must place before gene, else place after
            if(got != gene_hash_table.end()){
                right_before_gene.push_back(*iterator);
                iterator = after_gene.erase(iterator);
            }
            else
                ++iterator;
        }
        // if drop off -> place right away
        else
        {
            ++iterator;  
        }
    }   
    
    
    std::vector<unsigned> child_nodes;
    child_nodes.reserve(before_gene.size()+right_before_gene.size()+gene.size()+
        right_after_gene.size()+after_gene.size());
    
    child_nodes.insert(child_nodes.end(), before_gene.begin(),  before_gene.end());
    child_nodes.insert(child_nodes.end(), right_before_gene.begin(), right_before_gene.end());
    child_nodes.insert(child_nodes.end(), gene.begin(),   gene.end());
    child_nodes.insert(child_nodes.end(), right_after_gene.begin(), right_after_gene.end());
    child_nodes.insert(child_nodes.end(), after_gene.begin(), after_gene.end());
    
    child.set_nodes(child_nodes);
}

#define SWAP_PRINT 0

void Tour::swap_nodes(unsigned index_1, unsigned index_2)
{
        unsigned lowered_index, increased_index;
    if(index_1<index_2)
    {
        lowered_index   = index_2;
        increased_index = index_1;
    }
    
    else
    {
        lowered_index   = index_1;
        increased_index = index_2;
    }
        
    std::vector<DeliveryNode>::iterator begin,end;
    
    begin = nodes.begin() + increased_index;
    end   = nodes.begin() + lowered_index;
    
    unsigned otherNode1 = distanceMatrix.otherDelivery(index_1);
    unsigned otherNode2 = distanceMatrix.otherDelivery(index_1);
    
    for(auto iterator = begin; iterator!=end; ++iterator)
    {
        if(iterator->getNodeID()==otherNode1 || iterator->getNodeID()==otherNode2)
            return;
    }
    unsigned temp = begin->getNodeID();
    begin->setNodeID(end->getNodeID());
    end->setNodeID(begin->getNodeID());
}

void Tour::two_opt(unsigned index_1, unsigned index_2){
    
    // determine higher and lower indices
    unsigned lowered_index, increased_index;
    if(index_1<index_2)
    {
        lowered_index   = index_2;
        increased_index = index_1;
    }
    
    else
    {
        lowered_index   = index_1;
        increased_index = index_2;
    }
    
    std::vector<DeliveryNode>::iterator begin,end;
    
    begin = nodes.begin() + increased_index;
    end   = nodes.begin() + lowered_index + 1;
    
    for(auto iterator = begin;iterator!=end;++iterator)
    {
        // If it is the dropoff
        if(distanceMatrix.getDeliveryType(iterator->getNodeID())==Dropoff)
            for(auto internalIterator = begin;internalIterator!=iterator;++internalIterator)
            {
                if(internalIterator->getNodeID() == distanceMatrix.otherDelivery(iterator->getNodeID()))
                {
                    unsigned temp = internalIterator->getNodeID();
                    internalIterator->setNodeID(iterator->getNodeID());
                    iterator->setNodeID(temp);
                    break;
                }
            }
    }
    
    std::reverse(begin,end);
}


#define DEBUG_PRINT 0

void Tour::crossover(Tour const& mother, Tour const& father, 
        unsigned gene_size, unsigned gene_index,
        Tour& child)
{
    
    // extract gene from father
    std::vector<unsigned> gene;
    father.extract_gene(gene_size,gene_index,gene);
    
    Tour::insert_gene(mother,gene,gene_index,child);

#if(DEBUG_PRINT)       
    std::cout << "Father:";
    for(unsigned i=0; i<father.size(); i++)
        std::cout << " " << father.nodes[i].getNodeID();
    std::cout << std::endl;
    
    std::cout << "Mother:";
    for(unsigned i=0; i<mother.size(); i++)
        std::cout << " " << mother.nodes[i].getNodeID();
    std::cout << std::endl;
    
    std::cout << "Child: ";
    for(unsigned i=0; i<child.size(); i++)
        std::cout << " " << child.nodes[i].getNodeID();
    std::cout << std::endl;
#endif
    
    
}

void Tour::set_nodes(std::vector<unsigned> const& _nodes){
    nodes.clear();
    nodes.resize(_nodes.size());
    
    for(unsigned i=0; i<_nodes.size(); i++)
    {
        nodes[i] = DeliveryNode(_nodes[i]);
    }
    
}

void Tour::extract_gene (
        unsigned& gene_size, unsigned const& gene_index,std::vector<unsigned>& gene) const
{ 
    //cout<<"In extract_gene: \n";
    // get iterator of gene in father
    std::vector<DeliveryNode>::const_iterator center = nodes.begin() + gene_index;   
    
    // get iterators of beginning and end of gene 
    std::vector<DeliveryNode>::const_iterator begin,end;
   
    begin = center - gene_size/2;
    end   = center + gene_size/2;
    
    // check if gene overflows at the beginning
    if((center-nodes.begin())>gene_size/2)
        end   = center + gene_size/2;
    else
        end   = center + gene_size/2;
    
    unsigned i=0;
    for(auto it=begin; it<end; it++, i++)
        gene.push_back((*it).getNodeID());
    
}

std::vector<DeliveryNode>& Tour::getNodes(){
    return nodes;
}

DeliveryNode Tour::getNode(int index){
    return nodes[index];
}

DeliveryNode* Tour::getNodePtr(int index){
    return &nodes[index];
}

void Tour::setTime(){ 
    
    startDepot=distanceMatrix.closest_depot_to(nodes[0].getNodeID());
    endDepot=distanceMatrix.closest_depot_from(nodes.back().getNodeID());
    
    time=0;
    for(int i=0;i<nodes.size()-1;i++){
        // This line is incorrect now
        time+=distanceMatrix(nodes[i].getNodeID(),nodes[i+1].getNodeID());
    }
    time+=distanceMatrix(startDepot,nodes[0].getNodeID());  
    time+=distanceMatrix(nodes.back().getNodeID(),endDepot);
}

double Tour::getTime(){
    return time;
}

bool Tour::isVaild(){
    return !nodes.empty();
}

unsigned Tour::getStartDepot(){
    return startDepot;
}
unsigned Tour::getEndDepot(){
    return endDepot;
}

DeliveryNode Tour::getNodeAt(unsigned index){
    return nodes[index];
}

unsigned Tour::getNodeIDAt(unsigned index) const {
    return nodes[index].getNodeID();
}
bool Tour::isTourValid(){
    
    for(int i = 0;i<size();i++){
        if(getNodeIDAt(i)>=(size()/2)){
            bool flag=false;
            for(int j=0;j<i;j++)
                if(getNodeIDAt(j)==getNodeIDAt(i)-(size()/2))
                    flag=true;
            if(!flag)
                return false;
        }
    }
    
    return true;
}

bool Tour::isRepeated()
{
    bool repeated(false);
    
    for(int i=0;i<size();i++)
    {
        unsigned currID_i = nodes[i].getNodeID();
        unsigned count(0);
        for(unsigned j=0; j<size(); j++)
        {
            unsigned currID_j = nodes[j].getNodeID();
            if(currID_i == currID_j)
                count++;
        }
        
        if(count>1)
            repeated=true;
    }
    return repeated;
}

void SimpleTour::push_back(unsigned id){
    nodes.push_back(id);
}

void SimpleTour::reserve(unsigned size){
    nodes.size();
}
