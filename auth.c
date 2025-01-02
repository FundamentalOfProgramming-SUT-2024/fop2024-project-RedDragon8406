#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include "auth.h"
#include "main.h"
#include "menu.h"
#include "design.h"
#include "settings.h"

UserData* current_user = NULL;

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
                delwin(auth_win);
                clear();
                refresh();
                login_window();
                return;
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

void login_window() {
    WINDOW *login_win;
    int height = 17; 
    int width = 60;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    login_win = newwin(height, width, starty, startx);
    keypad(login_win, TRUE);
    box(login_win, 0, 0);

    const char *title = "LOGIN";
    mvwprintw(login_win, 1, (width - strlen(title)) / 2, "%s", title);

    const char *username_label = "Username:";
    const char *password_label = "Password:";

    mvwprintw(login_win, 3, 2, "%s", username_label);
    mvwprintw(login_win, 5, 2, "%s", password_label);

    const char *back_button = "Back";
    const char *login_button = "Login";

    mvwprintw(login_win, 10, (width - strlen(back_button)) / 2, "%s", back_button);
    mvwprintw(login_win, 12, (width - strlen(login_button)) / 2, "%s", login_button);

    char username[MAX_USERNAME_LENGTH] = "";
    char password[MAX_PASSWORD_LENGTH] = "";
    int highlight = 1;
    int cursor_pos = 0;
    int c;

    while (1) {
        if (highlight == 1) {
            wattron(login_win, A_REVERSE);
            mvwprintw(login_win, 3, height-2, "%-40s", username);
            wattroff(login_win, A_REVERSE);
        } else {
            mvwprintw(login_win, 3, height-2, "%-40s", username);
        }

        if (highlight == 2) {
            wattron(login_win, A_REVERSE);
            mvwprintw(login_win, 5, height-2, "%-40s", password);
            wattroff(login_win, A_REVERSE);
        } else {
            mvwprintw(login_win, 5, height-2, "%-40s", password);
        }

        if (highlight == 3) {
            wattron(login_win, A_REVERSE);
            mvwprintw(login_win, 10, (width - strlen(back_button)) / 2, "%s", back_button);
            wattroff(login_win, A_REVERSE);
        } else {
            mvwprintw(login_win, 10, (width - strlen(back_button)) / 2, "%s", back_button);
        }

        if (highlight == 4) {
            wattron(login_win, A_REVERSE);
            mvwprintw(login_win, 12, (width - strlen(login_button)) / 2, "%s", login_button);
            wattroff(login_win, A_REVERSE);
        } else {
            mvwprintw(login_win, 12, (width - strlen(login_button)) / 2, "%s", login_button);
        }

        wrefresh(login_win);
        c = wgetch(login_win);

        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = 4;
                else
                    --highlight;
                cursor_pos = (highlight == 1) ? strlen(username) : (highlight == 2) ? strlen(password) : 0;
                break;
            case KEY_DOWN:
                if (highlight == 4)
                    highlight = 1;
                else
                    ++highlight;
                cursor_pos = (highlight == 1) ? strlen(username) : (highlight == 2) ? strlen(password) : 0;
                break;
            case 10: // Enter key
                if (highlight == 3) { // Back button
                    delwin(login_win);
                    clear();
                    refresh();
                    authentication_window();
                    return;
                }else if (highlight == 4) { // Login button
                    // Perform login logic
                    if (login_user(username, password)) {
                        const char *success_message = "Login successful! Press a key to continue...";
                        mvwprintw(login_win, 14, (width - strlen(success_message)) / 2, "%s", success_message);
                        wrefresh(login_win);
                        getch();
                        delwin(login_win);
                        clear();
                        refresh();
                        show_main_menu();
                        return;
                    } else {
                        const char *failed_message = "Invalid username or password. Press a key to continue...";
                        mvwprintw(login_win, 14, (width - strlen(failed_message)) / 2, "%s", failed_message);
                        wrefresh(login_win);
                        getch();
                        mvwprintw(login_win,14, (width - strlen(failed_message)) / 2, "%*s", (int)strlen(failed_message), "");
                        wrefresh(login_win);
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
                }
                break;
        }
    }

    delwin(login_win);
    clear();
    refresh();
    show_main_menu();
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


                } else if (highlight == 5) { // sign up button
                    if(signup_user(username,password,email)){
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
    fprintf(file, "USERNAME %s\n", username);
    fprintf(file, "PASSWORD %s\n", password);
    fprintf(file, "EMAIL %s\n", email);
    fprintf(file, "STATUS 0\n");
    fprintf(file, "POINTS 0\n");
    fprintf(file, "GOLDS 0\n"); 
    fprintf(file, "XP 0\n"); 
    fclose(file);
    return 1;
}

int user_exists(const char* username) {
    char filepath[250];
    snprintf(filepath, sizeof(filepath), "./%s%s.db", USERS_DIR, username);

    FILE* file = fopen(filepath, "r");
    if (file) {
        fclose(file);
        return 1; // user exists
    }
    return 0; // username does not exist
}

int password_length_valid(const char* password) {
    return strlen(password) >= 7;
}

int password_final_validation(const char* password) {
    int lower = 0, upper = 0, digit = 0;
    for (int i = 0; i < strlen(password); i++) {
        if (islower(password[i])) {
        lower = 1;
        }
        if (isupper(password[i])) {
        upper = 1;
        }
        if (isdigit(password[i])) {
        digit = 1;
        }
    }
    return lower && upper && digit;
}

int email_valid(const char* email) {
    regex_t regex;
    int regex_init;

    const char* pattern = "^[^@]+@[^@.]+\\.[^@.]+$"; // the pattern
    regex_init = regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE); // some flags at the end
    regex_init = regexec(&regex, email, 0, NULL, 0); // won't need to save any substring
    regfree(&regex);

    if (regex_init == 0) {
        return 1; // Email is valid
    }
    return 0;
}

int save_user_data(UserData* user_data) {
    char filepath[250];
    snprintf(filepath, sizeof(filepath), "./%s%s.db", USERS_DIR, user_data->username);
    FILE* file = fopen(filepath, "w");
    fprintf(file, "USERNAME %s\n", user_data->username);
    fprintf(file, "PASSWORD %s\n", user_data->password);
    fprintf(file, "EMAIL %s\n", user_data->email);
    fprintf(file, "STATUS %d\n", user_data->status);
    fprintf(file, "POINTS %d\n", user_data->points);
    fprintf(file, "GOLDS %d\n", user_data->golds);
    fprintf(file, "XP %d\n", user_data->xp);
    fclose(file);
    return 1;
}

int read_user_data(UserData* user_data) {
    char filepath[250];
    snprintf(filepath, sizeof(filepath), "./%s%s.db", USERS_DIR, user_data->username);
    FILE* file = fopen(filepath, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (sscanf(line, "USERNAME %s", user_data->username) == 1) {
                if (user_data->username[strlen(user_data->username) - 1] == '\n') {
                    user_data->username[strlen(user_data->username) - 1] = '\0';
                }
                continue;
            }
            if (sscanf(line, "PASSWORD %s", user_data->password) == 1) {
                if (user_data->password[strlen(user_data->password) - 1] == '\n') {
                    user_data->password[strlen(user_data->password) - 1] = '\0';
                }
                continue;
            }
            if (sscanf(line, "EMAIL %s", user_data->email) == 1) {
                if (user_data->email[strlen(user_data->email) - 1] == '\n') {
                    user_data->email[strlen(user_data->email) - 1] = '\0';
                }
                continue;
            }
            if (sscanf(line, "STATUS %d", &user_data->status) == 1) continue;
            if (sscanf(line, "POINTS %d", &user_data->points) == 1) continue;
            if (sscanf(line, "GOLDS %d", &user_data->golds) == 1) continue;
            if (sscanf(line, "XP %d", &user_data->xp) == 1) continue;
        }
        fclose(file);
        return 1;
    } else {
        mvprintw(0, 0, "Error reading user file.");
        refresh();
        return 0;
    }
}

void check_logged_in_user(void) {
    current_user = (UserData *)malloc(sizeof(UserData));
    if (!load_current_user(current_user) || !read_user_data(current_user)) {
        free(current_user);
        current_user = NULL;
    }
}

int save_current_user(UserData* user_data) {
    FILE* file = fopen("current_user.db", "w");
    fprintf(file, "USERNAME %s\n", user_data->username);
    fclose(file);
    return 1;
}

int load_current_user(UserData* user_data) {
    // Maybe gonna change the algorythm later
    FILE* file = fopen("current_user.db", "r");
    if (file) {
        char line[256];
        if (fgets(line, sizeof(line), file) && sscanf(line, "USERNAME %s", user_data->username) == 1) {
            fclose(file);
            if (user_data->username[strlen(user_data->username) - 1] == '\n') {
                user_data->username[strlen(user_data->username) - 1] = '\0';
            }
            return 1; // Successfully loaded username
        }
        fclose(file);
    }
    return 0; // Failed to load username
}

int save_new_user_list(const char * username){
    FILE * file = fopen("users_list.db","a");
    fprintf(file, "%s\n", username);
    fclose(file);
    return 1;
}

int login_user(const char* username, const char* password) {
    // Maybe gonna change the algorythm later
    current_user = malloc(sizeof(UserData));
    strncpy(current_user->username, username, MAX_USERNAME_LENGTH);
    if (read_user_data(current_user)) {
        if (strcmp(username, current_user->username) == 0 && strcmp(password, current_user->password) == 0) {
            current_user->status = 1;
            save_user_data(current_user); // to save the status of being logged in ( maybe gonna delete that later )
            save_current_user(current_user);
            return 1; // Successful login
        } else {
            free(current_user);
            current_user = NULL;
            return 0; // Failed login
        }
    } else {
        free(current_user);
        current_user = NULL;
        return 0; // Failed to read user data
    }
}

void logout_user(void) {
    if (current_user != NULL) {
        current_user->status = 0; 
        save_user_data(current_user); // to save the status of being logged out
        remove("current_user.db"); // Remove the current user file ( not in the mood to write \0 lol )
        free(current_user);
        current_user = NULL;

        WINDOW *logout_win;
        int height = 10;
        int width = 56;
        int starty = (LINES - height) / 2;
        int startx = (COLS - width) / 2;

        logout_win = newwin(height, width, starty, startx);
        keypad(logout_win, TRUE);
        box(logout_win, 0, 0);

        apply_logout_design(logout_win); // Apply the design

        getch();

        delwin(logout_win);
        clear();
        refresh();
    }
}

int signup_user(const char* username, const char* password, const char* email) {
    // Gonna give it some design later
    if (strlen(username) == 0 || strlen(password) == 0 || strlen(email) == 0) {
        mvprintw(10, 2, "All fields are required! press a key to continue..");
        refresh();
        getch(); 
        mvprintw(10, 2, "%*s", COLS - 4, "");
        refresh();
        return 0;
    }
    if (!password_length_valid(password)) {
        mvprintw(10, 2, "Password must be at least 7 characters long. Press a key to continue...");
        refresh();
        getch();
        mvprintw(10, 2, "%*s", COLS - 4, ""); 
        refresh();
        return 0;
    }

    if(!password_final_validation(password)){
        mvprintw(10, 2, "Password is not complex enough! Press a key to continue...");
        refresh();
        getch();
        mvprintw(10, 2, "%*s", COLS - 4, ""); 
        refresh();
        return 0;   
    }

    if(!email_valid(email)){
        mvprintw(10, 2, "Email is not valid! Press a key to continue...");
        refresh();
        getch();
        mvprintw(10, 2, "%*s", COLS - 4, ""); 
        refresh();
        return 0;   
    }

    if (user_exists(username)) {
        mvprintw(1, 2, "Username already taken. Please choose another one. press a key to continue..");
        refresh();
        getch();
        mvprintw(10, 2, "%*s", COLS - 4, "");
        refresh();
        return 0;
    }

    int temp_status = store_user_data(username, password, email);
    if (temp_status) {  // ---------------------------- signup successful ----------------------------------------
        save_new_user_list(username);
        create_settings(username);
        mvprintw(0, 0, "User registered successfully! press a key to continue..");                              //
        refresh();                                                                                              //
        getch();                                                                                                //
        clear();                                                                                                //
        refresh();                                                                                              //
        show_main_menu();                                                                                       //
        return 1;       // ---------------------------- signup successful ----------------------------------------
    } else {
        mvprintw(10, 2, "Error registering user. Please try again. press a key to continue..");
        refresh();
        getch();
        mvprintw(10, 2, "%*s", COLS - 4, "");
        refresh();
        return 0;
    }
}

