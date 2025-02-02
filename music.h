#ifndef MUSIC_H
#define MUSIC_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

void show_music_win();
int listFiles(char *path, char files[][150], int maxFiles);
void initSDL();
int loadMusicFiles(char files[][150]) ;
void closeSDL();
#endif