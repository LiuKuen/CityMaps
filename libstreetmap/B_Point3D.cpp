/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Point3D.cpp
 * Author: pahlavil
 * 
 * Created on January 20, 2016, 2:55 PM
 */

#include "B_Point3D.h"
#include "KeyValueTable.hpp"
#include "Constants.h"
#include <cmath>
    
//default constructor
Point3D::Point3D(){}

// Constructor that takes in a position in LatLon coordinates and converts 
// them to 3D cartesian coordinates. The cartesian coordinates are only used 
// for minimizing distance and do not represent actual physical distances 
// between points.
Point3D::Point3D(LatLon pos_, unsigned key_){
    pos[0] = (cos(pos_.lat*DEG_TO_RAD) * cos(pos_.lon*DEG_TO_RAD));
    pos[1] = (cos(pos_.lat*DEG_TO_RAD) * sin(pos_.lon*DEG_TO_RAD));
    pos[2] = (sin(pos_.lat*DEG_TO_RAD));
    key = key_;
}

// Returns LatLon of actual position
LatLon Point3D::get_latlon(){
    float lat = std::asin(pos[2])/DEG_TO_RAD;
    float lon = std::acos(pos[0]/cos(lat*DEG_TO_RAD))/DEG_TO_RAD;
    
    // Take into account the fact that acos is 
    // defined for pi/2<theta<pi/2
    if(pos[1]<0) lon *= -1;
    
    return LatLon(lat, lon);
}
    
Point3D::value_type Point3D::operator[](size_t n) const{
    return pos[n];
}

void Point3D::operator=(Point3D rhs){
    pos[0] = rhs.pos[0];
    pos[1] = rhs.pos[1];
    pos[2] = rhs.pos[2];
    
    key    = rhs.key;
}

// check x1==x2 && y1==y2 && z1==z2
bool Point3D::operator==(Point3D rhs){
    return (rhs.pos[0]==pos[0] && rhs.pos[1]==pos[1] && rhs.pos[2]==pos[2]);
}
