#ifndef GAME_H
#define GAME_H

#include <time.h>

#define win_height 45
#define win_width 150

#define basic_padding 5

#define MaxHeightRoom 10
#define MaxWidthRoom 35

#define MinHeightRoom 8 // it's actually 4| 0 , 1->4 , 5 as for the walls
#define MinWidthRoom 12 // same thing

#define MaxHeightSubWindow 12
#define MaxWidthSubwindow 40

#define MAXSAT 200
#define MAXHEALTH 400

#define MAX_WEAPON_IN_ROOM 150
#define MAX_TAKEN_FOOD_COUNT 150
extern int current_level;
extern time_t FirstTime,SecondTime;

typedef enum{
    MACE,
    DAGGER,
    WAND,
    ARROW,
    SWORD
} Wep;


typedef enum{
    SPEED,
    DAMAGE,
    HEALTH
} Pot;

typedef enum{
    HIDDEN,
    PASS,
    NORMAL,
    UNLOCKED
} DODOULEG;

typedef enum{
    GOLD,
    BLACK
} GoldType;

typedef enum{
    REGULAR,
    ENCHANT,
    NIGHTMARE,
    TREASURE,
} RT;

typedef enum{
    DEAMON,
    FBM,
    GIANT,
    SNAKE,
    UNDEED
} EN;
typedef enum{
    MAM,
    AALA,
    JADOO,
    FASED
} FOOD;

typedef struct{
    int x;
    int y;
} Point;

typedef struct{
    Point loc;
    int generated;
    int password;
} Gen;

typedef struct _door{
    Point loc;
    DODOULEG kind;
    int locked;
    int show;
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
    char c;
} Staircase;

typedef struct{
    Point loc;
    int value;
    int taken;
    GoldType gtype;
    char code[10];
} Gold;

typedef struct{
    Point loc;
    Wep weapon;
    char code[20];
    int taken;
    int ifdn; // if dagger, it's number
    int ifan; // if arrow, it's number
    int ifwn; // if wand, it's number
} Weapon;

typedef struct{
    Point loc;
    Pot potion;
    char code[20];
    int taken;
} Potion;

typedef struct{
    time_t ttaken;
} TakenFood;

typedef struct{
    Point loc;
    int taken; 
    FOOD kind;
    char code[10];
} Food;

typedef struct{
    Point loc;
    int taken;
    int show;
} Trap;

typedef struct{
    Point loc;
    int taken;
    int broken;
} aKey;



typedef struct{
    Point loc;
    EN en;
    int health;
    int alive;
    char code[10];
    int trigerred;
    int crippled;
} Enemy;

typedef struct{
    int index;
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
    Potion **potions;
    int potions_number;
    Trap **traps;
    int traps_number;
    int show;
    int shouldgen;
    Gen *gen;
    int tries;
    int shouldkey;
    RT rt;
    Enemy **enemies;
    int enemies_number;
} Room;


typedef struct{
    Room **rooms;
    int len_rooms;
    Corridor **corrs;
    int corrs_number;
    Staircase *staircase;
    Staircase *bstaircase;
    int show;
    int showtrap;
    aKey * akey;
    int wroomkey;
} Level;


#define MAX_FOOD_COUNT 5
#define MAX_WEAPON_COUNT 5
#define MAX_POTION_COUNT 15
#define MAX_AKEY_COUNT 4
typedef struct{
    int fastmove;
    Point loc;
    int golds;
    int health;
    Food **foods;
    int foods_count;
    Weapon ** weapons;
    int weapons_count;
    Weapon *current_weapon;
    Potion ** potions;
    int potions_count;
    int should_pass;
    aKey **akeys;
    int akey_count;
    int passive;
    int sat; // saturation
    int onspeed;
    int damage;
    int dagcount;
    int arrowcount;
    int wandcount;
    int swordcount;
    int wktaken[5]; //kind of weapon taken

    int diffp[3];
    int pcof[3];
    int pcount[3];

    TakenFood **takenfoods[4];
    int diffc[4];

    int lasthit;
    int changelevel;
} Player;
void StartGame();
void PrintLevel(Level *level);
int importpasswin(Level *level, Player *player, int howmany);

#endif