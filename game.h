#ifndef GAME_H
#define GAME_H

#define win_height 45
#define win_width 150

#define MaxHeightRoom 10
#define MaxWidthRoom 35

#define MinHeightRoom 5 // it's actually 4| 0 , 1->4 , 5 as for the walls
#define MinWidthRoom 5 // same thing

#define MaxHeightSubWindow 12
#define MaxWidthSubwindow 40



typedef struct{
    int x;
    int y;
} Point;

typedef struct{
    Point start;
    int height;
    int width;
} Room;

typedef struct{
    Room **rooms;
    int len_rooms;
    int level_number;
} Level;


void StartGame();
#endif