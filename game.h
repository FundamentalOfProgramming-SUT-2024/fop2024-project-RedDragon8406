#ifndef GAME_H
#define GAME_H

#define win_height 45
#define win_width 150

#define basic_padding 5

#define MaxHeightRoom 10
#define MaxWidthRoom 35

#define MinHeightRoom 5 // it's actually 4| 0 , 1->4 , 5 as for the walls
#define MinWidthRoom 5 // same thing

#define MaxHeightSubWindow 12
#define MaxWidthSubwindow 40

extern int current_level;

typedef struct{
    int x;
    int y;
} Point;

typedef struct{
    Point loc;
} Door;

typedef struct{
    Door *node1;
    Door *node2;
    Point *locs;
    int locs_count;
} Corridor;


typedef struct{
    Point loc;
} Pillar;
typedef struct{
    Point loc;
    int side;
} Window;

typedef struct{
    Point loc;
} Staircase;

typedef struct{
    Point start;
    int height;
    int width;
    Door **doors;
    int door_number;
    Pillar **pillars;
    int pillars_number;
    Window *windows[2];
    int windows_number;
} Room;

typedef struct{
    Room **rooms;
    int len_rooms;
    Corridor **corrs;
    int corrs_number;
    Staircase *staircase;
} Level;


void StartGame();
void PrintLevel(Level *level);

#endif