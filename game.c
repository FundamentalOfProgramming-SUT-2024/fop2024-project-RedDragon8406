#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include "game.h"
#include "menu.h"
#include "utils.h"
#include "auth.h"
#include "settings.h"
#include "design.h"



#define FOOD_HEALTH 30

void InitLevelRoom(Level * level);
void StartGame();
void PrintLevel(Level* level);
void win_window();
void lost_window();
void food_window(Level *level,Player *player);
void weapon_window(Level *level,Player *player);
void potion_window(Level *level,Player *player);
void password_window(Level *level, Player *player);
int importpasswin(Level *level, Player *player, int howmany);
void init_player();


time_t FirstTime,SecondTime;
int current_level;
WINDOW * gamewin;
Player *player;
void StartGame(){
    init_player();
    
    FirstTime=time(NULL);
    SecondTime=time(NULL); // just some inits

    current_level=0;
    Level **levels=(Level **)malloc(4*sizeof(Level *));
    for(int i=0;i<4;i++){
        levels[i]=(Level *)malloc(sizeof(Level));
        levels[i]->len_rooms=6;
        levels[i]->rooms=(Room **)malloc(levels[i]->len_rooms*sizeof(Room *));
    }
    
    gamewin = newwin(win_height,win_width,0,0);
    noecho();
    keypad(gamewin,true);
    clear();
    

    InitLevelRoom(levels[0]);
    int levels_initialization[4]={1,0,0,0};
    add_player_to_level(levels[0],player);
    refresh();
    int c;
    while(1){
        if(player->health<=0){
            wclear(gamewin);
            wrefresh(gamewin);
            delwin(gamewin);
            lost_window();
        }
        if(player->should_pass){
            SecondTime=time(NULL);
            double timedifference;
            timedifference= difftime(SecondTime,FirstTime);
            if(timedifference>10){
                player->should_pass=0;
                handlegeneration(levels[current_level],player);
            }
        }

        handleVision(levels[current_level],player);
        PrintLevel(levels[current_level]);


        
        if(in_staircase(levels[current_level],player->loc)){
            if(current_level<3){
                wclear(gamewin);
                if(!levels_initialization[++current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }else{
                wclear(gamewin); //  // maybe not?
                wrefresh(gamewin);
                delwin(gamewin);
                win_window();
            }
        }
        else if(in_bstaircase(levels[current_level],player->loc)){
            if(current_level>0){
                wclear(gamewin);
                if(!levels_initialization[--current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }
        }

        const char *title = "LEVEL: ";
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) / 2, "%s%d", title,current_level+1);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4, "Golds: %d", player->golds);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 - 10, "Foods: %d", player->foods_count);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 - 22, "Weapons: %d", player->weapons_count);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 + 12, "Potions: %d ", player->potions_count);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 + 24, "Health: %d ", player->health);
        c=wgetch(gamewin);
        switch (c){
        case KEY_BACKSPACE:
            clear();
            endwin();
            refresh();
            if(current_user!=NULL){
                current_user->golds+=player->golds;
                current_user->points+=player->golds*5;
                save_user_data(current_user); // temp
            }
            show_main_menu();
            break;
        case 'p':
            if(current_level<3){
                wclear(gamewin);
                if(!levels_initialization[++current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }
            break;
        case 'o':
            if(current_level>0){
                wclear(gamewin);
                if(!levels_initialization[--current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }
            break;

        case 'm':
            if(levels[current_level]->show){
                levels[current_level]->show=0;
                wclear(gamewin);
            }
            else{
                levels[current_level]->show=1;
            }
            break;
        
        case 'f':
            wclear(gamewin);
            wrefresh(gamewin);
            food_window(levels[current_level],player);
            break;
        case 'i':
            wclear(gamewin);
            wrefresh(gamewin);
            weapon_window(levels[current_level],player);
            break;
        case 'u':
            wclear(gamewin);
            wrefresh(gamewin);;
            potion_window(levels[current_level],player);
            break;
        case 't':
            if(levels[current_level]->showtrap){
                levels[current_level]->showtrap=0;
            }else{
                levels[current_level]->showtrap=1;
            }
            break;
        case 'g':
            if(handlegeneration(levels[current_level],player)){
                password_window(levels[current_level],player);
            }
            break;
        case '\'':
            if(which_room(levels[current_level],player->loc)!=NULL){
                which_room(levels[current_level],player->loc)->tries=0;
            }
            break;
        default:
            break;
        }

        handlePlayermove(levels[current_level],c,player,gamewin);

        usleep(10000);
    }
}


void InitLevelRoom(Level * level){
    level->rooms=(Room **)malloc(6*sizeof(Room *));
    for(int iter=0;iter<2;iter++){
        for(int jter=0;jter<3;jter++){
            int which =(iter?5-jter:jter);
            level->rooms[which]=(Room *)malloc(sizeof(Room));
            level->rooms[which]->height=RHRG();
            level->rooms[which]->width=RWRG();
            level->rooms[which]->start.x=RXRG((MaxWidthSubwindow*jter)+2*jter,level->rooms[which]->width)+basic_padding*5;
            level->rooms[which]->start.y=RYRG((MaxHeightSubWindow*iter)+basic_padding*iter,level->rooms[which]->height)+(basic_padding*(iter+1));
            // mvwprintw(gamewin,basic_padding,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,2*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,3*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
            // mvwprintw(gamewin,3*basic_padding+2*MaxHeightSubWindow,basic_padding+45 ,"------------------------------------------------------------------------------------------\n");
            Room * room=level->rooms[which];
            room->show=0;
            room->index=which;
            room->tries=0;

            add_doors_to_room(room,which);
            add_pillars_to_room(room);
            add_windows_to_room(room);
            add_golds_to_room(room);
            for(int i=0;i<room->golds_number;i++){
                if(!strcmp(settings->difficulty,"hard")){
                    room->golds[i]->value=1;
                }
                else if(!strcmp(settings->difficulty,"medium")){
                    room->golds[i]->value=2;
                }
                else if(!strcmp(settings->difficulty,"easy")){
                    room->golds[i]->value=3;
                }
            }

            add_foods_to_room(room);
            add_traps_to_room(room);
            add_potions_to_room(room);
            add_weapons_to_room(room);
            // mvwprintw(gamewin,0,0,"%d,%d",level->rooms[0]->doors[0]->loc.y,level->rooms[0]->doors[0]->loc.x);
            // mvwprintw(gamewin,0,0,"%d,%d",room->doors[3]->loc.y,room->doors[3]->loc.x);

            // mvwprintw(gamewin,38+(which),25,"{%d}",room->door_number);
        }
    }
    add_corridors_to_level(level,gamewin);
    add_staircase_to_level(level);
    add_lockshow_to_level(level);
    add_gen(level);
    // for(int which=0;which<6;which++){
    //     Room * room=level->rooms[which];
    //     mvwprintw(gamewin,38+(which),3,"{h:%d w:%d y:%d x:%d}",room->height,room->width,room->start.y,room->start.x);    
    // }
    level->show=0;
    level->showtrap=0;
}




void PrintLevel(Level* level){
    box(gamewin,0,0);
    mvwprintw(gamewin,3,1,"show:%d",level->show);
    init_colors();
    for(int which=0;which<level->len_rooms;which++){
        Room * room=level->rooms[which];
        if(!room->show && !level->show){
            continue;
        }
        for(int i=0;i<room->height;i++){
            for(int j=0;j<room->width;j++){
                if(j==room->width-1 || j==0){
                    mvwprintw(gamewin,i+room->start.y,j+room->start.x,"|"); //walls
                }
                else if(i==room->height-1 || i==0){
                    mvwprintw(gamewin,i+room->start.y,j+room->start.x,"_"); //walls
                }
                else{
                    mvwprintw(gamewin,i+room->start.y,j+room->start.x,"."); //floors
                }
                if(i==0 && (j==0 || j==room->width-1)){
                    mvwprintw(gamewin,i+room->start.y,j+room->start.x," "); //room-edges
                }
                PrintDoor(gamewin,room);
                for(int k=0;k<room->pillars_number;k++){
                    mvwprintw(gamewin,room->pillars[k]->loc.y,room->pillars[k]->loc.x,"O"); // pillars
                }
                for(int i=0;i<room->windows_number;i++){
                    mvwprintw(gamewin,room->windows[i]->loc.y,room->windows[i]->loc.x,"="); // windows
                }
                for(int i=0;i<room->golds_number;i++){
                    if(!room->golds[i]->taken){
                        PrintGold(gamewin,room->golds[i],settings); // golds
                    }
                }
                for(int i=0;i<room->foods_number;i++){
                    if(!room->foods[i]->taken){
                        PrintFood(gamewin,room->foods[i],settings); // foods
                    }
                }
                for(int i=0;i<room->weapons_number;i++){
                    if(!room->weapons[i]->taken){
                        mvwprintw(gamewin,room->weapons[i]->loc.y,room->weapons[i]->loc.x,"%s", room->weapons[i]->code); // weapons
                    }
                }
                for(int i=0;i<room->potions_number;i++){
                    if(!room->potions[i]->taken){
                        mvwprintw(gamewin,room->potions[i]->loc.y,room->potions[i]->loc.x,"%s", room->potions[i]->code); // potions
                    }
                }
                if(room->shouldgen){
                    init_pair(51,51,COLOR_BLACK);
                    wattron(gamewin,COLOR_PAIR(51));
                    mvwprintw(gamewin,room->gen->loc.y,room->gen->loc.x,"&"); // password generator
                    wattroff(gamewin,COLOR_PAIR(51));
                }

                for(int i=0;i<room->traps_number;i++){
                    if(room->traps[i]->taken){
                        mvwprintw(gamewin,room->traps[i]->loc.y,room->traps[i]->loc.x,"^"); // traps
                    }
                    else if(level->showtrap){
                        mvwprintw(gamewin,room->traps[i]->loc.y,room->traps[i]->loc.x,"T"); // traps
                    }
                }


                mvwprintw(gamewin,38+(which),3,"{h:%d w:%d y:%d x:%d}",room->height,room->width,room->start.y,room->start.x);
                for(int i=0;i<room->door_number;i++){
                    mvwprintw(gamewin,38+(which),25+15*i,"(%d,%d|%d)",room->doors[i]->loc.y,room->doors[i]->loc.x,room->doors[i]->kind); //doors
                }
                mvwprintw(gamewin,38+(which),55,"[%d]",room->foods_number);

                if(room->gen){
                    mvwprintw(gamewin,5,1,"pass:[%.4d]",room->gen->generated? room->gen->password : 0);
                }else{
                    mvwprintw(gamewin,5,1,"           ");
                }

            }
        }
    }

    for(int i=0;i<level->corrs_number;i++){
        for(int j=0;j<level->corrs[i]->locs_count;j++){
            if(level->corrs[i]->show[j] || level->show){
                mvwprintw(gamewin, level->corrs[i]->locs[j].y, level->corrs[i]->locs[j].x,"#"); // corridors
            }
        }
    }
    if(level->rooms[level->len_rooms-1]->show || level->show){
        mvwprintw(gamewin, level->staircase->loc.y, level->staircase->loc.x, "%c",level->staircase->c); // staircase
    }
    if(level->rooms[0]->show || level->show){
        mvwprintw(gamewin, level->bstaircase->loc.y, level->bstaircase->loc.x, "%c",level->bstaircase->c); // staircase
    }
    if(which_room(level,player->loc)==NULL){
        mvwprintw(gamewin,5,1,"           ");
    }
    PrintPlayer(gamewin,player,settings);
}




void win_window(){
    // pre configuration
    int height = 7;
    int width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win_win = newwin(height, width, starty, startx);
    keypad(win_win, TRUE); // enable keypad
    box(win_win, 0, 0);
    curs_set(0);
    const char *win_text = "You Won! Press any key to return to main menu...";
    mvwprintw(win_win, 1, (width - strlen(win_text)) / 2, "%s", win_text);
    wrefresh(win_win);
    wgetch(win_win);
    wclear(win_win);
    delwin(win_win);
    if(current_user!=NULL){
        current_user->golds+=player->golds;
        current_user->points+=player->golds*5; // winning points
        current_user->points+=player->health*3; // winning points
        save_user_data(current_user); // temp
    }
    show_main_menu();
}
void lost_window(){
    // pre configuration
    int height = 7;
    int width = 60;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *lose_win = newwin(height, width, starty, startx);
    keypad(lose_win, TRUE); // enable keypad
    box(lose_win, 0, 0);
    curs_set(0);
    const char *lose_text = "You lost :( ! Press any key to return to main menu...";
    mvwprintw(lose_win, 1, (width - strlen(lose_text)) / 2, "%s", lose_text);
    wrefresh(lose_win);
    wgetch(lose_win);
    wclear(lose_win);
    delwin(lose_win);
    if(current_user!=NULL){
        current_user->points+=player->golds; // losing points
        save_user_data(current_user); // temp
    }
    show_main_menu();
}
void food_window(Level *level,Player *player){
    // pre configuration
    int height = 8;
    int width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *food_window = newwin(height, width, starty, startx);
    keypad(food_window, TRUE); // enable keypad
    box(food_window, 0, 0);
    curs_set(0);
    const char *food_intro = "number of remaining foods: ";
    int c;
    wrefresh(food_window);
    while(1){
        mvwprintw(food_window, 1, (width - strlen(food_intro)) / 2, "%s%d", food_intro,player->foods_count);
        wattron(food_window, A_REVERSE);
        mvwprintw(food_window, 3, (width - strlen("CONSUME FOOD")) / 2, "%s", "CONSUME FOOD");
        wattroff(food_window, A_REVERSE);
        if(!player->foods_count){
            mvwprintw(food_window, 3, (width - strlen("CONSUME FOOD")) / 2, "%s", "CONSUME FOOD");
        }

        mvwprintw(food_window, 5, (width - 10) / 2, "Health: %d", player->health); // need some serious design

        wrefresh(food_window);
        c=wgetch(food_window);
        switch(c){
            case 10: // enter key
                if(player->foods_count){
                    player->health+=FOOD_HEALTH;
                    player->foods_count--;
                }
                break;
            case KEY_BACKSPACE:
                wclear(food_window);
                delwin(food_window);
                PrintLevel(level);
                return;
            default:
                break;
        }
    }


}

void weapon_window(Level *level,Player *player){
    // pre configuration
    int height = 30;
    int width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *weapon_window = newwin(height, width, starty, startx);
    keypad(weapon_window, TRUE); // enable keypad
    box(weapon_window, 0, 0);
    curs_set(0);
    const char *weapon_intro = "number of weapons collected: ";
    int c;
    wrefresh(weapon_window);
    mvwprintw(weapon_window, 1, (width - strlen(weapon_intro)) / 2, "%s%d/%d", weapon_intro,player->weapons_count,MAX_WEAPON_COUNT);
    int highlight=player->cw_index;
    while(1){
        for(int i=0;i<player->weapons_count;i++){
            mvwprintw(weapon_window,4+2*i,5,"weapon %d: %s                        ",i+1,player->weapons[i]->code);
            if(i==player->cw_index){
                mvwprintw(weapon_window,4+2*i,5,"weapon %d: %s <--- selecetd weapon",i+1,player->weapons[i]->code);
            }
            if(i==highlight){
                wattron(weapon_window,A_REVERSE);
                mvwprintw(weapon_window,4+2*i,5,"weapon %d:",i+1);
                wattroff(weapon_window,A_REVERSE);
            }
        }
        wrefresh(weapon_window);
        c=wgetch(weapon_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(weapon_window);
                delwin(weapon_window);
                PrintLevel(level);
                return;

            case KEY_DOWN:
                highlight++;
                if(highlight<player->weapons_count){
                    continue;
                }
                else{
                    highlight=0;
                }
                break;
            case KEY_UP:
                highlight--;
                if(highlight>=0){
                    continue;
                }
                else{
                    highlight=player->weapons_count-1;
                }
                break;
            case 10: // enter
                player->cw_index=highlight;
                player->current_weapon=player->weapons[player->cw_index];
                break;
            default:
                break;
        }
    }


}


void potion_window(Level *level,Player *player){
    // pre configuration
    int height = 20;
    int width = 50;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *potion_window = newwin(height, width, starty, startx);
    keypad(potion_window, TRUE); // enable keypad
    box(potion_window, 0, 0);
    curs_set(0);
    const char *potion_intro = "number of potions collected: ";
    int c;
    wrefresh(potion_window);
    mvwprintw(potion_window, 1, (width - strlen(potion_intro)) / 2, "%s%d/%d", potion_intro,player->potions_count,MAX_POTION_COUNT);
    while(1){
        mvwprintw(potion_window,4,5,"speed potion %s : %d","\U0001F37E",player->spc);
        mvwprintw(potion_window,6,5,"health potion %s : %d","\U0001F377",player->hpc);
        mvwprintw(potion_window,8,5,"damage potion %s : %d","\U0001F37C",player->dpc);
        

        wrefresh(potion_window);
        c=wgetch(potion_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(potion_window);
                delwin(potion_window);
                PrintLevel(level);
                return;
            default:
                break;
        }
    }


}

void password_window(Level *level,Player *player){
    // pre configuration
    int height = 10;
    int width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *password_window = newwin(height, width, starty, startx);
    keypad(password_window, TRUE); // enable keypad
    box(password_window, 0, 0);
    curs_set(0);
    const char *password_intro = "here is the password: ";
    int c;
    wrefresh(password_window);
    mvwprintw(password_window, 1, (width - strlen(password_intro)) / 2, "%s", password_intro);


    int pass=which_room(level,player->loc)->gen->password;
    int random= rand() % 5;
    if(!random){
        pass=ReverseNumber(pass);
    }
    mvwprintw(password_window, 4, (width - 4) / 2, "%d", pass);
    while(1){
        wrefresh(password_window);
        c=wgetch(password_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(password_window);
                delwin(password_window);
                return;
            default:
                break;
        }
    }


}




void init_player(){
    player=(Player *)malloc(sizeof(Player));
    player->golds=0;
    player->foods=(Food **)malloc(MAX_FOOD_COUNT*sizeof(Food *));
    for(int i=0;i<MAX_FOOD_COUNT;i++){
        player->foods[i]=(Food *)malloc(sizeof(Food));
    }
    player->foods_count=0;
    if(!strcmp(settings->difficulty,"hard")){
        player->health=100;
    }
    else if(!strcmp(settings->difficulty,"medium")){
        player->health=140;
    }
    else if(!strcmp(settings->difficulty,"easy")){
        player->health=200;
    }
    else{
        player->health=0;
    }

    player->weapons_count=1;
    player->weapons=(Weapon **)malloc(MAX_WEAPON_COUNT*sizeof(Food *));
    for(int i=0;i<MAX_WEAPON_COUNT;i++){
        player->weapons[i]=(Weapon *)malloc(sizeof(Weapon));
    }
    player->weapons[0]->weapon=MACE;
    strcpy(player->weapons[0]->code,"\u2692");
    player->weapons[0]->taken=1;
    player->current_weapon=player->weapons[0];
    player->cw_index=0;


    player->potions_count=0;
    player->spc=0;
    player->hpc=0;
    player->dpc=0;
    player->potions=(Potion **)malloc(MAX_POTION_COUNT*sizeof(Potion *));
    for(int i=0;i<MAX_POTION_COUNT;i++){
        player->potions[i]=(Potion *)malloc(sizeof(Potion));
    }


    player->should_pass=0;
}

int importpasswin(Level *level, Player *player, int howmany){
    // pre configuration
    int height = 10;
    int width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *password_window = newwin(height, width, starty, startx);
    keypad(password_window, TRUE); // enable keypad
    box(password_window, 0, 0);
    const char *password_intro = "enter the password : ";
    init_pair(226,226,COLOR_BLACK);
    init_pair(202,202,COLOR_BLACK);
    switch(howmany){
        case 0:
            mvwprintw(password_window, 1, (width - strlen(password_intro)) / 2, "%s", password_intro);
            break;
        case 1:
            wattron(password_window,COLOR_PAIR(226));
            mvwprintw(password_window, 1, (width - strlen(password_intro)) / 2, "%s", password_intro);
            wattroff(password_window,COLOR_PAIR(226));
            break;
        case 2:
            wattron(password_window,COLOR_PAIR(202));
            mvwprintw(password_window, 1, (width - strlen(password_intro)) / 2, "%s", password_intro);
            wattroff(password_window,COLOR_PAIR(202));
            break;
        default:
            break;
    }
    int c;
    mvwprintw(password_window, 5, (width - 4) / 2, "----");
    wrefresh(password_window);
    while(1){
        mvwprintw(password_window,1,1,"%d",howmany);
        echo();
        int takenpass=0;
        mvwscanw(password_window,4,(width-4)/2,"%d",&takenpass);
        mvwprintw(password_window, 6, (width - 4) / 2, "%d",takenpass);
        wrefresh(password_window);
        noecho();
        mvwprintw(password_window, 8, (width - 11) / 2, "press enter");
        c=wgetch(password_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(password_window);
                delwin(password_window);
                return -1;
            case 10:
                wclear(password_window);
                delwin(password_window);
                return takenpass;
            default:
                break;
        }
        mvwprintw(password_window, 4, (width - 4) / 2, "    ");
        mvwprintw(password_window, 6, (width - 4) / 2, "    ");
        
    }
    return -1;
}