#include "GUI_Path_Box.h"
#include "Global_var.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"

PathBox::PathBox(){
    draw_box = false;
    draw_path = true;
    to_box_pressed = false;
    from_box_pressed = false;
    clear_on_click_from_box = false;
    clear_on_click_to_box = false;;
}

void PathBox::draw_path_box(){
    if(draw_box){
        to_box.draw_input_box(0, -0.04);
        from_box.draw_input_box(0, 0);

        draw_cross();
        draw_go();

        if(to_box_pressed){
            to_box.draw_suggestions_box(0, -0.04);
        }
        else if(from_box_pressed){
            from_box.draw_suggestions_box(0, -0.04);
        }
    }
}

void PathBox::pathbox_on_click_handler(float x, float y){
    if(draw_box){
        if(within_bounds(go_button_boundries, x, y)){
            if(to_box.get_input().size()   == 0 ||
               from_box.get_input().size() == 0){
                //temporary solution;
                if(to_box.get_input().size() == 0){
                    to_box.set_input("Please fill in with intersection or poi");
                    clear_on_click_to_box = true;
                }
                if(from_box.get_input().size() == 0){
                    from_box.set_input("Please fill in with intersection");
                    clear_on_click_from_box = true;
                }
            }
            else{
                find_path(to_box.get_input(), from_box.get_input());
            }
        }
        else if(within_bounds(close_button_boundries, x, y)){
            draw_box = false;
            draw_searchbox();
            from_box.clear_input();
            to_box.clear_input();
        }
        else{
            if(within_bounds(to_box.get_window_boundries(), x, y)){
                to_box_pressed = true;
                if(clear_on_click_to_box){
                    to_box.set_input("");
                    clear_on_click_to_box = false;
                }
            }
            else{
                to_box_pressed = false;
            }
            if(within_bounds(from_box.get_window_boundries(), x, y)){
                from_box_pressed = true;
                if(clear_on_click_from_box){
                    from_box.set_input("");
                    clear_on_click_from_box = false;
                }
            }
            else{
                from_box_pressed = false;
            }
            
            to_box.autocomplete_onclick_handler(x, y);
            from_box.autocomplete_onclick_handler(x, y);
        }
    }
    draw_screen();
}

void PathBox::draw_cross(){
    
    t_bound_box window = from_box.get_window_boundries();
    
    close_button_boundries = t_bound_box(window.top_right().x-window.get_width()*1/16, //most right
                                         window.top_right().y-window.get_height()*10.5/11, //most low
                                         window.top_right().x, //least right
                                         window.top_right().y-window.get_height()*0.02/11); //most high
    
    setcolor(RED);
    fillrect(close_button_boundries);
    
    setlinewidth(2);
    setcolor(WHITE);
    drawline(close_button_boundries.bottom_left().x+close_button_boundries.get_width()*1/4,
            close_button_boundries.bottom_left().y+close_button_boundries.get_height()*1/4,
            close_button_boundries.bottom_left().x+close_button_boundries.get_width()*3/4,
            close_button_boundries.bottom_left().y+close_button_boundries.get_height()*3/4);
    
    drawline(close_button_boundries.bottom_left().x+close_button_boundries.get_width()*1/4,
            close_button_boundries.bottom_left().y+close_button_boundries.get_height()*3/4,
            close_button_boundries.bottom_left().x+close_button_boundries.get_width()*3/4,
            close_button_boundries.bottom_left().y+close_button_boundries.get_height()*1/4);
}

void PathBox::user_input(char c, int keysym){
    if(to_box_pressed){
        to_box.user_input(c, keysym);
    }
    else if(from_box_pressed){
        from_box.user_input(c, keysym);
    }
}

void PathBox::draw_go(){

    t_bound_box window = to_box.get_window_boundries();
    
    go_button_boundries = t_bound_box(window.top_right().x-window.get_width()*1/16, //most right
                                      window.top_right().y-window.get_height()*10.5/11, //most low
                                      window.top_right().x, //least right
                                      window.top_right().y-window.get_height()*0.02/11);
    
    setcolor(GREEN);
    fillrect(go_button_boundries);
    setcolor(WHITE);
    drawtext(go_button_boundries.get_center(), "GO", window, 1);
}

bool PathBox::within_bounds(t_bound_box box, float x, float y){
    //xHigh, yHigh refer to the highest x and y values
    //xLow, yLow refer to the lowest x and y values
    //although it might seem that I have them inverted xHigh, xLow, yHigh, yLow
    //are based from the user seeing the screen ie) yHigh would be the highest
    //part of the screen
    float xHigh = abs(box.left());
    float yHigh = abs(box.bottom());
    float xLow = abs(box.right());
    float yLow = abs(box.top());
    x = abs(x);
    y = abs(y);
    
    if(xLow <= x && x <= xHigh && y >= yLow && y <= yHigh)
        return true;
    else
        return false;
}

void PathBox::find_path(string to_box_input, string from_box_input){
    foundPath = determine_type_of_name(to_box_input, from_box_input);
    //directives has the path ready
    foundDirectives = get_path_directives(foundPath);
    
    draw_screen();
}

void PathBox::enable_path_box(){
    draw_box = true;
}

void PathBox::draw_searchbox(){
    pathBoxSearchBoxSwitch = true;
}

bool PathBox::clickInside(float x, float y){
    return to_box.clickedInside(x,y)||from_box.clickedInside(x,y);
}

vector<string> PathBox::parse(string input){
    //names can hold either poi names or intersection names

    //bool is_intersection = false;
    
    vector<string> names;
    
    //temporary variable to hold the string
    string temp;
    
    //clear garbage values
    for(unsigned i = 0; i < input.size(); i++){
        //numbers are based off of ascii values
        if(input[i] < 32 || input[i] > 126){
            input.erase(i, 1);
        }
    }
    
    stringstream ss;
    ss << input;
    
    //split the string into multiple strings
    while(ss){
        ss >> temp;
        names.push_back(temp);
    }
    //string stream copies the last string twice
    names.pop_back();

    return names;
}

vector<string> PathBox::combine_names(vector<string> parsed_string){
    //will return either a street name or a poi name
    bool possible_intersection = false;
    vector<string> name;
    name.resize(2);
    
    auto iterator = parsed_string.begin();
    for(; iterator != parsed_string.end(); iterator++){
        //cout << *iterator << endl;
        if(*iterator == "&" || *iterator == "@"){
            possible_intersection = true;
            break;
        }
        name[0] += (*iterator + " ");
    }
    //get rid of the last space
    name[0].pop_back();
    
    if(possible_intersection){
        //skip past the & or @
        iterator++;
        //get the rest of the string
        for(; iterator != parsed_string.end() ; iterator++){
            if(*iterator == "&" || *iterator == "@")
                break;
            name[1] += (*iterator + " ");
        }
        //delete the last space
        name[1].pop_back();
        return name;
    }
    else{
        //only one name to take 
        name.pop_back();
        return name;
    } 
}

//input from the TO box could either be an poi or intersection
//form box must be an intersection
vector<unsigned> PathBox::determine_type_of_name(string to_box_input, string from_box_input){
    
    //contains the path of what intersections to take in order to get from point a
    //to point b
    vector<unsigned> path;
    
    vector<string> parsed_to_box_input;
    vector<string> parsed_from_box_input;
    
    parsed_to_box_input   = parse(to_box_input);
    parsed_from_box_input = parse(from_box_input);
    
    parsed_to_box_input   = combine_names(parsed_to_box_input);
    parsed_from_box_input = combine_names(parsed_from_box_input);
    
    //check errors
    if(parsed_from_box_input.size() < 2){
        from_box.set_input("Not an Intersection");
        clear_on_click_from_box = true;
        //empty
        return path;
    }

    //check if possible poi
    if(parsed_to_box_input.size() == 1){
        
        vector<unsigned> intersect_id_from = 
        find_intersection_ids_from_street_names(parsed_from_box_input[0], parsed_from_box_input[1]);
        if(intersect_id_from.size() < 1){
            from_box.set_input("Invalid Intersection");
            clear_on_click_from_box = true;
        }
        else{
            std::vector<unsigned> POI_IDs;    
            auto range = nameToPoiID.equal_range(parsed_to_box_input[0]);
            for (auto it = range.first; it != range.second; ++it) {
                POI_IDs.push_back(it->second);
            }  
            if(POI_IDs.size() == 0){
                //errors happen here
                to_box.set_input("Not a POI");
                clear_on_click_to_box = true;
            }
            else
                path = find_path_to_point_of_interest(intersect_id_from[0], parsed_to_box_input[0]);
        }
        
        if(path.size() == 0){
            //errors happen here
            to_box.set_input("POI is on the same intersection");
            clear_on_click_to_box = true;
        }
        else {
            intersectionFrom=intersect_id_from[0];
            POI_Name=parsed_to_box_input[0];
        }
    }
    //check if possible intersection
    else if(parsed_to_box_input.size() == 2){
        vector<unsigned> intersect_id_to   = 
        find_intersection_ids_from_street_names(parsed_to_box_input[0], parsed_to_box_input[1]);
        if(intersect_id_to.size()==0){
            to_box.set_input("Not an Intersection");
            clear_on_click_to_box = true;
            return path;
        }
        
        vector<unsigned> intersect_id_from = 
        find_intersection_ids_from_street_names(parsed_from_box_input[0], parsed_from_box_input[1]); 
        if(intersect_id_from.size() == 0){
            from_box.set_input("Not an Intersection");
            clear_on_click_from_box = true;
            return path;
        }
        
        //default to be the first path for now
        path = find_path_between_intersections(intersect_id_from[0], intersect_id_to[0]);
        
        //intersection does not exist
        if(path.size() == 0){
            //errors happen here
            to_box.set_input("No path found");
            from_box.set_input("No path found");
            clear_on_click_from_box = true;
            clear_on_click_to_box = true;
        }
        else{
            intersectionFrom=intersect_id_from[0];
            intersectionTo=intersect_id_to[0];
            POITo=std::numeric_limits<unsigned>::infinity();
        }
    }
    return path;
}
