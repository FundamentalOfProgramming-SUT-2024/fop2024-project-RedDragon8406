#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "save.h"
#include "utils.h"
#include "game.h"
#include "auth.h"

void SaveGame(Level **levels,Player *player, int li[5]){
    char path[100];
    snprintf(path,sizeof(path),"saves/%s",current_user->username);
    FILE *f=fopen(path,"wb");


    fwrite(&current_level,sizeof(int),1,f);
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
            fwrite(corr->show,sizeof(int),corr->locs_count,f);
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
    char ipath[100];
    snprintf(ipath,sizeof(ipath),"saves/%s.db",current_user->username);
    FILE *fi=fopen(ipath,"w");
    fprintf(fi,"%d %d %d %d %d",li[0],li[1],li[2],li[3],li[4]);
    fclose(fi);
}







void LoadGame(Level **levels, Player *player, int li[5]) { // need to malloc things here and then pass the fread!
    char path[100];
    snprintf(path, sizeof(path), "saves/%s", current_user->username);
    FILE *f = fopen(path, "rb");
    if (!f) {
        printf("Error opening save file.\n");
        return;
    }

    fread(&current_level, sizeof(int), 1, f);
    for (int i = 0; i < 5; i++) {
        fread(levels[i], sizeof(Level), 1, f);
        if (!li[i]) {
            continue;
        }
        for (int j = 0; j < levels[i]->len_rooms; j++) {
            Room *room = malloc(sizeof(Room));
            fread(room, sizeof(Room), 1, f);
            free(levels[i]->rooms[j]);
            levels[i]->rooms[j] = room;
            for (int k = 0; k < room->door_number; k++) {
                Door *door = malloc(sizeof(Door));
                fread(door, sizeof(Door), 1, f);
                room->doors[k] = door;
            }
            for (int k = 0; k < room->pillars_number; k++) {
                Pillar *pillar = malloc(sizeof(Pillar));
                fread(pillar, sizeof(Pillar), 1, f);
                room->pillars[k] = pillar;
            }
            for (int k = 0; k < room->windows_number; k++) {
                Window *window = malloc(sizeof(Window));
                fread(window, sizeof(Window), 1, f);
                room->windows[k] = window;
            }
            for (int k = 0; k < room->golds_number; k++) {
                Gold *gold = malloc(sizeof(Gold));
                fread(gold, sizeof(Gold), 1, f);
                room->golds[k] = gold;
            }
            for (int k = 0; k < room->foods_number; k++) {
                Food *food = malloc(sizeof(Food));
                fread(food, sizeof(Food), 1, f);
                room->foods[k] = food;
            }
            for (int k = 0; k < room->weapons_number; k++) {
                Weapon *weapon = malloc(sizeof(Weapon));
                fread(weapon, sizeof(Weapon), 1, f);
                room->weapons[k] = weapon;
            }
            for (int k = 0; k < room->potions_number; k++) {
                Potion *potion = malloc(sizeof(Potion));
                fread(potion, sizeof(Potion), 1, f);
                room->potions[k] = potion;
            }
            for (int k = 0; k < room->traps_number; k++) {
                Trap *trap = malloc(sizeof(Trap));
                fread(trap, sizeof(Trap), 1, f);
                room->traps[k] = trap;
            }
            for (int k = 0; k < room->enemies_number; k++) {
                Enemy *enemy = malloc(sizeof(Enemy));
                fread(enemy, sizeof(Enemy), 1, f);
                room->enemies[k] = enemy;
            }
            if (room->shouldgen) {
                Gen *gen = malloc(sizeof(Gen));
                fread(gen, sizeof(Gen), 1, f);
                room->gen = gen;
            }
        }
        for (int j = 0; j < levels[i]->corrs_number; j++) {
            Corridor *corr = malloc(sizeof(Corridor));
            fread(corr, sizeof(Corridor), 1, f);
            levels[i]->corrs[j] = corr;

            Door *node1 = malloc(sizeof(Door));
            fread(node1, sizeof(Door), 1, f);
            corr->node1 = node1;

            Door *node2 = malloc(sizeof(Door));
            fread(node2, sizeof(Door), 1, f);
            corr->node2 = node2;

            corr->locs = malloc(corr->locs_count * sizeof(Point));
            fread(corr->locs, sizeof(Point), corr->locs_count, f);

            corr->show = malloc(corr->locs_count * sizeof(int));
            fread(corr->show, sizeof(int), corr->locs_count, f);
        }
        Staircase *staircase = malloc(sizeof(Staircase));
        fread(staircase, sizeof(Staircase), 1, f);
        levels[i]->staircase = staircase;

        Staircase *bstaircase = malloc(sizeof(Staircase));
        fread(bstaircase, sizeof(Staircase), 1, f);
        levels[i]->bstaircase = bstaircase;

        aKey *akey = malloc(sizeof(aKey));
        fread(akey, sizeof(aKey), 1, f);
        levels[i]->akey = akey;
    }

    fread(player, sizeof(Player), 1, f);
    for (int i = 0; i < player->weapons_count; i++) {
        fread(player->weapons[i], sizeof(Weapon), 1, f);
    }
    fread(player->current_weapon , sizeof(Weapon), 1, f);

    for (int i = 0; i < player->potions_count; i++) {
        fread(player->potions[i], sizeof(Potion), 1, f);
    }
    for (int i = 0; i < player->akey_count; i++) {
        fread(player->akeys[i], sizeof(aKey), 1, f);
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < player->diffc[i]; j++) {
            fread(player->takenfoods[i][j], sizeof(TakenFood), 1, f);
            player->takenfoods[i][j]=time(NULL);
        }
    }
    fclose(f);
}
