#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include "settings.h"
#include "menu.h"
#include "auth.h"

Settings* settings = NULL;

void save_settings() {
    char filepath[250];
    if (current_user == NULL) {
        snprintf(filepath, sizeof(filepath), "settings.db");
    } else {
        snprintf(filepath, sizeof(filepath), "./settings/%s.db", current_user->username);
    }
    FILE* file = fopen(filepath, "w");
    fprintf(file, "DIFFICULTY %s\n", settings->difficulty);
    fprintf(file, "COLOR %s\n", settings->user_color);
    fclose(file);
}

void create_settings(const char *username) {
    if (settings == NULL) {
        settings = (Settings*)malloc(sizeof(Settings));
        settings->user = NULL;
        strcpy(settings->difficulty, "easy");
        strcpy(settings->user_color, "white");
    }
    char filepath[250];
    snprintf(filepath, sizeof(filepath), "./settings/%s.db", username);
    FILE* file = fopen(filepath, "w");
    fprintf(file, "DIFFICULTY %s\n", "easy");
    fprintf(file, "COLOR %s\n", "white");
    fclose(file);
}


void set_default_settings(){
    if (settings == NULL) {
        settings = (Settings*)malloc(sizeof(Settings));
        settings->user = NULL;
        strcpy(settings->difficulty, "easy");
        strcpy(settings->user_color, "white");
    }
    char filepath[250];
    snprintf(filepath, sizeof(filepath), "./settings.db");
    FILE* file = fopen(filepath, "w");
    fprintf(file, "DIFFICULTY %s\n", "easy");
    fprintf(file, "COLOR %s\n", "white");
    fclose(file);
}

void load_settings() {
    if (settings == NULL) {
        settings = (Settings*)malloc(sizeof(Settings));
        settings->user = NULL;
        strcpy(settings->difficulty, "easy");
        strcpy(settings->user_color, "white");
    }
    char filepath[250];
    if (current_user == NULL) {
        snprintf(filepath, sizeof(filepath), "settings.db");
    } else {
        settings->user=current_user;
        snprintf(filepath, sizeof(filepath), "./settings/%s.db", settings->user->username);
    }
    FILE* file = fopen(filepath, "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "DIFFICULTY %s", settings->difficulty) == 1) {
        } else if (sscanf(line, "COLOR %s", settings->user_color) == 1) {
        }
    }
    fclose(file);
}




void settings_window() {
    if (settings == NULL) {
        settings = (Settings*)malloc(sizeof(Settings));
        settings->user = NULL;
        strcpy(settings->difficulty, "easy");
        strcpy(settings->user_color, "white");
    }
    load_settings();
    WINDOW *settings_win;
    int height = 20;
    int width = 52;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    settings_win = newwin(height, width, starty, startx);
    keypad(settings_win, TRUE);
    box(settings_win, 0, 0);

    const char *title = "SETTINGS";
    mvwprintw(settings_win, 1, (width - strlen(title)) / 2, "%s", title);

    char *difficulty_levels[] = {"easy", "medium", "hard"};
    int difficulty_index = 0;
    for (int i = 0; i < 3; i++) {
        if (strcmp(settings->difficulty, difficulty_levels[i]) == 0) {
            difficulty_index = i;
            break;
        }
    }

    char *colors[] = {"white", "blue", "green"};
    int color_index = 0;
    for (int i = 0; i < 3; i++) {
        if (strcmp(settings->user_color, colors[i]) == 0) {
            color_index = i;
            break;
        }
    }

    int highlight = 0;
    int ch;
    while (1) {
        mvwprintw(settings_win, 3, 2, "Difficulty: ");
        if (highlight == 0) {
            for (int i = 0; i < 3; i++) {
                if (i == difficulty_index) {
                    wattron(settings_win, A_REVERSE);
                    mvwprintw(settings_win, 3, 14 + i * 8, "%s", difficulty_levels[i]);
                    wattroff(settings_win, A_REVERSE);
                } else {
                    mvwprintw(settings_win, 3, 14 + i * 8, "%s", difficulty_levels[i]);
                }
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if(i == difficulty_index){
                    wattron(settings_win, A_UNDERLINE);
                    mvwprintw(settings_win, 3, 14 + i * 8, "%s", difficulty_levels[i]);
                    wattroff(settings_win, A_UNDERLINE);
                }
                else{
                    mvwprintw(settings_win, 3, 14 + i * 8, "%s", difficulty_levels[i]);
                }
            }
        }


        mvwprintw(settings_win, 5, 2, "Hero's color: ");
        if (highlight == 1) {
            for (int i = 0; i < 3; i++) {
                if (i == color_index) {
                    wattron(settings_win, A_REVERSE);
                    mvwprintw(settings_win, 5, 16 + i * 8, "%s", colors[i]);
                    wattroff(settings_win, A_REVERSE);
                } else {
                    mvwprintw(settings_win, 5, 16 + i * 8, "%s", colors[i]);
                }
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if(i == color_index){
                    wattron(settings_win, A_UNDERLINE);
                    mvwprintw(settings_win, 5, 16 + i * 8, "%s", colors[i]);
                    wattroff(settings_win, A_UNDERLINE);
                }
                else{
                    mvwprintw(settings_win, 5, 16 + i * 8, "%s", colors[i]);
                }
            }
        }

        if (highlight == 2) {
            wattron(settings_win, A_REVERSE);
            mvwprintw(settings_win, 7, 2, "Save Settings");
            wattroff(settings_win, A_REVERSE);
        } else {
            mvwprintw(settings_win, 7, 2, "Save Settings");
        }

        wrefresh(settings_win);
        ch = wgetch(settings_win);

        switch (ch) {
            case KEY_LEFT:
                if (highlight == 0 && difficulty_index > 0) {
                    difficulty_index--;
                } else if(highlight==0){
                    difficulty_index = 2;
                }
                if (highlight == 1 && color_index > 0){
                    color_index--;
                }
                else if(highlight==1){
                    color_index=2;
                }
                break;
            case KEY_RIGHT:
                if (highlight == 0 && difficulty_index < 2) {
                    difficulty_index++;
                } else if(highlight == 0){
                    difficulty_index = 0;
                }
                if (highlight == 1 && color_index < 2){
                    color_index++;
                }
                else if(highlight==1){
                    color_index=0;
                }
                break;
            case KEY_UP:
                if (highlight > 0) {
                    highlight--;
                } else {
                    highlight = 2;
                }
                break;
            case KEY_DOWN:
                if (highlight < 2) {
                    highlight++;
                } else {
                    highlight = 0;
                }
                break;
            case KEY_BACKSPACE:
                delwin(settings_win);
                clear();
                refresh();
                show_main_menu();
                return;
            case 10: // Enter key
                if (highlight == 2) {
                    strcpy(settings->difficulty, difficulty_levels[difficulty_index]);
                    strcpy(settings->user_color,colors[color_index]);
                    save_settings();
                    delwin(settings_win);
                    clear();
                    refresh();
                    show_main_menu();
                    return;
                }
                break;
            default:
                break;
        }
    }
}
