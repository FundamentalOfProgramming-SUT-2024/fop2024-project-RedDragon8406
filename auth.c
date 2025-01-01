#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <ctype.h>
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
                delwin(auth_win);
                clear();
                refresh();
                signup_window();
                return;
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


void signup_window() {
    WINDOW *signup_win;
    int height = 17; 
    int width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    signup_win = newwin(height, width, starty, startx);
    keypad(signup_win, TRUE);
    box(signup_win, 0, 0);

    const char *title = "Sign Up";
    mvwprintw(signup_win, 1, (width - strlen(title)) / 2, "%s", title);

    const char *username_label = "Username:";
    const char *password_label = "Password:";
    const char *email_label = "Email:";

    mvwprintw(signup_win, 3, 2, "%s", username_label);
    mvwprintw(signup_win, 5, 2, "%s", password_label);
    mvwprintw(signup_win, 7, 2, "%s", email_label);

    const char *signup_button = "Sign Up";
    const char *back_button = "Back";

    mvwprintw(signup_win, 10, (width - strlen(back_button)) / 2, "%s", back_button);
    mvwprintw(signup_win, 12, (width - strlen(signup_button)) / 2, "%s", signup_button);

    char username[MAX_USERNAME_LENGTH] = "";
    char password[MAX_PASSWORD_LENGTH] = "";
    char email[MAX_EMAIL_LENGTH] = "";
    int highlight = 1;
    int cursor_pos = 0;
    int c;

    while (1) {
        if (highlight == 1) {
            wattron(signup_win, A_REVERSE);
            mvwprintw(signup_win, 3, 12, "%-30s", username);
            wattroff(signup_win, A_REVERSE);
        } else {
            mvwprintw(signup_win, 3, 12, "%-30s", username);
        }

        if (highlight == 2) {
            wattron(signup_win, A_REVERSE);
            mvwprintw(signup_win, 5, 12, "%-30s", password);
            wattroff(signup_win, A_REVERSE);
        } else {
            mvwprintw(signup_win, 5, 12, "%-30s", password);
        }

        if (highlight == 3) {
            wattron(signup_win, A_REVERSE);
            mvwprintw(signup_win, 7, 12, "%-30s", email);
            wattroff(signup_win, A_REVERSE);
        } else {
            mvwprintw(signup_win, 7, 12, "%-30s", email);
        }

        if (highlight == 4) {
            wattron(signup_win, A_REVERSE);
            mvwprintw(signup_win, 10, (width - strlen(back_button)) / 2, "%s", back_button);
            wattroff(signup_win, A_REVERSE);
        } else {
            mvwprintw(signup_win, 10, (width - strlen(back_button)) / 2, "%s", back_button);
        }

        if (highlight == 5) {
            wattron(signup_win, A_REVERSE);
            mvwprintw(signup_win, 12, (width - strlen(signup_button)) / 2, "%s", signup_button);
            wattroff(signup_win, A_REVERSE);
        } else {
            mvwprintw(signup_win, 12, (width - strlen(signup_button)) / 2, "%s", signup_button);
        }

        wrefresh(signup_win);
        c = wgetch(signup_win);

        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = 5;
                else
                    --highlight;
                cursor_pos = (highlight == 1) ? strlen(username) : (highlight == 2) ? strlen(password) : (highlight == 3) ? strlen(email) : 0;
                break;
            case KEY_DOWN:
                if (highlight == 5)
                    highlight = 1;
                else
                    ++highlight;
                cursor_pos = (highlight == 1) ? strlen(username) : (highlight == 2) ? strlen(password) : (highlight == 3) ? strlen(email) : 0;
                break;
            case 10: 
                if (highlight == 4) { // back button
                    delwin(signup_win);
                    clear();
                    refresh();
                    authentication_window();
                    return;
                } else if (highlight == 5) {
                    // signup button
                    if (strlen(username) == 0 || strlen(password) == 0 || strlen(email) == 0) {
                        mvwprintw(signup_win, 14, 2, "All fields are required!");
                        wrefresh(signup_win);
                    } else {
                        signup_user(username, password, email);
                        return;
                    }
                }
                break;
            default:
                if (highlight == 1 && isprint(c) && cursor_pos < MAX_USERNAME_LENGTH - 1) {
                    username[cursor_pos++] = c;
                    username[cursor_pos] = '\0';
                } else if (highlight == 1 && c == KEY_BACKSPACE && cursor_pos > 0) {
                    username[--cursor_pos] = '\0';
                } else if (highlight == 2 && isprint(c) && cursor_pos < MAX_PASSWORD_LENGTH - 1) {
                    password[cursor_pos++] = c;
                    password[cursor_pos] = '\0';
                } else if (highlight == 2 && c == KEY_BACKSPACE && cursor_pos > 0) {
                    password[--cursor_pos] = '\0';
                } else if (highlight == 3 && isprint(c) && cursor_pos < MAX_EMAIL_LENGTH - 1) {
                    email[cursor_pos++] = c;
                    email[cursor_pos] = '\0';
                } else if (highlight == 3 && c == KEY_BACKSPACE && cursor_pos > 0) {
                    email[--cursor_pos] = '\0';
                }
                break;
        }
    }

    delwin(signup_win);
    clear();
    refresh();
    show_main_menu();
}


int store_user_data(const char* username, const char* password, const char* email) {
    char filepath[250];
    snprintf(filepath, sizeof(filepath), "./%s%s.db", USERS_DIR, username);
    FILE* file = fopen(filepath, "w");
    if (file) {
        fprintf(file, "%s\n", username);
        fprintf(file, "%s\n", password);
        fprintf(file, "%s\n", email);
        fprintf(file, "0");
        fclose(file);
        return 1;
    } else {
        mvprintw(0, 0, "Error creating user file.");
        refresh();
        return 0;
    }
}


void signup_user(const char* username, const char* password, const char* email) {
    int temp_status=store_user_data(username, password, email);
    if (temp_status){
        mvprintw(0, 0, "User registered successfully!");
        refresh();
        getch();
        clear();
        refresh();
        show_main_menu();
    }
    else{
        refresh();
        getch();
        clear();
        refresh();
        signup_window();
    }
}
