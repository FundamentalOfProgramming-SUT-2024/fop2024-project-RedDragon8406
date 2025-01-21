#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ncurses.h>
#include <time.h>
#include "menu.h"
#include "exit.h"
#include "main.h"
#include "auth.h"
#include "settings.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <dirent.h>
int n_choices;
int main() {
    srand(time(NULL));
    check_logged_in_user();
    load_settings();
    show_main_menu();
    return 0;
}
