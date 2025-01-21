#include <ncurses.h>
#include <string.h>
#include "design.h"
#include "settings.h"
#include "auth.h"
#include "game.h"

#define COLOR_CUSTOM_EXIT 8
#define COLOR_CUSTOM_SILVER 9 
#define PLAYER_WHITE 10
#define PLAYER_BLUE 11
#define PLAYER_GREEN 12
#define COLOR_CUSTOM_GOLD 13 


int get_color_pair(const char *color_name) {
    if (!strcmp(color_name, "white")) {
        return PLAYER_WHITE;
    } else if (!strcmp(color_name, "blue")) {
        return PLAYER_BLUE;
    } else if (!strcmp(color_name, "green")) {
        return PLAYER_GREEN;
    }
    return 1;
}


void init_colors() {
    start_color();
    
    init_color(COLOR_CUSTOM_EXIT, 142 * 4, 179 * 4, 219 * 4); // color for exit message
    init_color(COLOR_CUSTOM_SILVER, 192 * 4, 192 * 4, 192 * 4); // silver color
    init_color(COLOR_CUSTOM_GOLD, 1000, 843, 0);

    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // menu border color
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // yes button color
    init_pair(3, COLOR_RED, COLOR_BLACK);    // no button color
    init_pair(4, COLOR_BLACK, COLOR_YELLOW); // hover effect color
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   // welcome text color
    init_pair(6, COLOR_CUSTOM_EXIT, COLOR_BLACK); // goodbye text color
    init_pair(7, COLOR_CUSTOM_SILVER, COLOR_BLACK); // custom silver color
    init_pair(COLOR_CUSTOM_GOLD, COLOR_CUSTOM_GOLD, COLOR_BLACK); // custom silver color
    init_pair(PLAYER_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(PLAYER_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(PLAYER_GREEN, COLOR_GREEN, COLOR_BLACK);
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

void PrintPlayer(WINDOW *gamewin, Player *player, Settings* settings){
    int desired_color=get_color_pair(settings->user_color);
    wattron(gamewin, COLOR_PAIR(desired_color));
    mvwprintw(gamewin, player->loc.y, player->loc.x, "@");
    wattroff(gamewin, COLOR_PAIR(desired_color));
}

void PrintGold(WINDOW *gamewin, Gold* gold , Settings* settings){
    wattron(gamewin, COLOR_PAIR(COLOR_CUSTOM_GOLD));
    mvwprintw(gamewin,gold->loc.y,gold->loc.x,"G"); // golds
    wattroff(gamewin, COLOR_PAIR(COLOR_CUSTOM_GOLD));
}
void PrintFood(WINDOW *gamewin, Food* food , Settings* settings){
    wattron(gamewin, COLOR_PAIR(COLOR_CYAN));
    mvwprintw(gamewin,food->loc.y,food->loc.x,"F"); // golds
    wattroff(gamewin, COLOR_PAIR(COLOR_CYAN));
}