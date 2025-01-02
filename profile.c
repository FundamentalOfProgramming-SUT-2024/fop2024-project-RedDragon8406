#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include "menu.h"
#include "auth.h"


void show_profile() {
    WINDOW *profile_win;
    int height = 16;
    int width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    int show_password = 0; // 0 for hide, 1 for show
    int choice = 0; // 0 for Hide Password, 1 for Show Password

    profile_win = newwin(height, width, starty, startx);
    keypad(profile_win, TRUE);
    box(profile_win, 0, 0);

    const char *title = "PROFILE";
    mvwprintw(profile_win, 1, (width - strlen(title)) / 2, "%s", title);

    mvwprintw(profile_win, 3, 4, "Username:");
    mvwprintw(profile_win, 4, 4, "Email:");
    mvwprintw(profile_win, 5, 4, "Status:");
    mvwprintw(profile_win, 6, 4, "Points:");
    mvwprintw(profile_win, 7, 4, "Golds:");
    mvwprintw(profile_win, 8, 4, "XP:");
    mvwprintw(profile_win, 9, 4, "Password:");

    if(current_user) {
        mvwprintw(profile_win, 3, 14, "%s", current_user->username);
        mvwprintw(profile_win, 4, 14, "%s", current_user->email);
        mvwprintw(profile_win, 5, 14, "%d", current_user->status);
        mvwprintw(profile_win, 6, 14, "%d", current_user->points);
        mvwprintw(profile_win, 7, 14, "%d", current_user->golds);
        mvwprintw(profile_win, 8, 14, "%d", current_user->xp);
        if (show_password) {
            mvwprintw(profile_win, 9, 14, "%s", current_user->password);
        } else {
            int len = strlen(current_user->password);
            char format[50]="************************************************";
            format[len]='\0';
            mvwprintw(profile_win, 9, 14, "%s", format);
        }
    }

    if (choice == 0) {
        wattron(profile_win, A_REVERSE);
        mvwprintw(profile_win, 11, 4, "Hide Password");
        wattroff(profile_win, A_REVERSE);
        mvwprintw(profile_win, 11, 22, "Show Password");
    } else {
        mvwprintw(profile_win, 11, 4, "Hide Password");
        wattron(profile_win, A_REVERSE);
        mvwprintw(profile_win, 11, 22, "Show Password");
        wattroff(profile_win, A_REVERSE);
    }

    wrefresh(profile_win);

    int ch;
    while ((ch = wgetch(profile_win)) != KEY_BACKSPACE) {
        switch (ch) {
            case KEY_LEFT:
                choice = 0;
                break;
            case KEY_RIGHT:
                choice = 1;
                break;
            case 10: // Enter key
                if (choice == 0) {
                    show_password = 0;
                } else {
                    show_password = 1;
                }
                break;
        }

        // Update button highlight
        if (choice == 0) {
            wattron(profile_win, A_REVERSE);
            mvwprintw(profile_win, 11, 4, "Hide Password");
            wattroff(profile_win, A_REVERSE);
            mvwprintw(profile_win, 11, 22, "Show Password");
        } else {
            mvwprintw(profile_win, 11, 4, "Hide Password");
            wattron(profile_win, A_REVERSE);
            mvwprintw(profile_win, 11, 22, "Show Password");
            wattroff(profile_win, A_REVERSE);
        }

        if (current_user) {
            if (show_password) {
                mvwprintw(profile_win, 9, 14, "%s", current_user->password);
            } else {
                int len = strlen(current_user->password);
                char format[50]="************************************************";
                format[len]='\0';
                mvwprintw(profile_win, 9, 14, "%s", format);
            }
        }

        wrefresh(profile_win);
    }

    delwin(profile_win);
    clear();
    refresh();
    show_main_menu();
}


