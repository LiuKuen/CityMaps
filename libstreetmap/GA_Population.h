/* 
 * File:   Population.h
 * Author: zhangba8
 *
 * Population is used to a population of candidate Tours.
 * 
 * Created on March 18, 2016, 6:12 PM
 */

#ifndef POPULATION_H
#define POPULATION_H

#include "GA_Tour.h"
#include <thread>
#include "Constants.h"
#include <mutex>



class Population{
private:
    // A list of tours; 
    std::multimap<double,Tour> tours;
    
    //std::map<double,SimpleTour> simpleTours;
    
public:
    /* Constructor */
    Population();
    void init();
    
    /* Setter */
    
    /* Getter */
    unsigned getPopulationSize();
    std::multimap<double,Tour>& getTours();
    Tour getBestTour();
    double getBestTime();
    Tour getTourAt(unsigned index);
    
    // Optimization function
    void optimization(Tour& resultTour,double& resultTime);
    //this is used to multi thread optimization
    void optimization_helper(unsigned numberOfChildren, 
        std::vector<Tour>& childrenVec);
    
    // Selection function
    void selection();
    
    bool isTourValid(Tour& tour);
    
    void clear();
    
};

#endif /* POPULATION_H */

