#include <stdlib.h>
#include <string.h>
#include "game.h"
#include <ncurses.h>
#include <time.h>
/*
        |   |          |   |
        |   |          |   |        
--------------------------------
---------------------------------
        |   |          |   |
        |   |          |   |

20 for width of each
10 for height of each
10 for both x,y of blank space between each subwindow

*/
int RHRG(){
    int y;
    y = (rand() % (MaxHeightRoom - MinHeightRoom)) + MinHeightRoom;
    return y;
}
int RWRG(){
    int x;
    x = (rand() % (MaxWidthRoom - MinWidthRoom)) + MinWidthRoom;
    return x;
}

int RYRG(int y_subwindow,int height_room){
    int y;
    y = (rand() % (MaxHeightSubWindow-height_room-1)) + y_subwindow;
    return y;
}
int RXRG(int x_subwindow,int widthroom){
    int x;
    x = (rand() % (MaxWidthSubwindow-widthroom-1)) + x_subwindow;
    return x;
}

void initialize_doors(Room *room) {
    room->doors = malloc(room->door_number * sizeof(Door*));
    for (int i = 0; i < room->door_number; i++) {
        room->doors[i] = malloc(sizeof(Door));
    }
}

void add_doors_to_room(Room *room,int which) {
    if(which==0){
        room->door_number = 1;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2) ) + 1; // bottom
        room->doors[0]->loc.y = room->start.y + room->height-1;
    }
    else if(which==5){
        room->door_number = 1;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2)) + 1; // top
        room->doors[0]->loc.y = room->start.y;
    }
    else if(which==2 || which==3){
        room->door_number = 2;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2)) + 1; // top
        room->doors[0]->loc.y = room->start.y;
        room->doors[1]->loc.x = room->start.x + (rand() % (room->width-2) ) + 1; // bottom
        room->doors[1]->loc.y = room->start.y + room->height-1;
    }
    else if(which==1 || which==4){
        room->door_number = 2;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2) ) + 1; // bottom
        room->doors[0]->loc.y = room->start.y + room->height-1;
        room->doors[1]->loc.x = room->start.x + (rand() %( room->width-2)) + 1; // top
        room->doors[1]->loc.y = room->start.y;
    }
}

void add_pillars_to_room(Room *room){
    room->pillars_number= rand() % ((room->height*room->width)/30);
    room->pillars=(Pillar **)malloc(room->pillars_number*sizeof(Pillar));
    for(int i=0;i<room->pillars_number;i++){
        Point first_guess;
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                j=0;
                first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
                first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
            }
        }
        room->pillars[i]=(Pillar *)malloc(sizeof(Pillar));
        room->pillars[i]->loc.x=first_guess.x;
        room->pillars[i]->loc.y=first_guess.y;
    }
}


void add_windows_to_room(Room *room){
    int how_many;
    // srand(time(NULL));
    how_many = (rand() % 3);
    int which;
    if(how_many==1){
        which= rand()%2;
    }
    room->windows_number=how_many;
    for(int i=0;i<how_many;i++){
        room->windows[i]=(Window *)malloc(sizeof(Window));
    }
    if(how_many==1){
        if(which){
            int y= (rand() % (room->height-2))+1;
            room->windows[0]->loc.x=room->start.x+room->width-1;
            room->windows[0]->loc.y=room->start.y+y;
        }else{
            int y= (rand() % (room->height-2))+1;
            room->windows[0]->loc.x=room->start.x;
            room->windows[0]->loc.y=room->start.y+y;
        }
        room->windows[0]->side=which;
    }
    else{
        for(int i=0;i<room->windows_number;i++){
            int y= (rand() % (room->height-2))+1;
            room->windows[i]->loc.x=room->start.x + (i?room->width-1:0);
            room->windows[i]->loc.y=room->start.y+y;
            room->windows[i]->side=i;
        }
    }
}

void add_staircase_to_level(Level *level){
    Room *room=level->rooms[level->len_rooms-1];
    Point first_guess;
    first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
    first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
    for(int j=0;j<room->pillars_number;j++){
        if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
            first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
            first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
            j=0;
        }
    }
    level->staircase=(Staircase *)malloc(sizeof(Staircase));
    level->staircase->loc.x=first_guess.x;
    level->staircase->loc.y=first_guess.y;
}










void add_corridors_to_level(Level *level,WINDOW *gamewin){
    Room **rooms=level->rooms;
    level->corrs=(Corridor **)malloc(5*sizeof(Corridor));
    for(int i=0;i<5;i++){
        level->corrs[i]=(Corridor *)malloc(sizeof(Corridor));
    }
    level->corrs_number=5;
    Corridor **corrs=level->corrs;
    int which,random_down,random_up,static_down,static_up,static_narrow,down;
    Point n1,n2;

    which=0;        // -------------------------------------------- first --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[(which?1:0)]; // bottom
    corrs[which]->node2=rooms[which+1]->doors[0]; // bottom
    // mvwprintw(gamewin,0,0,"%d,%d",rooms[which]->doors[0]->loc.y,rooms[which]->doors[0]->loc.x);
    random_down= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_down= (which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding) - n1.y+random_down;
    static_up=(which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding)- n2.y+random_down;
    static_narrow = n2.x - n1.x-1; // -1 is for not counting the first and last ones
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    // mvwprintw(gamewin,0,0,"%d %d %d",static_down,static_narrow,static_up);
    // mvwprintw(gamewin,1,0,"%d %d, %d %d",n1.y,n1.x,n2.y,n2.x);
    // mvwprintw(gamewin,2,0,"%d %d",(which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding),random_down);
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y+i+1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_down].x=n1.x+i+1;
        corrs[which]->locs[i+static_down].y=n1.y+static_down;
    }
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i+static_down+static_narrow].x=n1.x+static_narrow+1;
        corrs[which]->locs[i+static_down+static_narrow].y=n1.y+static_down-i;
    }



    which++; // -------------------------------------------- second --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[1]; // top
    corrs[which]->node2=rooms[which+1]->doors[0]; // top        
    random_up= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_up= -(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding) +n1.y+random_up;
    static_down=-(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding)+n2.y+random_up;
    static_narrow = n2.x - n1.x-1; // -1 is for not counting the first and last ones
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    // mvwprintw(gamewin,0,0,"%d %d %d",static_down,static_narrow,static_up);
    // mvwprintw(gamewin,1,0,"%d %d, %d %d",n1.y,n1.x,n2.y,n2.x);
    // mvwprintw(gamewin,2,0,"%d %d",(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding),random_up);
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y-i-1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_up].x=n1.x+i+1;
        corrs[which]->locs[i+static_up].y=n1.y-static_up;
    }
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i+static_up+static_narrow].y=n1.y-static_up+i;
        corrs[which]->locs[i+static_up+static_narrow].x=n1.x+static_narrow+1;
    }


    which++; // -------------------------------------------- third --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[1]; // bottom
    corrs[which]->node2=rooms[which+1]->doors[0]; // top        
    random_down= ( rand() % (2*basic_padding-1))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_down=MaxHeightSubWindow+basic_padding-n1.y + random_down;
    static_narrow=n2.x-n1.x;
    down=n2.y-n1.y-static_down;
    int way;
    if(static_narrow>0){
        static_narrow-=1;
        way=1;
    }
    else if(static_narrow<0){
        static_narrow=-static_narrow;
        static_narrow-=1;
        way=-1;
    }
    else{
        way=0;
    }
    corrs[which]->locs_count=static_down+static_narrow+down;
    corrs[which]->locs=(Point *)malloc((static_down+down+static_narrow)*sizeof(Point));
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y+i+1;
    }
    for(int i=0;i<static_narrow;i++){
        if(way>0){
            corrs[which]->locs[static_down+ i].x=n1.x+i+1;
            corrs[which]->locs[static_down + i].y=n1.y+static_down;
        }
        else if(way<0){
            corrs[which]->locs[static_down+ i].x=n1.x-i-1;
            corrs[which]->locs[static_down + i].y=n1.y+static_down;
        }
    }
    for(int i=0;i<down;i++){
        corrs[which]->locs[static_down+static_narrow+ i].x=n1.x+ way*(static_narrow+1);
        corrs[which]->locs[static_down+static_narrow+ i].y=n1.y+static_down+i;
    }
    mvwprintw(gamewin,0,0,"%d %d %d, count:%d way:%d",static_down,static_narrow,static_up,corrs[which]->locs_count,way);
    mvwprintw(gamewin,1,0,"%d %d, %d %d",n1.y,n1.x,n2.y,n2.x);
    // mvwprintw(gamewin,2,0,"%d %d",(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding),random_up);
    mvwprintw(gamewin,0,win_width/2,"%d %d %d",static_down,static_narrow,down);


    which++; // -------------------------------------------- fourth --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[(which?1:0)]; // bottom
    corrs[which]->node2=rooms[which+1]->doors[0]; // bottom
    // mvwprintw(gamewin,0,0,"%d,%d",rooms[which]->doors[0]->loc.y,rooms[which]->doors[0]->loc.x);
    random_down= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_down= (which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding) - n1.y+random_down;
    static_up=(which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding)- n2.y+random_down;
    static_narrow = n1.x - n2.x-1; // -1 is for not counting the first and last ones
    way=-1;
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    // mvwprintw(gamewin,0,0,"%d %d %d",static_down,static_narrow,static_up);
    // mvwprintw(gamewin,1,0,"%d %d, %d %d",n1.y,n1.x,n2.y,n2.x);
    // mvwprintw(gamewin,2,0,"%d %d",(which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding),random_down);
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y+i+1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_down].x=n1.x-i-1;
        corrs[which]->locs[i+static_down].y=n1.y+static_down;
    }
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i+static_down+static_narrow].x=n1.x-static_narrow-1;
        corrs[which]->locs[i+static_down+static_narrow].y=n1.y+static_down-i;
    }


    which++; // -------------------------------------------- fifth --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[1]; // top
    corrs[which]->node2=rooms[which+1]->doors[0]; // top        
    random_up= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_up= -(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding) +n1.y+random_up;
    static_down=-(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding)+n2.y+random_up;
    static_narrow = n1.x - n2.x-1; // -1 is for not counting the first and last ones
    way=-1;
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    // mvwprintw(gamewin,0,0,"%d %d %d",static_down,static_narrow,static_up);
    // mvwprintw(gamewin,1,0,"%d %d, %d %d",n1.y,n1.x,n2.y,n2.x);
    // mvwprintw(gamewin,2,0,"%d %d",(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding),random_up);
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y-i-1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_up].x=n1.x-i-1;
        corrs[which]->locs[i+static_up].y=n1.y-static_up;
    }
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i+static_up+static_narrow].y=n1.y-static_up+i;
        corrs[which]->locs[i+static_up+static_narrow].x=n1.x-static_narrow-1;
    }
    
}