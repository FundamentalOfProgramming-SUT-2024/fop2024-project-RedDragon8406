#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>

int RHLG(); // random height level generator
int RWLG();
int RHRG(); // random height room generator
int RWRG();
int RYRG(int y_subwindow,int height_room); // room y random generator
int RXRG(int x_subwindow,int widthroom);
void add_doors_to_room(Room *room,int which);
void add_corridors_to_level(Level* level,WINDOW *gamewin);
void add_pillars_to_room(Room *room);
void add_windows_to_room(Room *room);
void add_staircase_to_level(Level *level);
Room * which_room(Level *level,Point loc);
Door * is_door(Level *level,Point loc);
Window * is_window(Level *level, Point loc);
int check_wall_collide(Level* level,Room* room,Point next_move);
void add_player_to_level(Level* level,Player* player);
int handlePlayermove(Level *level,int ch,Player *player,WINDOW *gamewin);
Corridor * in_corridor(Level *level,Point loc);
int in_staircase(Level *level,Point loc);
int in_bstaircase(Level *level,Point loc);
int corr_index(Point loc, Corridor * corr);
void handleVision(Level* level,Player* player);
void add_golds_to_room(Room *room);
void add_foods_to_room(Room *room);
void add_weapons_to_room(Room *room);
void add_potions_to_room(Room *room);
void add_traps_to_room(Room *room);
void add_enemies_to_room(Room *room, Level *level);
void add_lockshow_to_level(Level *level);
void add_gen(Level *level);
void add_akey(Level *level);
int generatepassword();
void unlockdoor(Level *level, Player *player, Door *first);
int handlegeneration(Level *level, Player *player);
int ReverseNumber(int num);
void sort_keys(Player *player, int reversed);
void defuse_traps(Level *level,Player *player);

void handleRegen(Player *player);
void handleRot(Player *player);

int handleEnemymove(Level *level,Player *player,WINDOW *gamewin);
int handleTrajectorymove(Level *level,Weapon * weapon,Point wloc,WINDOW *gamewin, int wway,int x,Player *player);
int handleDamage(Player *player,Level * level,WINDOW *gamewin, int lh);
void handleEnemyDeath(Level *level, Player *player);
void spawnNewWeapon(Room *room,Player *player,int i, int wway,Weapon *wep);

int LenFood(Player *player);
#endif