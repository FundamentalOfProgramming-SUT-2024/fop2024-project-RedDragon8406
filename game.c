#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "menu.h"


void StartGame(){
    WINDOW * gamewin;
    Level *level_1=(Level *)malloc(sizeof(Level));
    Room *room_1=(Room *)malloc(sizeof(Room));
    room_1->height=10;
    room_1->width=30;;
    room_1->start.x=5;
    room_1->start.y=5;

    level_1->level_number=1;
    level_1->len_rooms=1;
    level_1->rooms=(Room **)malloc(level_1->len_rooms*sizeof(Room *));
    level_1->rooms[0]=room_1;
    gamewin = newwin(win_height,win_width,0,0);
    noecho();
    keypad(gamewin,true);
    clear();
    box(gamewin,0,0);
    mvwprintw(gamewin,10,10,"here at least!\n");
    refresh();
    wgetch(gamewin);
    for(int i=0;i<room_1->height;i++){
        for(int j=0;j<room_1->width;j++){
            if(i==room_1->height-1 || i==0){
                mvwprintw(gamewin,i+room_1->start.x,j+room_1->start.y,"-");
            }
            else if(j==room_1->width-1 || j==0){
                mvwprintw(gamewin,i+room_1->start.x,j+room_1->start.y,"|");
            }
            else{
                mvwprintw(gamewin,i+room_1->start.x,j+room_1->start.y,".");
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