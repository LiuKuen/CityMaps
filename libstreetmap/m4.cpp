
#include "m4.h"

std::vector<unsigned> traveling_courier(std::vector<DeliveryInfo> const& deliveries, std::vector<unsigned> const& depots){
   
    // Get start time
    auto startTime = chrono::high_resolution_clock::now();
//    
//    std::vector<unsigned> empty;
//    if(deliveries.size()<10)
//        return empty;
    if(deliveries.size()<20)
        POPULATION_GENERATION_TIME = 0.5;
    else
        POPULATION_GENERATION_TIME = 3.0;
    
    
    /****************************************************************************************************************/
    distanceMatrix.clear();
    //distanceMatrix.init_debug(deliveries,depots);
    distanceMatrix.init(deliveries,depots);
    
    auto currentTime = chrono::high_resolution_clock::now();
    auto prevTime = chrono::high_resolution_clock::now();
    auto wallClock = chrono::duration_cast<chrono::duration<double>> (currentTime - startTime);
        
    cout << "Loading matrix took "<< wallClock.count()<< " seconds." << endl;
    /****************************************************************************************************************/
    Tour resultTour;
    double resultTime;
    std::vector<unsigned> result;

    // Static variable clear
    Tour::shuffled_delivery_id.clear();
    Tour::drop_offs.clear();
    Tour::pick_ups.clear();
    
    // Set the random number generator
    srand(time(NULL));
    
    // Fill the shuffled_pick_ups_id with number from 0 - push_back.size-1
    for(int i=0; i<deliveries.size();i++)
        Tour::shuffled_delivery_id.push_back(i);

    // Initialize drop off and pick up vector in Tour class
    for(int i=0;i<deliveries.size();i++){
        Tour::drop_offs.push_back(deliveries[i].dropOff);
        Tour::pick_ups.push_back(deliveries[i].pickUp);
    }
    

    Population population;
    population.init();
    cout<<"Population size: "<<population.getPopulationSize()<<endl;
    
    resultTour   = population.getBestTour();
    resultTime = population.getBestTime();
    
    std::cout << "Number of deliveries: " << deliveries.size() << std::endl;
    
    // result stores the all segments of the path including 2 depots; result is 
    // the value that should be returned; it copies all elements in resultList
    result.clear();
    result.reserve(10000);  
    
    while(true){
        prevTime = chrono::high_resolution_clock::now();
        population.optimization(resultTour,resultTime);
        currentTime = chrono::high_resolution_clock::now();
        
        wallClock =chrono::duration_cast<chrono::duration<double>> (currentTime - prevTime);
        //cout<<"Optimization took "<<wallClock.count()<<endl;
        
        wallClock =chrono::duration_cast<chrono::duration<double>> (currentTime - startTime);

        if(wallClock.count() > 0.9 * TIME_LIMIT)
            break;
    }
    
    prevTime = chrono::high_resolution_clock::now();
    
    if(resultTour.size()!=0){
        // Get the path from start depot to the first intersection
        distanceMatrix.return_path(result, resultTour.getStartDepot(),
                resultTour.getNodeIDAt(0));

        // Get the path between all intersections
        for(unsigned i=0;i<resultTour.size()-1;i++){
            std::vector<unsigned> tempPath;
            
            distanceMatrix.return_path(result, 
                                       resultTour.getNodeIDAt(i),
                                       resultTour.getNodeIDAt(i+1));
        }   
        
        // Get the path from last intersection to the end depot
        distanceMatrix.return_path(result, 
                                   resultTour.getNodes().back().getNodeID(), 
                                   resultTour.getEndDepot());
        
    }
    currentTime = chrono::high_resolution_clock::now();
    wallClock =chrono::duration_cast<chrono::duration<double>> (currentTime - prevTime);

    cout<<"Generate path took: "<<wallClock.count()<<endl;
#if(!EXERCISE)
    // Highlight depot in blue
    for(int i=0;i<depots.size();i++)
        highlight_intersection(Point2D(getIntersectionPosition(depots[i])),2);

    // Highlight pickups in green; drop offs in red
    for(int i=0;i<deliveries.size();i++){
        highlight_intersection(Point2D(getIntersectionPosition(deliveries[i].dropOff)),1);
        highlight_intersection(Point2D(getIntersectionPosition(deliveries[i].pickUp)),0);
    }

    // Draw the path in black
    setlinewidth(2);
    setcolor(BLACK);
    for(int i=0;i<result.size();i++)
        draw_a_street_segment(result[i]);

#endif
    
    return result;
}

void initialize_intersection_ids(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots, std::vector<unsigned>&intersection_ids){
    //initialize the intersection_ids matrix
    for(unsigned i = 0; i < depots.size(); i++){
        intersection_ids.push_back(depots[i]);
    }
    for(unsigned i = 0; i < deliveries.size(); i++){
        intersection_ids.push_back(deliveries[i].pickUp);
    }
    for(unsigned i = 0; i < deliveries.size(); i++){
        intersection_ids.push_back(deliveries[i].dropOff);
    }
}

int parseLine(char* line){
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i-3] = '\0';
    i = atoi(line);
    return i;
}

int getValue(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];


    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}
