#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define NUM_CHOICES_LOGGED_OUT 5
#define NUM_CHOICES_LOGGED_IN 7

#define MAX_MUSIC 20

extern int n_choices;
extern Mix_Music *musics[MAX_MUSIC];
extern int on_pause;
extern int len_music;
extern int current_song;
#endif
