#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "game.h"
#include "menu.h"
#include "utils.h"
#include "auth.h"
#include "settings.h"
#include "design.h"
void InitLevelRoom(Level * level);
void StartGame();
void PrintLevel(Level* level);
void win_window();
int current_level;
WINDOW * gamewin;
Player *player;
void StartGame(){
    player=(Player *)malloc(sizeof(Player));
    player->golds=0;
    current_level=0;
    Level **levels=(Level **)malloc(4*sizeof(Level *));
    for(int i=0;i<4;i++){
        levels[i]=(Level *)malloc(sizeof(Level));
        levels[i]->len_rooms=6;
        levels[i]->rooms=(Room **)malloc(levels[i]->len_rooms*sizeof(Room *));
    }
    
    gamewin = newwin(win_height,win_width,0,0);
    noecho();
    keypad(gamewin,true);
    clear();
    box(gamewin,0,0);

    InitLevelRoom(levels[0]);
    int levels_initialization[4]={1,0,0,0};
    add_player_to_level(levels[0],player);
    refresh();
    int c;
    while(1){
        handleVision(levels[current_level],player);
        PrintLevel(levels[current_level]);



        if(in_staircase(levels[current_level],player->loc)){
            if(current_level<3){
                wclear(gamewin);
                if(!levels_initialization[++current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }else{
                wclear(gamewin); //  // maybe not?
                wrefresh(gamewin);
                delwin(gamewin);
                win_window();
            }
        }

        const char *title = "LEVEL: ";
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) / 2, "%s%d", title,current_level+1);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4, "Golds: %d", player->golds);
        c=wgetch(gamewin);
        switch (c){
        case KEY_BACKSPACE:
            clear();
            endwin();
            refresh();
            if(current_user!=NULL){
                current_user->golds+=player->golds;
                current_user->points+=player->golds*5;
                save_user_data(current_user); // temp
            }
            show_main_menu();
            break;
        case 'p':
            if(current_level<3){
                wclear(gamewin);
                if(!levels_initialization[++current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }
            break;
        case 'o':
            if(current_level>0){
                wclear(gamewin);
                if(!levels_initialization[--current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }
            break;

        case 'm':
            if(levels[current_level]->show){
                levels[current_level]->show=0;
                wclear(gamewin);
            }
            else{
                levels[current_level]->show=1;
            }
            break;
        default:
            break;
        }
        handlePlayermove(levels[current_level],c,player,gamewin);

        usleep(10000);
    }
}


void InitLevelRoom(Level * level){
    level->rooms=(Room **)malloc(6*sizeof(Room *));
    for(int iter=0;iter<2;iter++){
        for(int jter=0;jter<3;jter++){
            int which =(iter?5-jter:jter);
            level->rooms[which]=(Room *)malloc(sizeof(Room));
            level->rooms[which]->height=RHRG();
            level->rooms[which]->width=RWRG();
            level->rooms[which]->start.x=RXRG((MaxWidthSubwindow*jter)+2*jter,level->rooms[which]->width)+basic_padding*5;
            level->rooms[which]->start.y=RYRG((MaxHeightSubWindow*iter)+basic_padding*iter,level->rooms[which]->height)+(basic_padding*(iter+1));
            // mvwprintw(gamewin,basic_padding,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,2*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,3*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,3*basic_padding+2*MaxHeightSubWindow,basic_padding+45 ,"------------------------------------------------------------------------------------------\n");
            Room * room=level->rooms[which];
            room->show=0;

            add_doors_to_room(room,which);
            add_pillars_to_room(room);
            add_windows_to_room(room);
            add_golds_to_room(room);
            for(int i=0;i<room->golds_number;i++){
                if(!strcmp(settings->difficulty,"hard")){
                    room->golds[i]->value=1;
                    player->health=50;
                }
                else if(!strcmp(settings->difficulty,"medium")){
                    room->golds[i]->value=2;
                    player->health=70;
                }
                else if(!strcmp(settings->difficulty,"easy")){
                    room->golds[i]->value=3;
                    player->health=100;
                }
            }
            // mvwprintw(gamewin,0,0,"%d,%d",level->rooms[0]->doors[0]->loc.y,level->rooms[0]->doors[0]->loc.x);
            // mvwprintw(gamewin,0,0,"%d,%d",room->doors[3]->loc.y,room->doors[3]->loc.x);

            // mvwprintw(gamewin,38+(which),25,"{%d}",room->door_number);
        }
    }
    add_corridors_to_level(level,gamewin);
    add_staircase_to_level(level);
    // for(int which=0;which<6;which++){
    //     Room * room=level->rooms[which];
    //     mvwprintw(gamewin,38+(which),3,"{h:%d w:%d y:%d x:%d}",room->height,room->width,room->start.y,room->start.x);    
    // }
    level->show=0;
}




void PrintLevel(Level* level){
    mvwprintw(gamewin,3,1,"show:%d",level->show);
    init_colors();
    for(int which=0;which<level->len_rooms;which++){
        Room * room=level->rooms[which];
        if(!room->show && !level->show){
            continue;
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
                for(int k=0;k<room->door_number;k++){
                    mvwprintw(gamewin,room->doors[k]->loc.y,room->doors[k]->loc.x,"+"); // doors
                }
                for(int k=0;k<room->pillars_number;k++){
                    mvwprintw(gamewin,room->pillars[k]->loc.y,room->pillars[k]->loc.x,"O"); // pillars
                }
                for(int i=0;i<room->windows_number;i++){
                    mvwprintw(gamewin,room->windows[i]->loc.y,room->windows[i]->loc.x,"="); // windows
                }
                for(int i=0;i<room->golds_number;i++){
                    if(!room->golds[i]->taken){
                        PrintGold(gamewin,room->golds[i],settings);
                    }
                }


                mvwprintw(gamewin,38+(which),3,"{h:%d w:%d y:%d x:%d}",room->height,room->width,room->start.y,room->start.x);
                for(int i=0;i<room->door_number;i++){
                    mvwprintw(gamewin,38+(which),25+10*i,"(%d,%d)",room->doors[i]->loc.y,room->doors[i]->loc.x);
                }
            }
        }
    }

    for(int i=0;i<level->corrs_number;i++){
        for(int j=0;j<level->corrs[i]->locs_count;j++){
            if(level->corrs[i]->show[j] || level->show){
                mvwprintw(gamewin, level->corrs[i]->locs[j].y, level->corrs[i]->locs[j].x,"#"); // corridors
            }
        }
    }
    if(level->rooms[level->len_rooms-1]->show || level->show){
        mvwprintw(gamewin, level->staircase->loc.y, level->staircase->loc.x, "<"); // staircase
    }

    PrintPlayer(gamewin,player,settings);
}




void win_window(){
    // pre configuration
    int height = 7;
    int width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win_win = newwin(height, width, starty, startx);
    keypad(win_win, TRUE); // enable keypad
    box(win_win, 0, 0);
    curs_set(0);
    const char *win_text = "You Won! Press any key to return to main menu...";
    mvwprintw(win_win, 1, (width - strlen(win_text)) / 2, "%s", win_text);
    wrefresh(win_win);
    wgetch(win_win);
    wclear(win_win);
    delwin(win_win);
    if(current_user!=NULL){
        current_user->golds+=player->golds;
        current_user->points+=player->golds*5;
        save_user_data(current_user); // temp
    }
    show_main_menu();
}