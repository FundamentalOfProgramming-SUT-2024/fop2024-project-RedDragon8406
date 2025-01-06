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


#endif