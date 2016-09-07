#include "GUI_Search_Box.h"
#include "m1.h"
#include "m2.h"
#include "Global_var.h"

const float PI = 3.14159265359;

SearchBox::SearchBox(){
    clear_on_click = false;
    draw_found_intersection_icon = false;
    draw_box = true;
}

void SearchBox::searchbox_onclick_handler(float x, float y){
    
    if(within_bounds(magnifying_glass_boundires, x, y)){
        parse_user_input();
    }
    else if(within_bounds(direction_icon_boundries, x, y)){
        draw_box = false;
        pathBar.getFromBox()->set_input(get_input());
        draw_pathbox();
    }
    if(draw_box){
        if(within_bounds(window, x, y) && 
          !within_bounds(magnifying_glass_boundires, x, y) && 
          !within_bounds(direction_icon_boundries, x, y)){
            if(clear_on_click){
                input = "";
                clear_on_click = false; 
            }
        }
        autocomplete_onclick_handler(x, y);
    }
    
    draw_screen();
}

void SearchBox::draw_search_box(){
    if(draw_box){
        if(draw_found_intersection_icon){
            for(unsigned i = 0; i < intersectionLatLon.size(); i++){
                highlight_intersection(Point2D(intersectionLatLon[i]));
            }
        }
        draw_autocomplete_box(0,0,0,0);
        draw_magnifying_glass();
        draw_direction_icon();
    }
}

void SearchBox::draw_magnifying_glass(){
    magnifying_glass_boundires = t_bound_box(window.top_right().x-window.get_width()*1/16, //most right
                                             window.top_right().y-window.get_height()*10/11, //most low
                                             window.top_right().x, //least right
                                             window.top_right().y-window.get_height()*2/11); //most high
     
    
    float radius = magnifying_glass_boundires.get_width()*1/4;
    float center_y_offset = magnifying_glass_boundires.get_height()*1/10;
    float x_center = magnifying_glass_boundires.get_center().x;
    float y_center = magnifying_glass_boundires.get_center().y + center_y_offset;
    
    setcolor(WHITE);
    fillrect(magnifying_glass_boundires);
    
    setcolor(DARKGREY);
    fillarc(x_center,
            y_center,
            radius*9/10,
            0,
            360);
    setcolor(WHITE);
    fillarc(x_center,
            y_center,
            radius*8/10,
            0,
            360);
    
    //mathmatically they are the same!
    float base_point_x_y = radius*cos(PI/4);
    
    t_point top_left((x_center+base_point_x_y*6/10), 
                     (y_center-base_point_x_y*10/10));
    t_point top_right((x_center+base_point_x_y*10/10),
                      (y_center-base_point_x_y*6/10));
    t_point bottom_left((x_center+base_point_x_y*10/10), 
                        (y_center-base_point_x_y*14/10));
    t_point bottom_right((x_center+base_point_x_y*14/10), 
                         (y_center-base_point_x_y*10/10));
            
    t_point points[4] = {top_left, top_right, bottom_right, bottom_left};
    int number_of_points = 4;
    setcolor(DARKGREY);   
    fillpoly(points, number_of_points);
}

void SearchBox::draw_direction_icon(){
    direction_icon_boundries = t_bound_box(window.top_right().x-window.get_width()*2/16,
                                           window.top_right().y-window.get_height()*10/11,
                                           window.top_right().x-window.get_width()*1/16,
                                           window.top_right().y-window.get_height()*2/11);
    
    setcolor(WHITE);
    fillrect(direction_icon_boundries);
    
    //for reference of the icon I am drawing points start from the bottom left
    //and go clock wise
    //     .
    //...... .
    //.        .
    //. .... .
    //. .  . 
    //...   
    float center_x = direction_icon_boundries.get_xcenter();
    float center_y = direction_icon_boundries.get_ycenter();
    float width = direction_icon_boundries.get_width();
    float height = direction_icon_boundries.get_height();
    
    t_point one     (center_x - width*2/5, center_y + height*2/5);
    t_point two     (center_x - width*2/5, center_y - height*1/5);
    t_point three   (center_x            , center_y - height*1/5);
    t_point four    (center_x            , center_y - height*2/5);
    t_point five    (center_x + width*2/5, center_y - height*0.5/5);
    t_point six     (center_x            , center_y + height*1/5);
    t_point seven   (center_x            , center_y);
    t_point eight   (center_x - width*1/4, center_y);
    t_point nine    (center_x - width*1/4, center_y + height*2/5);
    
           
    setcolor(DARKGREY);
    t_point points[9] = {one, two, three, four, five, six, seven, eight, nine};
    int number_of_points = 9;
    fillpoly(points, number_of_points);
    
    
}

void SearchBox::parse_user_input(){

    string name_one;
    string temp;
    //symbol could be & or @ to signify that they want to find the intersection
    //between name_one or name_two
    string symbol;
    string name_two;
            
    stringstream ss;
    //clear garbage in inputs
    for(unsigned i = 0; i < input.size(); i++){
        if(input[i] < 32 || input[i] > 126){
            input.erase(i, 1);
        }
    }
    
    ss << input;
    
    //get first street name from string
    while(ss){
        ss >> temp;
        if(temp == "&" || temp == "@"){
            symbol = temp;
            break;
        }
        name_one +=temp;
        name_one += " ";
    }
    //take out the last space
    if(name_one.size() != 0)
        name_one.pop_back();
    else{
         clear_on_click = true;
         input = "invalid intersection";
    }
    
    temp.clear();
    
    //get last street name from string
    while(ss){
        ss >> temp;
        name_two += temp;
        name_two += " ";
    }
    //clear name of last word
    //for some reason ss copies the last word twice
    if(name_two.size() >= temp.size() + 2){
        for(unsigned i = 0; i < temp.size(); i++){
            name_two.pop_back();
        }
        name_two.pop_back();
        name_two.pop_back();
    }
    else{
         clear_on_click = true;
         input = "invalid intersection";
    }
    
    find_intersection(name_one, symbol, name_two);
}

void SearchBox::find_intersection(string name_one, string symbol, string name_two){
    clear_on_click = true;
    
    if(symbol == "&" || symbol == "@"){
        std::vector<unsigned> intersection_id = find_intersection_ids_from_street_names(name_one,name_two);
        if(intersection_id.size() == 0){
            input = "Intersection not found";
        }
        else{
            draw_found_intersection_icon = true;
            for(unsigned i = 0; i < intersection_id.size(); i++){
                intersectionLatLon.push_back(getIntersectionPosition(intersection_id[i]));
                listOfQueries.clear();
            }
        }
    }
}

void SearchBox::highlight_intersection(Point2D intersection)
{
    t_bound_box current_view = get_visible_world();

    t_point point1(intersection.x - current_view.get_height() / 70, intersection.y - current_view.get_height() / 80 + current_view.get_height() / 25);
    t_point point2(intersection.x + current_view.get_height() / 70, intersection.y - current_view.get_height() / 80 + current_view.get_height() / 25);
    t_point point3(intersection.x, intersection.y);

    t_point pointarr[] = {point1, point2, point3};

    setcolor(RED);
    fillpoly(pointarr, 3);

    fillarc(intersection.x, intersection.y + current_view.get_height() / 25, current_view.get_height() / 50, 0, 360);

    setcolor(BLACK);
    fillarc(intersection.x, intersection.y + current_view.get_height() / 25, current_view.get_height() / 100, 0, 360);
}

void SearchBox::clear_intersection_icons(){
    draw_found_intersection_icon = false;
    intersectionLatLon.clear();
}

void SearchBox::enable_search_box(){
    draw_box = true;
}

void SearchBox::draw_pathbox(){
    pathBoxSearchBoxSwitch = false;
}