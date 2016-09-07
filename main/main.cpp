#include "m4.h"
#include "m3.h"

int main() {
    
    load_map("/cad2/ece297s/public/maps/toronto.streets.bin");

    draw_map();
    
    close_map();
    
    std::cout << std::endl << "main.cpp finished";
}
