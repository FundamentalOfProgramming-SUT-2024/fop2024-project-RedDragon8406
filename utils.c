#include <stdlib.h>
#include <string.h>
#include "game.h"


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

void add_doors_to_room(Room *room) {
    room->door_number = (rand() % 3) + 1; // Random number between 1 and 3
    initialize_doors(room);

    for (int i = 0; i < room->door_number; i++) {
        int border = rand() % 4; // Randomly choose a border (0: top, 1: bottom, 2: left, 3: right)
        switch (border) {
            case 0: // Top border
                room->doors[i]->cord.x = room->start.x + (rand() % room->width-1) + 1;
                room->doors[i]->cord.y = room->start.y;
                break;
            case 1: // Bottom border
                room->doors[i]->cord.x = room->start.x + (rand() % room->width-1 ) + 1;
                room->doors[i]->cord.y = room->start.y + room->height-1;
                break;
            case 2: // Left border
                room->doors[i]->cord.x = room->start.x;
                room->doors[i]->cord.y = room->start.y + (rand() % room->height-1 ) + 1;
                break;
            case 3: // Right border
                room->doors[i]->cord.x = room->start.x + room->width-1;
                room->doors[i]->cord.y = room->start.y + (rand() % room->height-1 ) + 1;
                break;
        }
    }
}
