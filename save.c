#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "save.h"
#include "utils.h"
#include "game.h"
#include "auth.h"

void SaveGame(Level **levels,Player *player, int li[5]){
    char path[100];
    snprintf(path,sizeof(path),"saves/%s",current_user->username);
    FILE *f=fopen(path,"wb");

    for(int i=0;i<5;i++){
        fwrite(levels[i],sizeof(Level),1,f);
        if(!li[i]){
            continue;
        }
        for(int j=0;j<levels[i]->len_rooms;j++){
            Room *room=levels[i]->rooms[j];
            fwrite(room,sizeof(Room),1,f);
            for(int k=0;k<room->door_number;k++){
                fwrite(room->doors[k],sizeof(Door),1,f);
            }
            for(int k=0;k<room->pillars_number;k++){
                fwrite(room->pillars[k],sizeof(Pillar),1,f);
            }
            for(int k=0;k<room->windows_number;k++){
                fwrite(room->windows[k],sizeof(Window),1,f);
            }
            for(int k=0;k<room->golds_number;k++){
                fwrite(room->golds[k],sizeof(Gold),1,f);
            }
            for(int k=0;k<room->foods_number;k++){
                fwrite(room->foods[k],sizeof(Food),1,f);
            }
            for(int k=0;k<room->weapons_number;k++){
                fwrite(room->weapons[k],sizeof(Weapon),1,f);
            }
            for(int k=0;k<room->potions_number;k++){
                fwrite(room->potions[k],sizeof(Potion),1,f);
            }
            for(int k=0;k<room->traps_number;k++){
                fwrite(room->traps[k],sizeof(Trap),1,f);
            }
            for(int k=0;k<room->enemies_number;k++){
                fwrite(room->enemies[k],sizeof(Enemy),1,f);
            }
            if(room->shouldgen){
                fwrite(room->gen,sizeof(Gen),1,f);
            }
        }

        for(int j=0;j<levels[i]->corrs_number;j++){
            Corridor *corr=levels[i]->corrs[j];
            fwrite(corr,sizeof(Corridor),1,f);
            fwrite(corr->node1,sizeof(Door),1,f);
            fwrite(corr->node2,sizeof(Door),1,f);
            fwrite(corr->locs,sizeof(Point),corr->locs_count,f);
            fwrite(corr->show,sizeof(int),1,f);
        }

        fwrite(levels[i]->staircase,sizeof(Staircase),1,f);
        fwrite(levels[i]->bstaircase,sizeof(Staircase),1,f);
        fwrite(levels[i]->akey,sizeof(aKey),1,f);
    }

    
    fwrite(player,sizeof(Player),1,f);

    for(int i=0;i<player->weapons_count;i++){
        fwrite(player->weapons[i],sizeof(Weapon),1,f);
    }
    fwrite(player->current_weapon,sizeof(Weapon),1,f);
    for(int i=0;i<player->potions_count;i++){
        fwrite(player->potions[i],sizeof(Potion),1,f);
    }
    for(int i=0;i<player->akey_count;i++){
        fwrite(player->akeys[i],sizeof(aKey),1,f);
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<player->diffc[j];j++){
            fwrite(player->takenfoods[i][j],sizeof(TakenFood),1,f);
        }
    }

    fclose(f);
}
void LoadGame(){
    
}