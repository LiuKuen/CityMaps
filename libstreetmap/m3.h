/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3.h
 * Author: liujeffr
 *
 * Created on February 27, 2016, 9:27 PM
 */

#pragma once
#include <vector>
#include <string>
#include <m2.h>

// Returns a path (route) between the start intersection and the end 
// intersection, if one exists. If no path exists, this routine returns 
// an empty (size == 0) vector. If more than one path exists, the path 
// with the shortest travel time is returned. The path is returned as a vector 
// of street segment ids; traversing these street segments, in the given order,
// would take one from the start to the end intersection.
std::vector<unsigned> find_path_between_intersections(unsigned 
                   intersect_id_start, unsigned intersect_id_end);
void find_path_between_intersections(unsigned intersect_id_start, 
        unsigned intersect_id_end, std::vector<unsigned>&);
void find_path_between_intersections_append(unsigned intersect_id_start, 
        unsigned intersect_id_end, std::vector<unsigned>& path);

// Returns the time required to travel along the path specified. The path
// is passed in as a vector of street segment ids, and this function can 
// assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus 15 seconds per turn implied by the path. A turn occurs
// when two consecutive street segments have different street names.
double compute_path_travel_time(const std::vector<unsigned>& path);
double compute_path_travel_time(const std::list<unsigned>& path);


// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// If no such path exists, returns an empty (size == 0) vector.
std::vector<unsigned> find_path_to_point_of_interest (unsigned 
                   intersect_id_start, std::string point_of_interest_name);

// Given a vector of street segment ID's corresponding to a path, returns a 
// series of directives as a vector of strings 
std::vector<std::string> get_path_directives(std::vector<unsigned> path);

// determines the common intersection between two street segments
inline unsigned inCommon(unsigned segment1, unsigned segment2){
    StreetSegmentInfo info1 = getStreetSegmentInfo(segment1);
    StreetSegmentInfo info2 = getStreetSegmentInfo(segment2);
    
    if(info1.to == info2.to || info1.to == info2.from)
        return info1.to;
    return info1.from;
}

// Given a path vector (vector of street segment ID's) a 2D vector with the 
// street segments sorted by streets
TwoDimVector sortPath(std::vector<unsigned> path);

// Returns a turn directive when turning from previousSegment onto next segment
std::string turnDirective(unsigned const& previousSegment, unsigned const& nextSegment);

// Tells the driver how long to continue on a street
std::string straightDirective(std::vector<unsigned> currentSegments);

/************Functions are changed to handle multi-threading*******************/

//mt stands for multi-thread
//choose_data determines which data-structure to take from the vector
//of data-structures

std::vector<unsigned> mt_find_path_between_intersections(unsigned intersect_id_start, 
                                                         unsigned intersect_id_end, 
                                                         unsigned choose_data);
/******************************************************************************/
