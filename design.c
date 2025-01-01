#include <ncurses.h>
#include "design.h"

#define COLOR_CUSTOM_EXIT 8


void init_colors() {
    start_color();
    
    init_color(COLOR_CUSTOM_EXIT, 142 * 4, 179 * 4, 219 * 4); // Custom color for exit message

    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // Menu border color
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // Yes button color
    init_pair(3, COLOR_RED, COLOR_BLACK);    // No button color
    init_pair(4, COLOR_BLACK, COLOR_YELLOW); // Hover effect color
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   // Welcome text color
    init_pair(6, COLOR_CUSTOM_EXIT, COLOR_BLACK); // Goodbye text color
}
