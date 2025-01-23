#ifndef GAME_H
#define GAME_H

#define win_height 45
#define win_width 150

#define basic_padding 5

#define MaxHeightRoom 10
#define MaxWidthRoom 35

#define MinHeightRoom 8 // it's actually 4| 0 , 1->4 , 5 as for the walls
#define MinWidthRoom 8 // same thing

#define MaxHeightSubWindow 12
#define MaxWidthSubwindow 40

extern int current_level;


typedef enum{
    MACE,
    DAGGER,
    WAND,
    ARROW,
    SWORD
} Wep;

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
    int *show;
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
    Point loc;
    int value;
    int taken;
} Gold;

typedef struct{
    Point loc;
    Point nloc;
    Wep weapon;
    char code[20];
    int taken;
} Weapon;

typedef struct{
    Point loc;
    int taken; 
    int kind;
} Food;
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
    Gold **golds;
    int golds_number;
    Food **foods;
    int foods_number;
    Weapon **weapons;
    int weapons_number;
    int show;

} Room;

typedef struct{
    Room **rooms;
    int len_rooms;
    Corridor **corrs;
    int corrs_number;
    Staircase *staircase;
    int show;
} Level;


#define MAX_FOOD_COUNT 5
#define MAX_WEAPON_COUNT 10
typedef struct{
    Point loc;
    int golds;
    int health;
    Food **foods;
    int foods_count;
    Weapon ** weapons;
    int weapons_count;
    Weapon *current_weapon;
    int cw_index;
} Player;
void StartGame();
void PrintLevel(Level *level);

#endif