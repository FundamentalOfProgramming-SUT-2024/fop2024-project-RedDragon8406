#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include "settings.h"
#include "menu.h"
#include "auth.h"

Settings* settings=NULL;

void save_settings() {
    
    char filepath[250];
    if (settings->user == NULL) {
        snprintf(filepath, sizeof(filepath), "settings.db");
    }
    else{
        snprintf(filepath, sizeof(filepath), "./settings/%s.db", settings->user->username);
    }
    FILE* file = fopen(filepath, "w");
    if(settings->user==NULL){
        
        fprintf(file, "DIFFICULTY %s\n", "easy");
        fprintf(file, "COLOR %s\n", "white");
        fclose(file);
    }
    fprintf(file, "DIFFICULTY %s\n", settings->difficulty);
    fprintf(file, "COLOR %s\n", settings->user_color);
    fclose(file);
}
void create_settings(const char * username) {
    char filepath[250];
    snprintf(filepath, sizeof(filepath), "./settings/%s.db", username);
    FILE* file = fopen(filepath, "w");
    fprintf(file, "DIFFICULTY %s\n", "easy");
    fprintf(file, "COLOR %s\n", "white");
    fclose(file);
}

void load_settings() {
    char filepath[250];
    if (settings->user == NULL) {
        snprintf(filepath, sizeof(filepath), "settings.db");
    }
    else{
        snprintf(filepath, sizeof(filepath), "./settings/%s.db", settings->user->username);
    }
    FILE* file = fopen(filepath, "r");
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "DIFFICULTY %s", settings->difficulty) == 1) {
            if (settings->difficulty[strlen(settings->difficulty) - 1] == '\n') {
                settings->difficulty[strlen(settings->difficulty) - 1] = '\0';
            }
            continue;
        }
        if (sscanf(line, "COLOR %s", settings->user_color) == 1) {
            if (settings->user_color[strlen(settings->user_color) - 1] == '\n') {
                settings->user_color[strlen(settings->user_color) - 1] = '\0';
            }
            continue;
        }
    }
    fclose(file);
}

void settings_window(){
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

    mvwprintw(settings_win, 3, 2, "Difficulty: ");
    mvwprintw(settings_win, 3, 14, "%s",settings->difficulty);
    mvwprintw(settings_win, 5, 2, "Hero's Color: ");
    mvwprintw(settings_win, 5, 16, "%s",settings->user_color);

    wrefresh(settings_win);

    int ch;
    while ((ch = wgetch(settings_win)) != KEY_BACKSPACE) {
        continue;
    }

    delwin(settings_win);
    clear();
    refresh();
    show_main_menu();
}