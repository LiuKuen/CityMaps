#include <map>
#include "GA_Population.h"

//this is for guarding the data structure
std::mutex tours_mutex;

Population::Population() {
}

void Population::init() {

    auto startTime = chrono::high_resolution_clock::now();
    int i = 0;
    unsigned start_node;

    float temp = GREEDY_RANGE;
    GREEDY_RANGE = 1;
    for (start_node = 0; start_node < Tour::pick_ups.size(); start_node++) {
        Tour randomTour;
        Tour::getGreedyTour(randomTour, start_node);
        tours.insert(std::pair<double, Tour>(randomTour.getTime(), randomTour));
    }

    GREEDY_RANGE = temp;

    while (true) {

        start_node = rand() % Tour::pick_ups.size();
        Tour randomTour;

        Tour::getGreedyTour(randomTour, start_node);
        tours.insert(std::pair<double, Tour>(randomTour.getTime(), randomTour));

        //Tour::getRandomTour(randomTour);       
        //tours.insert(std::pair<double,Tour>(randomTour.getTime(),randomTour)); 

        auto currentTime = chrono::high_resolution_clock::now();
        auto wallClock = chrono::duration_cast<chrono::duration<double>> (currentTime - startTime);
        if (wallClock.count() > POPULATION_GENERATION_TIME || tours.size() >= POPULATION_GENERATION_SIZE)
            break;
    }
}

unsigned Population::getPopulationSize() {
    return tours.size();
}

void Population::selection() {
    //cout<<std::this_thread::get_id()<<" In selection: \n";
    //cout<<"In selection: \n";
    /* Go through the tours and take out the bad ones.
     * Right now I am using an exponential function to random select the 
     * possibility of each tour to be taken out. */
    unsigned tour_size_beginning = tours.size();

    auto iterator = tours.begin();

    for (unsigned i = 0; i < (unsigned) (tour_size_beginning * KILL_PERCENTS); ++i) {
        iterator = tours.end();
        --iterator;
        tours.erase(iterator);
    }

//    iterator = tours.begin();
//
//    int index = 0;
//    while (iterator != tours.end()) {
//        int kill_posibility;
//        if (index < tour_size_beginning / 6) {
//            kill_posibility = 0;
//        } else if (index < 2 * tour_size_beginning / 6) {
//            kill_posibility = 20;
//        } else if (index < 3 * tour_size_beginning / 6) {
//            kill_posibility = 40;
//        } else if (index < 4 * tour_size_beginning / 6) {
//            kill_posibility = 60;
//        } else if (index < 5 * tour_size_beginning / 6) {
//            kill_posibility = 80;
//        } else {
//            kill_posibility = 100;
//        }
//
//        int random_possibility = rand() % 100;
//
//        if (random_possibility < kill_posibility) {
//            iterator = tours.erase(iterator);
//        } else
//            ++iterator;
//
//        index++;
//    }
    //cout<<std::this_thread::get_id()<<" End selection: \n";
}

std::multimap<double, Tour>& Population::getTours() {
    return tours;
}

Tour Population::getBestTour() {
    return tours.begin()->second;
}

double Population::getBestTime() {
    return tours.begin()->first;
}

Tour Population::getTourAt(unsigned index) {
    auto it = tours.begin();

    for (unsigned i = 0; i < index; ++i)
        ++it;

    return it->second;
}

void Population::optimization(Tour& resultTour, double& resultTime) {

    // Get start time
    auto startTime = chrono::high_resolution_clock::now();


    // Only do optimization if the length of nodes is greater than 7
    if (tours.begin()->second.size() > 7) {

        unsigned tour_size_begin = tours.size();
        // select stronger offsprings
        selection();
        unsigned tour_size_after = tours.size();

        //        unsigned numberOfChildren = tour_size_begin - tour_size_after;
        unsigned numberOfChildren = getPopulationSize() / (1.0 - KILL_PERCENTS) - getPopulationSize();

        //numberOfCycles determine the number of times we need to go through the numberOfChildern
        //we will have eight threads per cycle

        std::thread current_threads[NUMBER_OF_CORES];
        unsigned num_threads = NUMBER_OF_CORES;

        unsigned number_of_children_per_thread[num_threads];

        // determine the number of children to generate in each thread
        unsigned remainder = numberOfChildren % num_threads;

        for (unsigned i = 0; i < num_threads; i++) {
            number_of_children_per_thread[i] = numberOfChildren / num_threads;

            // distribute leftovers while there are still some left
            if (remainder != 0) {
                number_of_children_per_thread[i] += 1;
                remainder--;
            }
        }

        // create vector containing 
        std::vector<Tour> chidren_tours[num_threads];

        for (unsigned i = 0; i < num_threads; i++) {
            current_threads[i] = std::thread(&Population::optimization_helper,
                    this, number_of_children_per_thread[i], std::ref(chidren_tours[i]));
        }

        //joining the threads
        for (unsigned i = 0; i < num_threads; i++) {
            current_threads[i].join();
        }

        for (unsigned i = 0; i < num_threads; i++)
            for (unsigned j = 0; j < chidren_tours[i].size(); j++)
                tours.insert(std::pair<double, Tour>(chidren_tours[i][j].getTime(),
                    chidren_tours[i][j]));

    }

    if (resultTime != tours.begin()->first) {
        resultTime = tours.begin()->first;
        resultTour = tours.begin()->second;

        cout << "population size: " << tours.size() << endl;
        cout << "resultTime: " << resultTime << endl;

        auto currentTime = chrono::high_resolution_clock::now();
        auto wallClock = chrono::duration_cast<chrono::duration<double>> (currentTime - startTime);
    }
}

void Population::optimization_helper(unsigned numberOfChildren,
        std::vector<Tour>& childrenVec) {

    for (int i = 0; i < numberOfChildren; i++) {
        unsigned fatherIndex = rand() % getPopulationSize();
        unsigned motherIndex = rand() % getPopulationSize();

        //unsigned gene_size = getTourAt(fatherIndex).size() * GENESIZE_PERCENTS;
        //unsigned gene_size = GENE_SIZE;
        unsigned gene_size = rand() % (unsigned) (getTourAt(fatherIndex).size() * GENESIZE_PERCENTS);

        unsigned gene_index = rand() % (getTourAt(fatherIndex).size() - 2 * gene_size) + gene_size;

        //clock_t time =clock();

        Tour child;
        //    Tour temp = child; //!!!!!!
        Tour::crossover(getTourAt(motherIndex), getTourAt(fatherIndex),
                gene_size, gene_index, child);

        unsigned mutation_index_1, mutation_index_2;

        do {
            mutation_index_1 = rand() % (getTourAt(fatherIndex).size());
            mutation_index_2 = rand() % (getTourAt(fatherIndex).size());
        }        while (
                (child.getNodeIDAt(mutation_index_1) == distanceMatrix.otherDelivery(child.getNodeIDAt(mutation_index_2)))
                || mutation_index_1 == mutation_index_2
                );

        int swap_posibility = rand() % 100;
        if (swap_posibility < 100 * MUTATION_PERCENTS)
            child.swap_nodes(mutation_index_1, mutation_index_2);
            //child.two_opt(mutation_index_1, mutation_index_2);

        child.setTime();
        childrenVec.push_back(child);
        
    }
}

bool Population::isTourValid(Tour& tour) {
    for (int i = 0; i < tour.size(); i++) {
        if (tour.getNodeIDAt(i) >= (tour.size() / 2)) {
            bool flag = false;
            for (int j = 0; j < i; j++)
                if (tour.getNodeIDAt(j) == tour.getNodeIDAt(i)-(tour.size() / 2))
                    flag = true;
            if (!flag)
                return false;
        }
    }

    return true;
}

void Population::clear() {
    tours.clear();
}
