#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ncurses.h>
#include "menu.h"
#include "exit.h"
#include "main.h"
#include "auth.h"
#include "settings.h"

int n_choices;
int main() {
    settings=(Settings *)malloc(sizeof(Settings));
    settings->user=NULL;
    strcpy(settings->difficulty,"easy");
    strcpy(settings->user_color,"white");
    check_logged_in_user();
    load_settings();
    show_main_menu();
    return 0;
}
