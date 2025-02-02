#ifndef DESIGN_H
#define DESIGN_H

#define COLOR_CUSTOM_EXIT 8
#define COLOR_CUSTOM_SILVER 9 
#define PLAYER_WHITE 10
#define PLAYER_BLUE 11
#define PLAYER_GREEN 12
#define COLOR_CUSTOM_GOLD 13 
#define TREASURE_ROOM_COLOR 14

#include "game.h"
#include "settings.h"
#include "auth.h"



void init_colors();
void apply_logout_design(WINDOW *logout_win);
void PrintPlayer(WINDOW *gamewin, Player *player, Settings* settings);
void PrintGold(WINDOW *gamewin, Gold* gold , Settings* settings);
void PrintFood(WINDOW *gamewin, Food* food , Settings* settings);
void PrintDoor(WINDOW *gamewin, Room * room);
void PrintRoom(WINDOW *gamewin, Room *room);
void PrintHS(WINDOW *win,Player *player, int x, int y, int padding);
void welcome_message(WINDOW *chatwin);
void chatclear(WINDOW *chatwin);
void chat_new_room(WINDOW *chatwin);
void chat_gold(WINDOW *chatwin, Gold *gold);
void chat_enemy_hit(WINDOW *chatwin, Enemy *enemy);
void chat_hit_enemy(WINDOW *chatwin, Player *player,Enemy *enemy);
void potion_situation(WINDOW *gamewin, Player *player);
void show_current_weapon(WINDOW *gamewin, Player *player);

#endif
