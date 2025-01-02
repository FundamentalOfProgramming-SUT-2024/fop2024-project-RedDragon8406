#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include "scoreboard.h"
#include "menu.h"
#include "main.h"
#include "auth.h"

UserData *user_points[MAX_USERS];
int user_count;

void load_user_data() {
    user_count = 0;
    FILE *file = fopen("users_list.db", "r");
    if (file != NULL) {
        char username[MAX_USERNAME_LENGTH];
        while (fscanf(file, "%s", username) == 1) {
            UserData *user = malloc(sizeof(UserData));
            strncpy(user->username, username, MAX_USERNAME_LENGTH);
            if (read_user_data(user)) {
                user_points[user_count++] = user;
            }
        }
        fclose(file);
    }
}


void sort_users_by_points(){
    for(int i=0;i<user_count-1;i++){
        for(int j=i+1;j<user_count;j++){
            if(user_points[j]->points>user_points[i]->points){
                UserData *temp=user_points[i];
                user_points[i]=user_points[j];
                user_points[j]=temp;
            }
        }
    }
}

void show_scoreboard() {
    load_user_data();
    sort_users_by_points();
    WINDOW *scoreboard_win;
    int height = 20;
    int width = 52;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    scoreboard_win = newwin(height, width, starty, startx);
    keypad(scoreboard_win, TRUE);
    box(scoreboard_win, 0, 0);

    const char *title = "SCOREBOARD";
    mvwprintw(scoreboard_win, 1, (width - strlen(title)) / 2, "%s", title);

    mvwprintw(scoreboard_win, 3, 2, "Rank");
    mvwprintw(scoreboard_win, 3, 8, "Username");
    mvwprintw(scoreboard_win, 3, 28, "Points");
    mvwprintw(scoreboard_win, 3, 38, "Golds");
    mvwprintw(scoreboard_win, 3, 48, "XP");

    for (int i = 0; i < 5; i++) {
        if (i >= user_count) {
            break;
        }
        mvwprintw(scoreboard_win, i + 5, 2, "%d", i + 1);
        if(current_user){
            if(!strcmp(user_points[i]->username,current_user->username)){
                mvwprintw(scoreboard_win, i + 5, 5, "--> %s", user_points[i]->username);
            }
            else{
                mvwprintw(scoreboard_win, i + 5, 9, "%s", user_points[i]->username);
            }
        }
        else{
            mvwprintw(scoreboard_win, i + 5, 9, "%s", user_points[i]->username);
        }
        mvwprintw(scoreboard_win, i + 5, 30, "%d", user_points[i]->points);
        mvwprintw(scoreboard_win, i + 5, 40, "%d", user_points[i]->golds);
        mvwprintw(scoreboard_win, i + 5, 49, "%d", user_points[i]->xp);
    }

    wrefresh(scoreboard_win);

    int ch;
    while ((ch = wgetch(scoreboard_win)) != KEY_BACKSPACE) {
        continue;
    }

    delwin(scoreboard_win);
    clear();
    refresh();
    show_main_menu();
}

