/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Point2D.h
 * Author: pahlavil
 *
 * Created on January 20, 2016, 2:54 PM
 */

#ifndef POINT3D_H
#define POINT3D_H

#include <ostream>
#include <iostream>
#include <cmath>
#include <vector>
#include "LatLon.h"

/*   
 * This class acts as a node for the k-d tree library (kdlibtree++).
 * It holds 3D points.
 * 
 * Overview:
 *  The reason data is stored in 3D is that it is easier to calculate the distance 
 *  between two points this way. In order to calculate accurate enough distances 
 *  directly in spherical (lat/lon) coordinates, the source code must be changed.
 * 
 *  The reduction in efficiency due to the extra dimension is negligible for the
 *  current use.
 * 
 *  The distance calculated between two points in the find_nearest algorithm is 
 *  the direct distance (i.e. not arc length) but minimizing this distance gives 
 *  the same results as minimizing arc length. Following the same principle, the
 *  radius of the Eartn is irrelevant to minimizing distance.
 * 
 *  Each node has a key which represents an OSM ID (street ID or point of 
 *  interest ID).
 * 
 * Required members for proper use:
 *  Since the KDTree::KDTree class is a container class, it requires a class or
 *  struct to use as a node.
 * 
 *  For proper use it requires only two things:
 *      a value_type typedef to identify what type the coordinates are in.
 *      a "typedef operator[]()" to access the nth dimensional coordinates.
 * 
 */

class Point3D{
    // Position in 3D cartesian coordinates
    double pos[3];
    
    // OSM ID (street_id or point_of_interest_id)
    unsigned key = -1U; //holds intersection or POI ID
public:
    size_t index;
    
    // Required for kdlibtree++ (identifies what type the coordinates are in)
    typedef double value_type;
    
    //default constructor
    Point3D();
    
    // Constructor that takes in a position in LatLon coordinates and converts 
    // them to 3D cartesian coordinates. The cartesian coordinates are only used 
    // for minimizing distance and do not represent actual physical distances 
    // between points.
    Point3D(LatLon pos_, unsigned key_);
    
    // Returns LatLon of actual position
    LatLon get_latlon();
    
    // Used by the KDTree library to access x, y and z coordinates
    value_type operator[](size_t n) const;
    
    // Key accessor function
    unsigned getKey() const{return key;}
    
    // Operators used for testing  
    void operator=(Point3D rhs);    
    bool operator==(Point3D rhs);
    
    // Mainly for debugging
    void print(){
        std::cout << "(x,y,z) = (" << pos[0] << "," 
                << pos[1] << "," << pos[2] << ")" << std::endl;
        std::cout << "Street ID: " << key << std::endl;
    }
};

#endif /* POINT3D_H */

