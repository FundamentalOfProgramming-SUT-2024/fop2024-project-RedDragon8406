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
    for(int iter=0;iter<3;iter++){
        for(int jter=0;jter<2;jter++){
            Room * room=level->rooms[2*iter+jter];
            room=(Room *)malloc(sizeof(Room));
            room->height=RHRG();
            room->width=RWRG();
            room->start.x=RXRG((MaxWidthSubwindow*iter)+2*iter,room->width);
            room->start.y=RYRG((MaxHeightSubWindow*jter)+2*jter,room->height);
            add_doors_to_room(room);
            mvwprintw(gamewin,38+(2*iter+jter),3,"{%d %d %d %d} {%d %d}",room->height,room->width,room->start.y,room->start.x,(MaxHeightSubWindow*jter)+2*jter,(MaxWidthSubwindow*iter)+2*iter);
            mvwprintw(gamewin,38+(2*iter+jter),25,"{%d}",room->door_number);
            for(int i=0;i<room->door_number;i++){
                mvwprintw(gamewin,38+(2*iter+jter),30+10*i,"{%d,%d}",room->doors[i]->cord.y,room->doors[i]->cord.x);
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
                        if(i+room->start.y==room->doors[k]->cord.y && j+room->start.x==room->doors[k]->cord.x){
                            mvwprintw(gamewin,i+room->start.y,j+room->start.x,"+");
                        }
                    }
                }
            }
        }
    }
}

