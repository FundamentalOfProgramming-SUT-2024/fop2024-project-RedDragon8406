#include <ncurses.h>
#include <string.h>
#include "scoreboard.h"
#include "menu.h" 
#include "main.h"
void show_scoreboard() {
    WINDOW *scoreboard_win;
    int height = 20;
    int width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    scoreboard_win = newwin(height, width, starty, startx);
    keypad(scoreboard_win, TRUE);
    box(scoreboard_win, 0, 0);

    const char *title = "SCOREBOARD";
    mvwprintw(scoreboard_win, 1, (width - strlen(title)) / 2, "%s", title);

    wrefresh(scoreboard_win);

    int ch;
    while ((ch = wgetch(scoreboard_win)) != KEY_BACKSPACE) {
        // Wait for backspace key to return to main menu
    }

    delwin(scoreboard_win);
    clear();
    refresh();
    show_main_menu();
}
