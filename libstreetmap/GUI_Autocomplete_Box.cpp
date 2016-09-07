#include <m2.h>
#include <X11/Xlib.h>
#include "Global_var.h"
#include "GUI_Autocomplete_cell.h"

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

AutocompleteBox::AutocompleteBox(){
    pressed = false;
    autoCompletePressed = false;
    currentHighlighting=0;
}

void AutocompleteBox::draw_autocomplete_box(float input_offset_x, float input_offset_y, 
        float suggestions_offset_x, float suggestions_offset_y){
    
    draw_input_box(input_offset_x, input_offset_y);

    if(pressed){
        draw_suggestions_box(suggestions_offset_x, suggestions_offset_y);
    }
}

//basically the white part of the box
void AutocompleteBox::draw_input_box(float offset_x, float offset_y){
    t_bound_box current_view = get_visible_world();
    
    float y_offset = current_view.get_height()*offset_y;
    float x_offset = current_view.get_width()*offset_x;
    
    //hard coded for now we can generalize this for a more powerful implementation
    window = t_bound_box(current_view.bottom_left().x+current_view.get_width()/100+x_offset,
                         current_view.bottom_left().y+current_view.get_height()*59/60+y_offset,
                         current_view.bottom_left().x+current_view.get_width()*2/3+x_offset,
                         current_view.bottom_left().y+current_view.get_height()*57/60+y_offset);
    
    setcolor(WHITE);
    fillrect(window);
    
    if(!pressed){
        setcolor(DARKGREY);
    }
    else
        setcolor(FIREBRICK);
    
    setlinewidth(1);
    drawrect(window);
    
    setcolor(BLACK);
    setfontsize(9);
    
    drawtext(window.get_center(), input, window, 1);
}

//draws the suggestions from a given string
void AutocompleteBox::draw_suggestions_box(float offset_x, float offset_y){
    int numberOfQueries = searchBoxAutoComplete.numberOfQueries();
    
    if(numberOfQueries > 4){
        numberOfQueries = 4;
    }
    if(numberOfQueries > 0){
        t_bound_box current_view = get_visible_world();

        //initialize the boxes
        //fucking clear the shit
        listOfQueries.clear();

        float y_offset = current_view.get_height()*offset_y;
        float x_offset = current_view.get_width()*offset_x;

        for(int i = 0; i < numberOfQueries; i++){
            t_bound_box autocompleteWindow(current_view.bottom_left().x+current_view.get_width()/100+x_offset,
                                           current_view.bottom_left().y+current_view.get_height()*(59 - (i+1.17)*2)/60+y_offset,
                                           current_view.bottom_left().x+current_view.get_width()*2/3+x_offset,
                                           current_view.bottom_left().y+current_view.get_height()*(57 - (i+1.17)*2)/60+y_offset);

            AutocompleteCell cell(autocompleteWindow, searchBoxAutoComplete.getQueriesAt(i));
            listOfQueries.push_back(cell);
        }

        //draw the boxes
        for(int i = 0; i < numberOfQueries; i++){
             setcolor(WHITE);
             fillrect(listOfQueries[i].getWindow());
             setcolor(BLACK);
             setfontsize(9);
             drawtext(listOfQueries[i].getWindow().get_center(),
                      listOfQueries[i].getQuery(), 
                      listOfQueries[i].getWindow(),
                      1);
        }
    }
    if(currentHighlighting>=numberOfQueries)
        currentHighlighting=numberOfQueries;
    
    // Highlight one box
    if(currentHighlighting!=0){
        setlinewidth(3);
        setcolor(LIGHTSKYBLUE);
        drawrect(listOfQueries[currentHighlighting-1].getWindow());
    }
}

void AutocompleteBox::autocomplete_window_handler(float x, float y){
    for(unsigned i = 0; i < listOfQueries.size(); i++){
        if(within_bounds(listOfQueries[i].getWindow(), x, y) &&
           searchBoxAutoComplete.numberOfQueries() != 0){
            input = listOfQueries[i].getQuery();
        }
    }
}

bool AutocompleteBox::within_bounds(t_bound_box box, float x, float y){
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

void AutocompleteBox::autocomplete_onclick_handler(float x, float y){
    if(pressed){
        autocomplete_window_handler(x, y);
    }
    
    if(within_bounds(window, x, y)){
        pressed = true;
        //possible cursor implementation later
    }
    else
        pressed = false;
    
    draw_screen();
}

void AutocompleteBox::user_input(char c, int keysym){
    if(pressed){
        user_input_handler(c, keysym, input);
    }
    draw_screen();
}

void AutocompleteBox::user_input_handler(char c, int keysym, string& input){
    
    if (keysym == XK_BackSpace) {
            if (!input.empty())
                input.pop_back();
            else
                return;
    }
    else if(keysym==XK_Up||keysym==XK_Down)
        set_currentHighlighting(keysym);
    
    else if((c<='z' && c>='a')||(c<='Z'&&c>='A') ||c==' ' || (c<='9'&&c>='0')
            || c=='@'||c=='&' || c=='\''){
            input += c;
    }
    //findWindowAutocomplete is a global variable... rip
    searchBoxAutoComplete.updateQueries(namesDatabase, keysym, input);
    searchBoxAutoComplete.autoCompleteString(keysym, input,currentHighlighting);
    
    return;
}

t_bound_box AutocompleteBox::get_window_boundries(){
    return window;
}

string AutocompleteBox::get_input(){
    return input;
}

bool AutocompleteBox::clickedInside(float x, float y){
    bool inside=false; 
    for(unsigned i=0;i<this->listOfQueries.size();i++)
        inside=inside || within_bounds(listOfQueries[i].getWindow(),x,y);
    
    return inside || within_bounds(window,x,y);
}
void AutocompleteBox::set_currentHighlighting(int keysym){
    
    if(keysym==XK_Up && currentHighlighting>1)
        currentHighlighting--;
        
    else if(keysym==XK_Down && currentHighlighting<(int)listOfQueries.size())
        currentHighlighting++;

}

void AutocompleteBox::set_input(string message){
    input = message;
}