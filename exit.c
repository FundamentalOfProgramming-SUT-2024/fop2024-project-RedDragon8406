#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "exit.h"
#include "menu.h"
#include "choices.h"
#include "design.h"
#include "main.h"

void print_menu(WINDOW *menu_win, int highlight, int n_choices);


void show_exit_confirmation(WINDOW *menu_win, int highlight) {
    int choice = 1; // 0 for yes, 1 for no
    int c;

    wclear(menu_win);
    // pre configuration
    int height = 7;
    int width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *exit_win = newwin(height, width, starty, startx);
    keypad(exit_win, TRUE); // enable keypad
    box(exit_win, 0, 0);
    
    const char *exit_text = "Are you sure you want to exit?";
    mvwprintw(exit_win, 1, (width - strlen(exit_text)) / 2, "%s", exit_text);

    mvwprintw(exit_win, 3, 14, "Yes");
    mvwprintw(exit_win, 3, 24, "No");
    wrefresh(exit_win);
    curs_set(0);

    while (1) {
        if (choice == 0) { // highlight Yes
            wattron(exit_win, COLOR_PAIR(2)); // green color
            mvwprintw(exit_win, 3, 14, "Yes");
            wattroff(exit_win, COLOR_PAIR(2));
            mvwprintw(exit_win, 3, 24, "No");
        } else { // highlight No
            mvwprintw(exit_win, 3, 14, "Yes");
            wattron(exit_win, COLOR_PAIR(3)); // red color
            mvwprintw(exit_win, 3, 24, "No");
            wattroff(exit_win, COLOR_PAIR(3));
        }

        wrefresh(exit_win);
        c = wgetch(exit_win);
        
        switch (c) {
            case KEY_LEFT:
                choice = 0;
                break;
            case KEY_RIGHT:
                choice = 1;
                break;
            case 10: // enter key
                if (choice == 0) {
                    // yes
                    delwin(exit_win); 
                    clear(); 
                    refresh(); 
                    
                    const char *goodbye_text = "Goodbye! Press any key to exit...";
                    attron(COLOR_PAIR(6)); 
                    mvprintw((LINES - 1) / 2, (COLS - strlen(goodbye_text)) / 2, "%s", goodbye_text);
                    attroff(COLOR_PAIR(6));

                    refresh();
                    getch();
                    endwin();
                    exit(0);
                } else {
                    // no
                    wclear(exit_win);
                    wrefresh(exit_win);
                    delwin(exit_win);
                    print_menu(menu_win, highlight, n_choices);
                    return;
                }
        }
    }
}
