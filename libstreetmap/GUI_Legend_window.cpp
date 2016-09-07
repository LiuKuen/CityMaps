#include <m2.h>

void draw_legend(){
    
    const float icon_offset = 10;
    const float text_offset = 5;
    
    t_bound_box current_view = get_visible_world();
    
    //create bounds for outer window
    window_size = t_bound_box(current_view.bottom_left().x+current_view.get_width()/3,
                current_view.bottom_left().y+current_view.get_height()/3.5,
                current_view.bottom_left().x+current_view.get_width()*2/3,
                current_view.bottom_left().y+current_view.get_height()*5/6);
    
    //create bounds for draw closed box
    close_box_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*18.5/20,
                window_size.bottom_left().y+window_size.get_height()*19.25/20,
                window_size.bottom_left().x+window_size.get_width(),
                window_size.bottom_left().y+window_size.get_height());
    
    //draw outer window
    setcolor(WHITE);
    fillrect(window_size);
    
    setcolor(DARKGREY);
    setlinewidth(1);
    drawrect(window_size);
    
    //draw close box
    setcolor(RED);
    fillrect(close_box_size);
    
    //draw 'x' for close box
    setlinewidth(2);
    setcolor(WHITE);
    settextrotation(0);
    
    drawline(close_box_size.bottom_left().x+close_box_size.get_width()*1/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*1/4,
            close_box_size.bottom_left().x+close_box_size.get_width()*3/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*3/4);
    
    drawline(close_box_size.bottom_left().x+close_box_size.get_width()*1/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*3/4,
            close_box_size.bottom_left().x+close_box_size.get_width()*3/4,
            close_box_size.bottom_left().y+close_box_size.get_height()*1/4);

    //specify points to draw
    Point2D shopIcon(window_size.bottom_left().x+window_size.get_width()/icon_offset,
            window_size.bottom_left().y+window_size.get_height()/7);
    
    Point2D hospitalIcon(window_size.bottom_left().x+window_size.get_width()/icon_offset,
            window_size.bottom_left().y+window_size.get_height()*2/7);
    
    Point2D collegeIcon(window_size.bottom_left().x+window_size.get_width()/icon_offset,
            window_size.bottom_left().y+window_size.get_height()*3/7);
    
    Point2D bankIcon(window_size.bottom_left().x+window_size.get_width()/icon_offset,
            window_size.bottom_left().y+window_size.get_height()*4/7);
    
    Point2D libraryIcon(window_size.bottom_left().x+window_size.get_width()/icon_offset,
            window_size.bottom_left().y+window_size.get_height()*5/7);
    
    Point2D warningIcon(window_size.bottom_left().x+window_size.get_width()/icon_offset,
            window_size.bottom_left().y+window_size.get_height()*6/7);
    
    
    //draw title
    setfontsize(20);
    set_color(Black);
    drawtext(window_size.bottom_left().x+window_size.get_width()/2, 
             window_size.bottom_left().y+window_size.get_height()/1.05, "LEGEND", 1, 1);
             
    setfontsize(9);
    draw_outline(shopIcon);
    set_color(SHOP_C);
    draw_shop_icon(shopIcon);
    set_color(Black);
    setfontsize(12);
    drawtext(shopIcon.x + current_view.get_width()/text_offset, shopIcon.y, "Retail", 1, 1);

    draw_outline(hospitalIcon);
    set_color(HOSPITAL_NC);
    draw_hospital_icon(hospitalIcon);
    set_color(Black);
    setfontsize(12);
    drawtext(shopIcon.x + current_view.get_width()/text_offset, hospitalIcon.y, "Hospital", 1, 1);

    draw_outline(collegeIcon);
    set_color(UNIVERSITY_NC);
    draw_college_icon(collegeIcon);
    set_color(Black);
    setfontsize(12);
    drawtext(shopIcon.x + current_view.get_width()/text_offset, collegeIcon.y, "University", 1, 1);

    draw_outline(bankIcon);
    set_color(BANK_C);
    draw_bank_icon(bankIcon);
    set_color(Black);
    setfontsize(12);
    drawtext(shopIcon.x + current_view.get_width()/text_offset, bankIcon.y, "Bank", 1, 1);

    draw_outline(libraryIcon);
    set_color(LIBRARY_C);
    draw_library_icon(libraryIcon);
    set_color(Black);
    setfontsize(12);
    drawtext(shopIcon.x + current_view.get_width()/text_offset, libraryIcon.y, "Library", 1, 1);
    
    draw_warning_icon(warningIcon);
    set_color(Black);
    setfontsize(12);
    drawtext(shopIcon.x + current_view.get_width()/text_offset, warningIcon.y, "Traffic Accident", 1, 1);
}

void mouse_press_legend_window_handler(float x, float y, t_event_buttonPressed event){
    if(inside_box(x,y,close_box_size)){
        legendButtonPressed = false;
        draw_screen();
    }
}

