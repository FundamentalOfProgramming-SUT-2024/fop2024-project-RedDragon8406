#include <ncurses.h>
#include "design.h"
#include <string.h>
#define COLOR_CUSTOM_EXIT 8
#define COLOR_CUSTOM_SILVER 9 

void init_colors() {
    start_color();
    
    init_color(COLOR_CUSTOM_EXIT, 142 * 4, 179 * 4, 219 * 4); // color for exit message
    init_color(COLOR_CUSTOM_SILVER, 192 * 4, 192 * 4, 192 * 4); // silver color

    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // menu border color
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // yes button color
    init_pair(3, COLOR_RED, COLOR_BLACK);    // no button color
    init_pair(4, COLOR_BLACK, COLOR_YELLOW); // hover effect color
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   // welcome text color
    init_pair(6, COLOR_CUSTOM_EXIT, COLOR_BLACK); // goodbye text color
    init_pair(7, COLOR_CUSTOM_SILVER, COLOR_BLACK); // custom silver color
}

void apply_logout_design(WINDOW *logout_win) {
    wattron(logout_win, COLOR_PAIR(7) | A_BOLD | A_ITALIC);

    const char *title = "LOGOUT";
    int width = getmaxx(logout_win);
    mvwprintw(logout_win, 1, (width - strlen(title)) / 2, "%s", title);

    const char *message = "You have logged out. Press any key to continue...";
    mvwprintw(logout_win, 6, (width - strlen(message)) / 2, "%s", message);

    wrefresh(logout_win);
    wattroff(logout_win, COLOR_PAIR(7) | A_BOLD | A_ITALIC); 
    wrefresh(logout_win);
}

