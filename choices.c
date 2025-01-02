#include "choices.h"
#include "main.h"
#include "auth.h"
char *choices_logged_out[] = {
    "Start Game",
    "Play as Guest",
    "Login/Signup",
    "Scoreboard",
    "Settings",
    "Exit"
};

char *choices_logged_in[] = {
    "New Game",
    "Load Game",
    "Profile",
    "Scoreboard",
    "Settings",
    "Logout",
    "Exit"
};

char **choices;

void update_choices() {
    if (current_user) {
        choices = choices_logged_in;
        n_choices = NUM_CHOICES_LOGGED_IN;
    } else {
        choices = choices_logged_out;
        n_choices = NUM_CHOICES_LOGGED_OUT;
    }
}
