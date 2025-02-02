#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ncurses.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <dirent.h>
#include "menu.h"
#include "exit.h"
#include "main.h"
#include "auth.h"
#include "settings.h"
#include "game.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>

int listFiles(char *path, char files[][100], int maxFiles) {
    struct dirent *entry;
    DIR *dp = opendir(path);
    int count = 0;

    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)) && count < maxFiles) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }

        if (strstr(entry->d_name, ":Zone.Identifier") != NULL) {
            continue;
        }

        strncpy(files[count], entry->d_name, 100);
        files[count][99] = '\0'; 
        count++;
    }

    closedir(dp);
    return count; 
}



void show_music_win() {
    WINDOW *musicwin;
    int height = 35;
    int width = 75;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    musicwin = newwin(height, width, starty, startx);
    keypad(musicwin, TRUE);
    box(musicwin, 0, 0);

    char *title = "MUSIC MENU";
    mvwprintw(musicwin,1,(width-strlen(title))/2,"%s",title);
    char files[MAX_MUSIC][100]; 
    len_music = listFiles("music",files,20);
    
    int ch=0;
    char path[150];
    snprintf(path,sizeof(path),"music/%s",files[0]);
    int highlight=0;
    while (1) {
        if(highlight==0){
            wattron(musicwin,A_REVERSE);
            mvwprintw(musicwin,3,32,"PAUSE/PLAY");
            wattroff(musicwin,A_REVERSE);
        }else{
            mvwprintw(musicwin,3,32,"PAUSE/PLAY");
        }

        for(int i=0;i<len_music;i++){
            if(i==highlight-1){
                wattron(musicwin,A_REVERSE);
            }
            mvwprintw(musicwin,5+2*i,1,"music[%i]: %s",i,files[i]);
            if(i==highlight-1){
                wattroff(musicwin,A_REVERSE);
            }
        }

        ch=wgetch(musicwin);
        switch (ch) {
            case KEY_BACKSPACE:
                wclear(musicwin);
                wrefresh(musicwin);
                delwin(musicwin);
                return;
            case 10:
                if(highlight==0){
                    if(on_pause){
                        Mix_ResumeMusic();
                    }else{
                        Mix_PauseMusic();
                    }
                    on_pause = (on_pause + 1) % 2;
                }else{
                    Mix_PlayMusic(musics[highlight-1],1);
                    current_song=highlight-1;
                }
                break;
            case KEY_DOWN:
                highlight=(highlight+1) % (len_music + 1);
                break;
            case KEY_UP:
                highlight=highlight?highlight-1:len_music;
                break;
        }

        wrefresh(musicwin);
    }
}


void initSDL() {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}
int loadMusicFiles(char files[][100]) {
    int count = 0;
    for (int i = 0; i < len_music; i++) {
        char path[150];
        snprintf(path,sizeof(path),"music/%s",files[i]);
        musics[count] = Mix_LoadMUS(path);
        if (musics[count] != NULL) {
            count++;
        }
    }
    return count;
}


void closeSDL() {
    for (int i = 0; i < len_music; i++) {
        if (musics[i] != NULL) {
            Mix_FreeMusic(musics[i]);
        }
    }

    Mix_CloseAudio();

    SDL_Quit();
}

