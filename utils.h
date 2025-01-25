#ifndef UTILS_H
#define UTILS_H

int RHRG(); // room height random generator
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
int check_wall_collide(Level* level,Room* room,Point next_move);
void add_player_to_level(Level* level,Player* player);
void handlePlayermove(Level *level,int ch,Player *player,WINDOW *gamewin);
Corridor * in_corridor(Level *level,Point loc);
int in_staircase(Level *level,Point loc);
int corr_index(Point loc, Corridor * corr);
void handleVision(Level* level,Player* player);
void add_golds_to_room(Room *room);
void add_foods_to_room(Room *room);
void add_weapons_to_room(Room *room);
void add_potions_to_room(Room *room);
void add_traps_to_room(Room *room);

#endif