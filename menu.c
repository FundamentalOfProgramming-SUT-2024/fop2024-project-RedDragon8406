// menu.c
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "menu.h"
#include "exit.h"
#include "choices.h" // Include the choices header

void print_menu(WINDOW *menu_win, int highlight, int n_choices) {
    int x, y, i;
    x = 2;
    y = 3; // Adjusted to make space for the welcome message
    box(menu_win, 0, 0);
    for (i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) { // Highlight the current choice
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}

void show_main_menu() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    initscr();
    clear();
    noecho();
    cbreak(); // Line buffering disabled. Pass on everything
    curs_set(0); // Hide cursor
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);

    int height = 12;
    int width = 30;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    mvprintw(0, (COLS - strlen("Welcome to My Rogue Game!")) / 2, "Welcome to My Rogue Game!");
    mvprintw(1, 0, "Use arrow keys to move, Enter to select");
    refresh();
    print_menu(menu_win, highlight, NUM_CHOICES);
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = NUM_CHOICES;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == NUM_CHOICES)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            default:
                break;
        }
        print_menu(menu_win, highlight, NUM_CHOICES);
        if (choice == 6) { // Exit selected
            show_exit_confirmation(menu_win, highlight);
            choice = 0; // Reset choice after returning from exit confirmation
        } else if (choice != 0) {
            mvprintw(23, 0, "You chose %s\n", choices[choice - 1]);
            clrtoeol();
            refresh();
            getch();
            break;
        }
    }
    endwin();
}
