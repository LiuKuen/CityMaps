/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Point2D.h
 * Author: pahlavil
 *
 * Created on February 18, 2016, 3:40 PM
 */

#ifndef POINT2D_H
#define POINT2D_H

#include "Constants.h"
#include "LatLon.h"

extern double AVG_LAT;

struct Point2D{
    float x;
    float y;
   
    // Average latitude calculated during load_map
    // static float AverageMapLat;
    
    // Constructor takes coordinates in lat/lon and 
    // returns approximate (x,y) coordinates
    Point2D(){}
    
    
    Point2D(const Point2D& rhs){
        x = rhs.x;
        y = rhs.y;
    }
    
    Point2D(LatLon geo);
    
    Point2D(float _x, float _y);
    
    LatLon get_latlon();
    
    Point2D operator+(const Point2D& v1){
        Point2D result;
        result.x=this->x+v1.x;
        result.y=this->y+v1.y;
        return result;
    }
    
    Point2D operator-(const Point2D& v1){
        Point2D result;
        result.x=this->x-v1.x;
        result.y=this->y-v1.y;
        return result;
    }
    
    Point2D operator/(float i){
        Point2D result;
        result.x=this->x/i;
        result.y=this->y/i;
        return result;
    }
    
    Point2D operator*(float i){
        Point2D result;
        result.x=this->x*i;
        result.y=this->y*i;
        return result;
    }
    
    // Dot product
    double operator*(const Point2D& rhs){
        return x*rhs.x + y*rhs.y;
    }
    
    // Return vector magnitude
    double mag(){
        return std::sqrt(x*x+y*y);
    }
};

#endif /* POINT2D_H */

