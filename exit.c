#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "exit.h"
#include "menu.h"
#include "choices.h"
#include "design.h" // Include the design header

// Prototype for print_menu function
void print_menu(WINDOW *menu_win, int highlight, int n_choices);

// Function to display the exit confirmation dialog
void show_exit_confirmation(WINDOW *menu_win, int highlight) {
    int choice = 1; // 0 for Yes, 1 for No
    int c;

    wclear(menu_win);
    int height = 7;
    int width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *exit_win = newwin(height, width, starty, startx);
    keypad(exit_win, TRUE); // Enable keypad for exit_win
    box(exit_win, 0, 0);
    
    const char *exit_text = "Are you sure you want to exit?";
    mvwprintw(exit_win, 1, (width - strlen(exit_text)) / 2, "%s", exit_text);

    mvwprintw(exit_win, 3, 14, "Yes");
    mvwprintw(exit_win, 3, 24, "No");
    wrefresh(exit_win);
    curs_set(0); // Hide cursor

    while (1) {
        if (choice == 0) { // Highlight Yes
            wattron(exit_win, COLOR_PAIR(2)); // Apply green color
            mvwprintw(exit_win, 3, 14, "Yes");
            wattroff(exit_win, COLOR_PAIR(2));
            mvwprintw(exit_win, 3, 24, "No");
        } else { // Highlight No
            mvwprintw(exit_win, 3, 14, "Yes");
            wattron(exit_win, COLOR_PAIR(3)); // Apply red color
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
            case 10: // Enter key
                if (choice == 0) {
                    // User chose Yes
                    delwin(exit_win); // Delete exit window
                    clear(); // Clear the screen
                    refresh(); // Refresh to apply changes
                    
                    const char *goodbye_text = "Goodbye! Press any key to exit...";
                    attron(COLOR_PAIR(6)); // Apply custom exit message color
                    mvprintw((LINES - 1) / 2, (COLS - strlen(goodbye_text)) / 2, "%s", goodbye_text);
                    attroff(COLOR_PAIR(6));

                    refresh();
                    getch();
                    endwin();
                    exit(0);
                } else {
                    // User chose No
                    wclear(exit_win);
                    wrefresh(exit_win);
                    delwin(exit_win);
                    print_menu(menu_win, highlight, NUM_CHOICES);
                    return;
                }
        }
    }
}
