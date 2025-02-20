#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ncurses.h>
#include <time.h>
#include <locale.h>
#include "menu.h"
#include "exit.h"
#include "main.h"
#include "auth.h"
#include "settings.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <dirent.h>
#include "music.h"

int n_choices;
Mix_Music *musics[MAX_MUSIC];
int on_pause=0;
int len_music;
int current_song=-1;
int main() {
    setlocale(LC_ALL, "");
    srand(time(NULL));

    initSDL();
    char files[MAX_MUSIC][150];
    len_music = listFiles("music",files,20);
    loadMusicFiles(files);

    check_logged_in_user();
    load_settings();
    show_main_menu();
    return 0;
}
