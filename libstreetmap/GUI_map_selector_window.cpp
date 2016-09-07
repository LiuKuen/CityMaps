#include <m2.h>

void draw_map_selector_window(){
     
    t_bound_box current_view = get_visible_world();
    
    window_size = t_bound_box(current_view.bottom_left().x+current_view.get_width()/6,
                current_view.bottom_left().y+current_view.get_height()/6,
                current_view.bottom_left().x+current_view.get_width()*5/6,
                current_view.bottom_left().y+current_view.get_height()*5/6);
    
    close_box_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*19/20,
                window_size.bottom_left().y+window_size.get_height()*19/20,
                window_size.bottom_left().x+window_size.get_width(),
                window_size.bottom_left().y+window_size.get_height());
    
    setcolor(WHITE);
    fillrect(window_size);
    
    setcolor(DARKGREY);
    setlinewidth(1);
    drawrect(window_size);
    
    setcolor(RED);
    fillrect(close_box_size);
    
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
    
    map1_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*1/24,
            window_size.bottom_left().y+window_size.get_height()*13/16,
            window_size.bottom_left().x+window_size.get_width()*7/24,
            window_size.bottom_left().y+window_size.get_height()*15/16);
    
    map2_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*9/24,
            window_size.bottom_left().y+window_size.get_height()*13/16,
            window_size.bottom_left().x+window_size.get_width()*15/24,
            window_size.bottom_left().y+window_size.get_height()*15/16);
    
    map3_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*17/24,
            window_size.bottom_left().y+window_size.get_height()*13/16,
            window_size.bottom_left().x+window_size.get_width()*23/24,
            window_size.bottom_left().y+window_size.get_height()*15/16);
    
    map4_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*1/24,
            window_size.bottom_left().y+window_size.get_height()*9/16,
            window_size.bottom_left().x+window_size.get_width()*7/24,
            window_size.bottom_left().y+window_size.get_height()*11/16);
    
    map5_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*9/24,
            window_size.bottom_left().y+window_size.get_height()*9/16,
            window_size.bottom_left().x+window_size.get_width()*15/24,
            window_size.bottom_left().y+window_size.get_height()*11/16);
    
    map6_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*17/24,
            window_size.bottom_left().y+window_size.get_height()*9/16,
            window_size.bottom_left().x+window_size.get_width()*23/24,
            window_size.bottom_left().y+window_size.get_height()*11/16);
    
    map7_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*1/24,
            window_size.bottom_left().y+window_size.get_height()*5/16,
            window_size.bottom_left().x+window_size.get_width()*7/24,
            window_size.bottom_left().y+window_size.get_height()*7/16);
    
    map8_size = t_bound_box(window_size.bottom_left().x+window_size.get_width()*9/24,
            window_size.bottom_left().y+window_size.get_height()*5/16,
            window_size.bottom_left().x+window_size.get_width()*15/24,
            window_size.bottom_left().y+window_size.get_height()*7/16);
    
    ok_button_size = t_bound_box(window_size.get_center().x-window_size.get_width()*1/5,
            window_size.bottom_left().y+window_size.get_height()*1/16,
            window_size.get_center().x+window_size.get_width()*1/5,
            window_size.bottom_left().y+window_size.get_height()*3/16);
    
    setcolor(BLACK);
    drawrect(map1_size);
    drawrect(map2_size);
    drawrect(map3_size);
    drawrect(map4_size);
    drawrect(map5_size);
    drawrect(map6_size);
    drawrect(map7_size);
    drawrect(map8_size);
    
    setcolor(LIGHTSKYBLUE);
    fillrect(ok_button_size);
    
    setfontsize(9);
    setcolor(BLACK);
    drawtext(map1_size.get_center().x,map1_size.get_center().y,"TORONTO",map1_size.get_width(),1);
    drawtext(map2_size.get_center().x,map2_size.get_center().y,"TORONTO CANADA",map2_size.get_width(),1);
    drawtext(map3_size.get_center().x,map3_size.get_center().y,"NEW YORK",map3_size.get_width(),1);
    drawtext(map4_size.get_center().x,map4_size.get_center().y,"MOSCOW",map4_size.get_width(),1);
    drawtext(map5_size.get_center().x,map5_size.get_center().y,"HAMILTON CANADA",map5_size.get_width(),1);
    drawtext(map6_size.get_center().x,map6_size.get_center().y,"SAINT HELENA",map6_size.get_width(),1);
    drawtext(map7_size.get_center().x,map7_size.get_center().y,"CAIRO EGYPT",map7_size.get_width(),1);
    drawtext(map8_size.get_center().x,map8_size.get_center().y,"LONDON ENGLAND",map8_size.get_width(),1);
    
    setfontsize(12);
    drawtext(ok_button_size.get_center().x,ok_button_size.get_center().y,"OK",ok_button_size.get_width(),1);
    
    if(map_selected){
        setcolor(LIGHTSKYBLUE);
        switch(map_selected){
            case 1:
                drawrect(map1_size);
                break;
            case 2:
                drawrect(map2_size);
                break;
            case 3:
                drawrect(map3_size);
                break;
            case 4:
                drawrect(map4_size);
                break;
            case 5:
                drawrect(map5_size);
                break;
            case 6:
                drawrect(map6_size);
                break;
            case 7:
                drawrect(map7_size);
                break;
            case 8:
                drawrect(map8_size);
                break;
            default:
            assert(("This line should never be executed!",1==0));
                
        }
    }
    
}

void mouse_press_map_selector_window_handler (float x, float y, t_event_buttonPressed event){
    if(inside_box(x,y,close_box_size))
        MapSelectorButtonPressed=false;
    else if(inside_box(x,y,map1_size))
        map_selected=1;
    else if(inside_box(x,y,map2_size))
        map_selected=2;
    else if(inside_box(x,y,map3_size))
        map_selected=3;
    else if(inside_box(x,y,map4_size))
        map_selected=4;
    else if(inside_box(x,y,map5_size))
        map_selected=5;
    else if(inside_box(x,y,map6_size))
        map_selected=6;
    else if(inside_box(x,y,map7_size))
        map_selected=7;
    else if(inside_box(x,y,map8_size))
        map_selected=8;
    else if(inside_box(x,y,ok_button_size)){
        act_on_ok_button_pressed();
        update_message_to_current_city();
    }
    else
        return;
    
    draw_screen();
}

void act_on_ok_button_pressed(){
    if(!map_selected)
        return;
    
    setlinewidth(4);
    setcolor(RED);
    drawrect(ok_button_size);
    
    flushinput();
    delay(50);
    flushinput();
    
    close_map();
    act_on_clear_button(draw_screen);
    
    switch(map_selected){
        case 1:
            load_map("/cad2/ece297s/public/maps/toronto.streets.bin");
            break;
        case 2:
            load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
            break;
        case 3:
            load_map("/cad2/ece297s/public/maps/newyork.streets.bin");
            break;
        case 4:
            load_map("/cad2/ece297s/public/maps/moscow.streets.bin");
            break;
        case 5:
            load_map("/cad2/ece297s/public/maps/hamilton_canada.streets.bin");
            break;
        case 6:
            load_map("/cad2/ece297s/public/maps/saint_helena.streets.bin");
            break;
        case 7:
            load_map("/cad2/ece297s/public/maps/cairo_egypt.streets.bin");
            break;
        case 8:
            load_map("/cad2/ece297s/public/maps/london_england.streets.bin");
            break;
        default:
            assert(("This line should never be executed!",1==0));
    }
}