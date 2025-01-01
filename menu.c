#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "menu.h"
#include "exit.h"
#include "choices.h"
#include "design.h" 
#include "main.h" // last include
#include "auth.h"
void print_menu(WINDOW *menu_win, int highlight, int n_choices);
void handle_logged_in_choice(WINDOW *menu_win, int choice, int highlight);
void handle_logged_out_choice(WINDOW *menu_win, int choice, int highlight);


void show_main_menu() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    initscr();
    clear();
    noecho();
    cbreak(); // disabling line buffer
    curs_set(0); // hiding the cursor

    init_colors();

    // update which choices should be shown
    update_choices();

    int height = 12;
    int width = 30;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);

    const char *welcome_text = "Welcome to My Rogue Game!";
    attron(COLOR_PAIR(5)); 
    mvprintw(0, (COLS - strlen(welcome_text)) / 2, "%s", welcome_text);
    attroff(COLOR_PAIR(5)); 

    refresh();
    print_menu(menu_win, highlight, n_choices);
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_choices)
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
        print_menu(menu_win, highlight, n_choices);

        if (logged_in_state) {
            handle_logged_in_choice(menu_win, choice, highlight);
        } else {
            handle_logged_out_choice(menu_win, choice, highlight);
        }
    }
    endwin();
}

void print_menu(WINDOW *menu_win, int highlight, int n_choices) {
    int x, y, i;
    x = 2;
    y = 3; //  make some space for the welcome message
    box(menu_win, 0, 0);
    for (i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) { // highlight the current choice
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}

void handle_logged_in_choice(WINDOW *menu_win, int choice, int highlight) {
    switch (choice) {
        case 1: // Start Game
            mvprintw(23, 0, "Start Game clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 2: // Load Game
            mvprintw(23, 0, "Load Game clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 3: // Profile
            mvprintw(23, 0, "Profile clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 4: // Scoreboard
            mvprintw(23, 0, "Scoreboard clicked (logged in)\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 5: // Settings
            mvprintw(23, 0, "Settings clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 6: // Logout
            mvprintw(23, 0, "Logout clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 7: // Exit
            show_exit_confirmation(menu_win, highlight);
            break;
        default:
            break;
    }
}

void handle_logged_out_choice(WINDOW *menu_win, int choice, int highlight) {
    switch (choice) {
        case 1: // Start Game
            mvprintw(23, 0, "Start Game clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 2: // Play as Guest
            mvprintw(23, 0, "Play as Guest clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 3: // Login/Signup
            authentication_window();
            break;
        case 4: // Scoreboard
            mvprintw(23, 0, "Scoreboard clicked (logged out)\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 5: // Settings
            mvprintw(23, 0, "Settings clicked\n");
            clrtoeol();
            refresh();
            getch();
            break;
        case 6: // Exit
            show_exit_confirmation(menu_win, highlight);
            break;
        default:
            break;
    }
}
