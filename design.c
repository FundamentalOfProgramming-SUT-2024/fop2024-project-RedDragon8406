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
    // wattron(gamewin, COLOR_PAIR(COLOR_CYAN));
    mvwprintw(gamewin,food->loc.y,food->loc.x,"%s",food->code); // golds
    // wattroff(gamewin, COLOR_PAIR(COLOR_CYAN));
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


void PrintHS(WINDOW *win,Player *player, int x, int y, int padding){
    int hc=(player->health-1) * 10 / MAXHEALTH + 1;
    int sc=(player->sat-1) * 10 / MAXSAT + 1;
    mvwprintw(win,y,x,"                     ");
    mvwprintw(win,y+padding,x,"                     ");
    for(int i=0;i<hc;i++){
        if(hc<=3){
            mvwprintw(win,y,x+2*i,"\u2764");
        }else if(hc <= 6){
            mvwprintw(win,y,x+2*i,"\U0001F49B");
        }else{
            mvwprintw(win,y,x+2*i,"\U0001F49A");
        }
    }
    for(int i=0;i<sc;i++){
        mvwprintw(win,y+padding,x+2*i,"\U0001F352");
    }
}


void welcome_message(WINDOW *chatwin){
    if(current_level==4){
        wattron(chatwin,A_ITALIC);
        mvwprintw(chatwin,1+countchat,1,"Welcome to The Trasure Room!");
        wattroff(chatwin,A_ITALIC);

    }else{
        wattron(chatwin,A_ITALIC);
        mvwprintw(chatwin,1+countchat,1,"Welcome to Level %d!",current_level+1);
        wattroff(chatwin,A_ITALIC);
    }
    countchat++;
}

void chatclear(WINDOW *chatwin){
    for(int i=1;i<chat_width;i++){
        for(int j=1;j<=2;j++){
            mvwprintw(chatwin,j,i," ");
        }
    }
}

void chat_new_room(WINDOW *chatwin){
    wattron(chatwin,A_ITALIC);
    mvwprintw(chatwin,1+countchat,1,"Entered a new room!");
    wattroff(chatwin,A_ITALIC);
    countchat++;
}

void chat_gold(WINDOW *chatwin, Gold *gold){
    init_pair(220,220,COLOR_BLACK);
    wattron(chatwin,A_ITALIC);
    wattron(chatwin,COLOR_PAIR(220));
    switch (gold->gtype)
    {
    case GOLD:

        mvwprintw(chatwin,1+countchat,1,"You recieved a normal gold.");
        break;
    case BLACK:
        mvwprintw(chatwin,1+countchat,1,"You recieved a black gold.");
        break;
    default:
        break;
    }
    wattroff(chatwin,COLOR_PAIR(220));
    wattroff(chatwin,A_ITALIC);
    countchat++;
}


void chat_enemy_hit(WINDOW *chatwin, Enemy *enemy){
    init_pair(160,160,COLOR_BLACK);
    wattron(chatwin,A_ITALIC);
    wattron(chatwin,COLOR_PAIR(160));
    switch (enemy->en)
    {
    case DEAMON:
         mvwprintw(chatwin,1+countchat,1,"Deamon gave you %d damage :(",5);
        break;
    case FBM:
         mvwprintw(chatwin,1+countchat,1,"Fire Breathing Monster gave you %d damage :(",10);
        break;
    case GIANT:
         mvwprintw(chatwin,1+countchat,1,"Giant gave you %d damage :(",15);
        break;
    case SNAKE:
         mvwprintw(chatwin,1+countchat,1,"Snake gave you %d damage :(",20);
        break;
    case UNDEED:
        mvwprintw(chatwin,1+countchat,1,"Undeed gave you %d damage :(",30);
        break;
    default:
        break;
    }
    wattroff(chatwin,COLOR_PAIR(160));
    wattroff(chatwin,A_ITALIC);
    countchat++;
}


void chat_hit_enemy(WINDOW *chatwin, Player *player,Enemy *enemy){
    char hwep[40];
    char ename[40];
    switch(player->current_weapon->weapon){
        case MACE:
            strcpy(hwep,"Mace");
            break;
        case DAGGER:
            strcpy(hwep,"Dagger");
            break;
        case WAND:
            strcpy(hwep,"Magic Wand");
            break;
        case ARROW:
            strcpy(hwep,"Arrow");
            break;
        case SWORD:
            strcpy(hwep,"Sword");
            break;
    }
    switch (enemy->en){
        case DEAMON:
            strcpy(ename,"Deamon");
            break;
        case FBM:
            strcpy(ename,"Fire Breathing Monster");
            break;
        case GIANT:
            strcpy(ename,"Giant");
            break;
        case SNAKE:
            strcpy(ename,"Snake");
            break;
        case UNDEED:
            strcpy(ename,"Undeed");
            break;
    }

    init_pair(124,124,COLOR_BLACK);
    wattron(chatwin,A_ITALIC);
    wattron(chatwin,COLOR_PAIR(124));
    if(enemy->health>0){
        mvwprintw(chatwin,1+countchat,1,"You hit %s with your mighty %s! Remaining Health: %d",ename,hwep,enemy->health);
    }else{
        mvwprintw(chatwin,1+countchat,1,"You killed %s with your mighty %s.",ename,hwep);
    }
    wattroff(chatwin,A_ITALIC);
    wattroff(chatwin,COLOR_PAIR(124));
    countchat++;
}

void potion_situation(WINDOW *gamewin, Player *player){
    init_pair(168,168,COLOR_BLACK); // r
    init_pair(83,83,COLOR_BLACK); //g
    init_pair(146,146,COLOR_BLACK); // fake b lol
    int padding=0;
    for(int i=0;i<3;i++){
        if(player->pcof[i]!=1){
            padding+=1;
        }
    }
    int cp=padding;
    for(int i=0;i<3;i++){
        char text[40];
        if(player->pcof[i]==1){
            continue;
        }
        switch(i){
            case SPEED:
                wattron(gamewin,COLOR_PAIR(83));
                snprintf(text,sizeof(text),"Speedy! \U0001F37E");
                break;
            case DAMAGE:
                wattron(gamewin,COLOR_PAIR(146));
                snprintf(text,sizeof(text),"Powerful! \U0001F37C");
                break;
            case HEALTH:
                wattron(gamewin,COLOR_PAIR(168));
                snprintf(text,sizeof(text),"Healthy! \U0001F377");
                break;
        }
        mvwprintw(gamewin,8 + padding-cp,1,"%s",text);
        switch(i){
            case SPEED:
                wattroff(gamewin,COLOR_PAIR(83));
                break;
            case DAMAGE:
                wattroff(gamewin,COLOR_PAIR(146));
                break;
            case HEALTH:
                wattroff(gamewin,COLOR_PAIR(168));
                break;
        }
        cp--;
    }
}
void show_current_weapon(WINDOW *gamewin, Player *player){
    mvwprintw(gamewin,6,1,"Current Weapon: %s ",player->current_weapon->code);
}