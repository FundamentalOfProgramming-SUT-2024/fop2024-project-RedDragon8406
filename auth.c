#include <ncurses.h>
#include <string.h>
#include "auth.h"
#include "main.h"
#include "menu.h"
#include "design.h"

void authentication_window() {
    WINDOW *auth_win;
    int highlight = 1;
    int choice = 0;
    int c;

    clear();
    noecho();
    cbreak();
    curs_set(0);

    init_colors();

    int height = 12;
    int width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    auth_win = newwin(height, width, starty, startx);
    keypad(auth_win, TRUE);

    const char *options[] = {
        "Login",
        "Signup",
        "Continue as Guest"
    };
    int n_options = 3;

    box(auth_win, 0, 0);
    const char *title = "Login/Signup/Guest";
    mvwprintw(auth_win, 1, (width - strlen(title)) / 2, "%s", title);

    const char *backspace_message = "Press Backspace to return to main menu";
    mvwprintw(auth_win, height - 2, (width - strlen(backspace_message)) / 2, "%s", backspace_message);

    refresh();
    wrefresh(auth_win);

    while (1) {
        for (int i = 0; i < n_options; i++) {
            if (highlight == i + 1) {
                wattron(auth_win, A_REVERSE);
                mvwprintw(auth_win, i + 3, (width - strlen(options[i])) / 2, "%s", options[i]);
                wattroff(auth_win, A_REVERSE);
            } else {
                mvwprintw(auth_win, i + 3, (width - strlen(options[i])) / 2, "%s", options[i]);
            }
        }
        wrefresh(auth_win);

        c = wgetch(auth_win);

        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_options;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_options)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            case KEY_BACKSPACE:
                refresh();
                delwin(auth_win);
                clear();
                refresh();
                show_main_menu();
                return;
            default:
                break;
        }

        if (choice != 0) {
            if (choice == 1) { // Login
                mvprintw(LINES - 2, 0, "Login selected\n");
                refresh();
                getch();
                break;
            } else if (choice == 2) { // Signup
                mvprintw(LINES - 2, 0, "Signup selected\n");
                refresh();
                getch();
                break;
            } else if (choice == 3) { // Continue as Guest
                refresh();
                delwin(auth_win);
                clear();
                refresh();
                show_main_menu();
                return;
            }
        }
    }
    endwin();
}
