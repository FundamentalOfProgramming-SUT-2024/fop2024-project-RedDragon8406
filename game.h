#ifndef GAME_H
#define GAME_H

#define win_height 40
#define win_width 150


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