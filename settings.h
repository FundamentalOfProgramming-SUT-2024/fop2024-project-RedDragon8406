#ifndef SETTINGS_H
#define SETTINGS_H

#include "auth.h"


typedef struct {
    char difficulty[30];
    char user_color[30];
    UserData *user;
} Settings;

extern Settings* settings;

void save_settings();
void load_settings();
void create_settings(const char * username);
void settings_window();
#endif 
