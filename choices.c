#include "choices.h"
#include "main.h"

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

char **choices; // pointer to which choices should i refer to

void update_choices() {
    if (logged_in_state) {
        choices = choices_logged_in;
        n_choices = NUM_CHOICES_LOGGED_IN;
    } else {
        choices = choices_logged_out;
        n_choices = NUM_CHOICES_LOGGED_OUT;
    }
}
