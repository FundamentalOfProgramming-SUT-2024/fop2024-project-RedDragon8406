#ifndef DESIGN_H
#define DESIGN_H


#include "game.h"
#include "settings.h"
#include "auth.h"

void init_colors();
void apply_logout_design(WINDOW *logout_win);
void PrintPlayer(WINDOW *gamewin, Player *player, Settings* settings);
void PrintGold(WINDOW *gamewin, Gold* gold , Settings* settings);
void PrintFood(WINDOW *gamewin, Food* food , Settings* settings);
void PrintDoor(WINDOW *gamewin, Room * room);
#endif
