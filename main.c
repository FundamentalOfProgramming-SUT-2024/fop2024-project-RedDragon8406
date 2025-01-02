#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ncurses.h>
#include "menu.h"
#include "exit.h"
#include "main.h"
#include "auth.h"


int n_choices;
int main() {
    check_logged_in_user();
    show_main_menu();
    
    if (current_user) {
        save_user_data(current_user);
        save_current_user(current_user);
    }

    return 0;
}
