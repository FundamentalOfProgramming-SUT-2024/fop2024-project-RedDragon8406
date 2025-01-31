#ifndef SAVE_H
#define SAVE_H

#include "game.h"
#include "auth.h"
#include "utils.h"

void SaveGame(Level **levels,Player *player, int li[5]);
void LoadGame();
#endif 
