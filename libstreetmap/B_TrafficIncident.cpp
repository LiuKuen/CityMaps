/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TrafficIncident.cpp
 * Author: osboxes
 * 
 * Created on 17 February 2016, 04:15
 */

#include "B_TrafficIncident.h"

void set_color(int type){
    t_color color;
    //take the first two digits
    color.blue = type % 256;
    
    //take the second two digits
    type /= 256;
    color.green = type % 256;
    
    //take the last two digits
    type /= 256;
    color.red = type % 256;
    
    setcolor(color);
}

TrafficIncident::TrafficIncident(std::string line, unsigned _ID){
    std::vector<std::string> x = split(line, ' ');
    
    ID = _ID;
    
    type=x[0];
    severity=x[1];
    roadClosed=x[2].compare("false");
    
    float lat, lon;
    lat=std::stof(x[3],NULL);
    lon=std::stof(x[4],NULL);
    point = LatLon(lat, lon);
    
    lat=std::stof(x[5],NULL);
    lon=std::stof(x[6],NULL);
    toPoint = LatLon(lat, lon);
    
    std::string temp = x[7];
    for(unsigned i=8;i<x.size();i++)
        temp=temp+" "+x[i];
    
    description=temp;
}

void TrafficIncident::draw(){
    // Get current screen width
    t_bound_box current_screen = get_visible_world();
    const float screenWidth = current_screen.get_width();
    
    // Icon width as a percentage of the screen
    const float iconWidth = 0.0075*screenWidth;
    const int numPoints = 3;
    
    // Define 3 corners of an equilateral triangle and its center
    Point2D center(point);
    
    t_point bottomLeft (center.x, center.y);
    t_point bottomRight(center.x, center.y);
    t_point top        (center.x, center.y);
    
    bottomLeft.x  -= iconWidth;
    bottomRight.x += iconWidth;
    
    bottomLeft.y  -= std::sqrt(3)/2*iconWidth;
    bottomRight.y -= std::sqrt(3)/2*iconWidth;
    top.y         += std::sqrt(3)/2*iconWidth;
    
    t_point pointArr[] = {bottomLeft, bottomRight, top}; 
    set_color(INCIDENT_COLOR);
    fillpoly(pointArr, numPoints); 
    setcolor(YELLOW);
    drawtext(center.x+iconWidth/8, center.y-iconWidth/4, "!", 10, 10);
}

void TrafficIncident::print(){
    std::cout << "********" << type << "********" << std::endl;
    std::cout << "Description: " << description << std::endl;
    std::cout << "Severity: " << severity << std::endl;
    std::cout << "roadClosed: " << roadClosed << std::endl;
    std::cout << "Point: (" << point.lat << "," << point.lon << ")" << std::endl;
    std::cout << "To Point:: (" << toPoint.lat << "," << toPoint.lon << ")" << std::endl;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}