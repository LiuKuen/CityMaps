/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "B_Point2D.h"
#include "Global_var.h"

Point2D::Point2D(LatLon geo){
        x = geo.lon *  cos(AVG_LAT*DEG_TO_RAD);
        y = geo.lat;
    }
    
Point2D::Point2D(float _x, float _y){
        x = _x;
        y = _y;
    }
LatLon Point2D::get_latlon(){
        LatLon result(y,x/cos(AVG_LAT * DEG_TO_RAD));
        return result;
    }
   

