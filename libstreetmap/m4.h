/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m4.h
 * Author: zhangba8
 *
 * Created on March 16, 2016, 2:20 PM
 */

#ifndef M4_H
#define M4_H

#pragma once
#include <vector>
#include <time.h>
#include "m3.h"
#include "GA_DistanceMatrix.h"
#include "Global_var.h"
#include "GA_Population.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"


int parseLine(char* line);
int getValue();

// This routine takes in a vector of N deliveries (pickUp, dropOff
// intersection pairs) and another vector of M intersections that
// are legal start and end points for the path. 
//
// The first vector gives the delivery information: a set of 
// pickUp/dropOff pairs of intersection ids which specify the 
// deliveries to be made. A delivery can only be dropped-off after 
// the associated item has been picked-up. 
// 
// The second vector gives the intersection
// ids of courier company depots containing trucks; you start at any
// one of these depots and end at any one of the depots.
//
// This routine returns a vector of street segment ids that form a
// path, where the first street segment id is connected to a depot
// intersection, and the last street segment id also connects to a
// depot intersection.  The path must traverse all the delivery
// intersections in an order that allows all deliveries to be made --
// i.e. a package won't be dropped off if you haven't picked it up
// yet.
//
// You can assume that N is always at least 1, and M is always at
// least one (i.e. both input vectors are non-empty).
//
// It is legal for the same intersection to appear multiple times in
// the pickUp or dropOff list (e.g. you might have two deliveries with
// a pickUp intersection id of #50). The same intersection can also
// appear as both a pickUp location and a dropOff location.
//        
// If you have two pickUps to make at an intersection, 
// traversing the intersection once is sufficient
// to pick up both packages, and similarly one traversal of an 
// intersection is sufficient to drop off all the packages that have
// already been picked up and need to be dropped off at that intersection.
//
// Depots will never appear as pickUp or dropOff locations for deliveries.
//  
// If no path connecting all the delivery locations
// and a start and end depot exists, this routine should return an
// empty (size == 0) vector.

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots);

//it is going to be important to have the information in a specific order...
//depots first, pickUps next, then dropOffs

//this function contains all the ids of depots, pickUps and dropOff locations (in that order)
void initialize_intersection_ids(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots, std::vector<unsigned>& intersection_ids);

#endif /* M4_H */

