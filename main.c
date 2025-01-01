#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ncurses.h>
#include "menu.h"
#include "exit.h"
#include "main.h"

int logged_in_state = 0;
int n_choices = NUM_CHOICES_LOGGED_OUT;


int main() {
    show_main_menu();
    return 0;
}
