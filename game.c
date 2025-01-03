#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "menu.h"
#include "utils.h"

void StartGame(){
    WINDOW * gamewin;
    Level *level_1=(Level *)malloc(sizeof(Level));
    level_1->level_number=1;
    level_1->len_rooms=6;
    level_1->rooms=(Room **)malloc(level_1->len_rooms*sizeof(Room *));

    gamewin = newwin(win_height,win_width,0,0);
    noecho();
    keypad(gamewin,true);
    clear();
    box(gamewin,0,0);
    for(int iter=0;iter<3;iter++){
        for(int jter=0;jter<2;jter++){
            Room * room=level_1->rooms[2*iter+jter];
            room=(Room *)malloc(sizeof(Room));
            room->height=RHRG();
            room->width=RWRG();
            room->start.x=RXRG((MaxWidthSubwindow*iter)+2*iter,room->width);
            room->start.y=RYRG((MaxHeightSubWindow*jter)+2*jter,room->height);
            mvwprintw(gamewin,38+(2*iter+jter),3,"{%d %d %d %d} {%d %d}\n",room->height,room->width,room->start.y,room->start.x,(MaxHeightSubWindow*jter)+2*jter,(MaxWidthSubwindow*iter)+2*iter);
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
                }
            }
        }
    }


    refresh();
    int c;
    while(1){
        c=wgetch(gamewin);
        switch (c){
        case KEY_BACKSPACE:
            clear();
            endwin();
            refresh();
            show_main_menu();
            break;
        default:
            break;
        }
    }
}