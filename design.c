#include <ncurses.h>
#include <string.h>
#include "design.h"
#include "settings.h"
#include "auth.h"
#include "game.h"
#include "utils.h"
#include "main.h"


int get_color_pair(const char *color_name) {
    if (!strcmp(color_name, "white")) {
        return PLAYER_WHITE;
    } else if (!strcmp(color_name, "blue")) {
        return PLAYER_BLUE;
    } else if (!strcmp(color_name, "green")) {
        return PLAYER_GREEN;
    }
    return 1;
}


void init_colors() {
    start_color();
    
    init_color(COLOR_CUSTOM_EXIT, 142 * 4, 179 * 4, 219 * 4); // color for exit message
    init_color(COLOR_CUSTOM_SILVER, 192 * 4, 192 * 4, 192 * 4); // silver color
    init_color(COLOR_CUSTOM_GOLD, 1000, 843, 0);
    init_color(TREASURE_ROOM_COLOR,224 * 4, 217 * 4 , 128 *  4);

    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // menu border color
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // yes button color
    init_pair(3, COLOR_RED, COLOR_BLACK);    // no button color
    init_pair(4, COLOR_BLACK, COLOR_YELLOW); // hover effect color
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   // welcome text color
    init_pair(6, COLOR_CUSTOM_EXIT, COLOR_BLACK); // goodbye text color
    init_pair(7, COLOR_CUSTOM_SILVER, COLOR_BLACK); // custom silver color
    init_pair(COLOR_CUSTOM_GOLD, COLOR_CUSTOM_GOLD, COLOR_BLACK); // custom silver color
    init_pair(PLAYER_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(PLAYER_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(PLAYER_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(TREASURE_ROOM_COLOR,TREASURE_ROOM_COLOR,COLOR_BLACK);
}

void apply_logout_design(WINDOW *logout_win) {
    wattron(logout_win, COLOR_PAIR(7) | A_BOLD | A_ITALIC);

    const char *title = "LOGOUT";
    int width = getmaxx(logout_win);
    mvwprintw(logout_win, 1, (width - strlen(title)) / 2, "%s", title);

    const char *message = "You have logged out. Press any key to continue...";
    mvwprintw(logout_win, 6, (width - strlen(message)) / 2, "%s", message);

    wrefresh(logout_win);
    wattroff(logout_win, COLOR_PAIR(7) | A_BOLD | A_ITALIC); 
    wrefresh(logout_win);
}

void PrintPlayer(WINDOW *gamewin, Player *player, Settings* settings){
    int desired_color=get_color_pair(settings->user_color);
    wattron(gamewin, COLOR_PAIR(desired_color));
    mvwprintw(gamewin, player->loc.y, player->loc.x, "@");
    wattroff(gamewin, COLOR_PAIR(desired_color));
}

void PrintGold(WINDOW *gamewin, Gold* gold , Settings* settings){
    if(gold->gtype){
        init_pair(246,246,COLOR_BLACK);
        wattron(gamewin, COLOR_PAIR(246));
        mvwprintw(gamewin,gold->loc.y,gold->loc.x,"%s",gold->code); // golds
        wattroff(gamewin, COLOR_PAIR(246));
    }else{
        wattron(gamewin, COLOR_PAIR(COLOR_CUSTOM_GOLD));
        mvwprintw(gamewin,gold->loc.y,gold->loc.x,"%s",gold->code); // golds
        wattroff(gamewin, COLOR_PAIR(COLOR_CUSTOM_GOLD));
    }
}
void PrintFood(WINDOW *gamewin, Food* food , Settings* settings){
    wattron(gamewin, COLOR_PAIR(COLOR_CYAN));
    mvwprintw(gamewin,food->loc.y,food->loc.x,"F"); // golds
    wattroff(gamewin, COLOR_PAIR(COLOR_CYAN));
}

void PrintDoor(WINDOW *gamewin, Room * room){
    for(int k=0;k<room->door_number;k++){
        if(!room->doors[k]->show){
            continue;
        }
        switch(room->doors[k]->kind){
            case HIDDEN:
                mvwprintw(gamewin,room->doors[k]->loc.y,room->doors[k]->loc.x,"?"); // doors
                break;
            case PASS:
                wattron(gamewin,COLOR_PAIR(3));
                mvwprintw(gamewin,room->doors[k]->loc.y,room->doors[k]->loc.x,"\u2761"); // doors
                wattroff(gamewin,COLOR_PAIR(3));
                break;
            case UNLOCKED:
                wattron(gamewin,COLOR_PAIR(2));
                mvwprintw(gamewin,room->doors[k]->loc.y,room->doors[k]->loc.x,"\u2761"); // doors
                wattroff(gamewin,COLOR_PAIR(2));
                break;
            case NORMAL:
                mvwprintw(gamewin,room->doors[k]->loc.y,room->doors[k]->loc.x,"+"); // doors
                break;
        }
    }
}



void PrintRoom(WINDOW *gamewin, Room *room){
    init_pair(124,124,COLOR_BLACK); // nightmare
    init_pair(165,165,COLOR_BLACK); // enchant
    switch(room->rt){
        case 0:
            break;
        case 1:
            wattron(gamewin,COLOR_PAIR(165));
            break;
        case 2:
            wattron(gamewin,COLOR_PAIR(124));
            break;
        case 3:
            wattron(gamewin,COLOR_PAIR(TREASURE_ROOM_COLOR));
            break;
    }
    for(int i=0;i<room->height;i++){
        for(int j=0;j<room->width;j++){
            if(j==room->width-1 || j==0){
                mvwprintw(gamewin,i+room->start.y,j+room->start.x,"|"); //walls
            }
            else if(i==room->height-1 || i==0){
                mvwprintw(gamewin,i+room->start.y,j+room->start.x,"_"); //walls
            }
            else{
                mvwprintw(gamewin,i+room->start.y,j+room->start.x,"."); //floors
            }
            if(i==0 && (j==0 || j==room->width-1)){
                mvwprintw(gamewin,i+room->start.y,j+room->start.x," "); //room-edges
            }

        }
    }
    PrintDoor(gamewin,room);
    for(int k=0;k<room->pillars_number;k++){
        mvwprintw(gamewin,room->pillars[k]->loc.y,room->pillars[k]->loc.x,"O"); // pillars
    }
    for(int i=0;i<room->windows_number;i++){
        mvwprintw(gamewin,room->windows[i]->loc.y,room->windows[i]->loc.x,"="); // windows
    }


    switch(room->rt){
        case 0:
            break;
        case 1:
            wattroff(gamewin,COLOR_PAIR(165));
            break;
        case 2:
            wattroff(gamewin,COLOR_PAIR(124));
            break;
        case 3:
            wattroff(gamewin,COLOR_PAIR(TREASURE_ROOM_COLOR));
            break;
    }
}