#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "menu.h"
#include "utils.h"

void InitLevelRoom(Level * level);
int current_level;

WINDOW * gamewin;
void StartGame(){
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
    refresh();
    int c;
    while(1){
        const char *title = "LEVEL: ";
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) / 2, "%s%d", title,current_level+1);
        c=wgetch(gamewin);
        switch (c){
        case KEY_BACKSPACE:
            clear();
            endwin();
            refresh();
            show_main_menu();
            break;
        case 'p':
            if(current_level<3){
                wclear(gamewin);
                InitLevelRoom(levels[++current_level]);
            }
            break;
        case 'o':
            if(current_level>0){
                wclear(gamewin);
                InitLevelRoom(levels[--current_level]);
            }
            break;
        default:
            break;
        }
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
            mvwprintw(gamewin,basic_padding,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            mvwprintw(gamewin,basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            mvwprintw(gamewin,2*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            mvwprintw(gamewin,3*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            mvwprintw(gamewin,3*basic_padding+2*MaxHeightSubWindow,basic_padding+45 ,"------------------------------------------------------------------------------------------\n");
            Room * room=level->rooms[which];
           
            add_doors_to_room(room,which);
            add_pillars_to_room(room);
            // mvwprintw(gamewin,0,0,"%d,%d",level->rooms[0]->doors[0]->loc.y,level->rooms[0]->doors[0]->loc.x);
            // mvwprintw(gamewin,0,0,"%d,%d",room->doors[3]->loc.y,room->doors[3]->loc.x);

            // mvwprintw(gamewin,38+(which),3,"{h:%d w:%d y:%d x:%d} {%d %d}",room->height,room->width,room->start.y,room->start.x,(MaxHeightSubWindow*jter)+2*jter,(MaxWidthSubwindow*iter)+2*iter);
            // mvwprintw(gamewin,38+(which),25,"{%d}",room->door_number);
        }
    }
    add_corridors_to_room(level,gamewin);
    for(int iter=0;iter<2;iter++){
        for(int jter=0;jter<3;jter++){
            int which=(iter?5-jter:jter);
            Room * room=level->rooms[which];
            for(int i=0;i<room->door_number;i++){
                mvwprintw(gamewin,38+(which),30+10*i,"(%d,%d)",room->doors[i]->loc.y,room->doors[i]->loc.x);
            }
            for(int i=0;i<room->height;i++){
                for(int j=0;j<room->width;j++){
                    if(j==room->width-1 || j==0){
                        mvwprintw(gamewin,i+room->start.y,j+room->start.x,"|");
                    }
                    else if(i==room->height-1 || i==0){
                        mvwprintw(gamewin,i+room->start.y,j+room->start.x,"_");
                    }
                    else{
                        mvwprintw(gamewin,i+room->start.y,j+room->start.x,".");
                    }
                    if(i==0 && (j==0 || j==room->width-1)){
                        mvwprintw(gamewin,i+room->start.y,j+room->start.x," ");
                    }
                    for(int k=0;k<room->door_number;k++){
                        if(i+room->start.y==room->doors[k]->loc.y && j+room->start.x==room->doors[k]->loc.x){
                            mvwprintw(gamewin,i+room->start.y,j+room->start.x,"+");
                        }
                    }
                    for(int k=0;k<room->pillars_number;k++){
                        mvwprintw(gamewin,room->pillars[k]->loc.y,room->pillars[k]->loc.x,"O");
                    }
                }
            }
            if(which == 0){
                for(int i=0;i<level->corrs_number;i++){
                    for(int j=0;j<level->corrs[i]->locs_count;j++){
                        mvwprintw(gamewin, level->corrs[i]->locs[j].y, level->corrs[i]->locs[j].x,"#");
                    }
                }
            }
        }
    }
    for(int which=0;which<6;which++){
        Room * room=level->rooms[which];
        mvwprintw(gamewin,38+(which),3,"{h:%d w:%d y:%d x:%d}",room->height,room->width,room->start.y,room->start.x);    
    }
}

