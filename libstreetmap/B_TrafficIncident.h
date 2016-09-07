/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TrafficIncident.h
 * Author: osboxes
 *
 * Created on 17 February 2016, 04:15
 */

#ifndef TRAFFICINCIDENT_H
#define TRAFFICINCIDENT_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "LatLon.h"
#include "graphics.h"
#include <vector>
#include "B_Point2D.h"

class TrafficIncident {
private:
    std::string description;
    std::string severity;
    std::string type;
    
    bool roadClosed;
    
    LatLon point;
    LatLon toPoint;
    
    unsigned ID;
public:
    TrafficIncident(std::string, unsigned _ID);
    void print();
    void draw();
    
    LatLon getPos(){return point;}
    unsigned getID(){return ID;}
    std::string getMessage(){return (type + ": " + description);}
};

void set_color(int type);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

#endif /* TRAFFICINCIDENT_H */

