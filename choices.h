#ifndef CHOICES_H
#define CHOICES_H

#define NUM_CHOICES_LOGGED_OUT 6
#define NUM_CHOICES_LOGGED_IN 7

extern char *choices_logged_out[];
extern char *choices_logged_in[];

extern char **choices;

void update_choices();

#endif 
