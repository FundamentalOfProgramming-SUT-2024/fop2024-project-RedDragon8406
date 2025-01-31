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
void InitFinalLevel(Level *level);
void StartGame();
void PrintLevel(Level* level); // here
void win_window();
void lost_window();
void food_window(Level *level,Player *player);
void weapon_window(Level *level,Player *player);
void projwin(Level *level,Player *player);
void potion_window(Level *level,Player *player);
void password_window(Level *level, Player *player);
int importpasswin(Level *level, Player *player, int howmany);
void pause_window(Level *level,Player *player);
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
    Level **levels=(Level **)malloc(5*sizeof(Level *));
    for(int i=0;i<4;i++){
        levels[i]=(Level *)malloc(sizeof(Level));
        levels[i]->len_rooms=6;
        levels[i]->rooms=(Room **)malloc(levels[i]->len_rooms*sizeof(Room *));
    }
    levels[4]=(Level *)malloc(sizeof(Level));
    levels[4]->len_rooms=1;
    levels[4]->rooms=(Room **)malloc(levels[4]->len_rooms*sizeof(Room *));
    
    gamewin = newwin(win_height,win_width,0,0);
    noecho();
    keypad(gamewin,true);
    clear();
    

    InitLevelRoom(levels[0]);
    int levels_initialization[5]={1,0,0,0,0};
    add_player_to_level(levels[0],player);
    refresh();
    int c;
    while(1){
        if(player->health<=0){
            wclear(gamewin);
            wrefresh(gamewin);
            delwin(gamewin);
            lost_window();
        }else if(player->health>MAXHEALTH){
            player->health=MAXHEALTH;
        }

        if(player->sat<=0){
            player->sat=0;
        }else if(player->sat>MAXSAT){
            player->sat=MAXSAT;
        }

        handleEnemyDeath(levels[current_level],player);

        if(player->should_pass){
            SecondTime=time(NULL);
            double timedifference;
            timedifference= difftime(SecondTime,FirstTime);
            if(timedifference>30){ // seconds available
                player->should_pass=0;
                handlegeneration(levels[current_level],player);
            }
        }
        if(!player->pcount[SPEED]){player->pcof[SPEED]=1;player->onspeed=0;}
        if(!player->pcount[HEALTH]){player->pcof[HEALTH]=1;}
        if(!player->pcount[DAMAGE]){player->pcof[DAMAGE]=1;}

        handleRegen(player);
        handleRot(player);


        handleVision(levels[current_level],player);
        PrintLevel(levels[current_level]);


        
        if(in_staircase(levels[current_level],player->loc) && player->changelevel){
            player->changelevel=0;
            if(current_level<3){
                wclear(gamewin);
                if(!levels_initialization[++current_level]){
                    levels_initialization[current_level]=1;
                    InitLevelRoom(levels[current_level]);
                }
                add_player_to_level(levels[current_level],player);
                levels[current_level]->rooms[0]->show=1;
                PrintLevel(levels[current_level]);
            }else if(current_level==3){
                wclear(gamewin);
                if(!levels_initialization[++current_level]){
                    levels_initialization[current_level]=1;
                    InitFinalLevel(levels[current_level]);
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
        else if(in_bstaircase(levels[current_level],player->loc) && player->changelevel){
            player->changelevel=0;
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
        player->changelevel=0;

        const char *title = "LEVEL: ";
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) / 2, "%s%d", title,current_level+1);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 - 5, "Golds: %d", player->golds);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 - 15, "Foods: %d", LenFood(player));
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 - 27, "Weapons: %d", player->weapons_count);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 + 5, "Potions: %d ", player->potions_count);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 + 16, "Sat: %d ", player->sat);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) * 3 / 4 + 26, "Health: %d ", player->health);
        mvwprintw(gamewin, win_height-2, (win_width - strlen(title)) / 2 - 10, "AKEYS:%d ", player->akey_count);
        c=wgetch(gamewin);
        switch (c){
        case KEY_BACKSPACE:
            // clear();
            // endwin();
            // refresh();
            // if(current_user!=NULL){
            //     current_user->golds+=player->golds;
            //     current_user->points+=player->golds*5;
            //     save_user_data(current_user); // temp
            // }
            // show_main_menu();
            pause_window(levels[current_level],player);
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
            }else if(current_level==3){
                wclear(gamewin);
                if(!levels_initialization[++current_level]){
                    levels_initialization[current_level]=1;
                    InitFinalLevel(levels[current_level]);
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
        case 'e':
            player->fastmove=1;
            break;
        case 'q':
            defuse_traps(levels[current_level],player);
            break;
        case 'z':
            player->passive = (player->passive + 1) % 2;
            break;
        case ' ':
            handleDamage(player,levels[current_level],gamewin,0);
            break;
        case 'r':
            handleDamage(player,levels[current_level],gamewin,player->lasthit);
            break;
        case 10:
            player->changelevel=1;
            break;
        default:
            break;
        }
        if(player->passive){
            mvwprintw(gamewin, 15, 1, "pm");
            wrefresh(gamewin);
        }else{
            mvwprintw(gamewin, 15, 1, "  ");
            wrefresh(gamewin);
        }

        if(player->fastmove){
            mvwprintw(gamewin, 16, 1, "fm mode");
            wrefresh(gamewin);

            c=wgetch(gamewin);
            if(c=='e'){
                player->fastmove=0;
            }
            else{
                while(handlePlayermove(levels[current_level],c,player,gamewin)){
                    handleVision(levels[current_level],player);
                    player->sat+=1;
                    if(player->pcount[SPEED]>0){
                        player->pcount[SPEED]++;
                    }
                    if(player->pcount[HEALTH]>0){
                        player->pcount[HEALTH]++;
                    }
                    if(player->pcount[DAMAGE]>0){
                        player->pcount[DAMAGE]++;
                    }
                    if(which_room(levels[current_level],player->loc)!=NULL){
                        if(which_room(levels[current_level],player->loc)->rt==ENCHANT){
                            player->sat+=4;
                        }
                    }
                }
                player->sat-=1;
                player->pcount[SPEED]--;
                if(player->pcount[SPEED]<0){
                    player->pcount[SPEED]=0;
                }
                player->pcount[HEALTH]--;
                if(player->pcount[HEALTH]<0){
                    player->pcount[HEALTH]=0;
                }
                player->pcount[DAMAGE]--;
                if(player->pcount[DAMAGE]<0){
                    player->pcount[DAMAGE]=0;
                }
                if(which_room(levels[current_level],player->loc)!=NULL){
                    if(which_room(levels[current_level],player->loc)->rt==ENCHANT){
                        player->sat-=4;
                    }
                }
                player->fastmove=0;
            }
        }
        else{
            handlePlayermove(levels[current_level],c,player,gamewin);
            if(player->pcof[SPEED]==2){
                player->onspeed = (player->onspeed + 1) % 2;
                if(!player->onspeed){
                    handleEnemymove(levels[current_level],player,gamewin);
                }
            }else{
                handleEnemymove(levels[current_level],player,gamewin);
            }
        }
        mvwprintw(gamewin, 16, 1, "       ");
        wrefresh(gamewin);

        usleep(10000);
    }
}


void InitLevelRoom(Level * level){
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
            int rt = rand() % 3;
            if(!rt){
                room->rt=NIGHTMARE;
            }else{
                room->rt=REGULAR;
            }

            add_doors_to_room(room,which);
            add_pillars_to_room(room);
            add_windows_to_room(room);
            add_golds_to_room(room);
            for(int i=0;i<room->golds_number;i++){
                if(!strcmp(settings->difficulty,"hard")){
                    if(room->golds[i]->gtype){
                        room->golds[i]->value=5;
                    }else{
                        room->golds[i]->value=1;
                    }
                }
                else if(!strcmp(settings->difficulty,"medium")){
                    if(room->golds[i]->gtype){
                        room->golds[i]->value=10;
                    }else{
                        room->golds[i]->value=2;
                    }
                }
                else if(!strcmp(settings->difficulty,"easy")){
                    if(room->golds[i]->gtype){
                        room->golds[i]->value=15;
                    }else{
                        room->golds[i]->value=3;
                    }
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
    int vich= rand() % level->len_rooms;
    for(int i=0;i<level->len_rooms;i++){
        if(vich==i){
            level->rooms[i]->shouldkey=1;
            level->wroomkey=i;
        }else{
            level->rooms[i]->shouldkey=0;
        }
    }
    add_akey(level);
    for(int i=0;i<level->len_rooms;i++){
        add_enemies_to_room(level->rooms[i],level);
    }
}

void InitFinalLevel(Level *level){
    int which = 0;
    level->rooms[which]=(Room *)malloc(sizeof(Room));
    level->rooms[which]->height=RHLG();
    level->rooms[which]->width=RWLG();
    level->rooms[which]->start.x=RXRG(0,level->rooms[which]->width)+basic_padding*5;
    level->rooms[which]->start.y=RYRG(0,level->rooms[which]->height)+(basic_padding*+1);
    // mvwprintw(gamewin,basic_padding,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
    // mvwprintw(gamewin,basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
    // mvwprintw(gamewin,2*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
    // mvwprintw(gamewin,3*basic_padding+MaxHeightSubWindow,basic_padding,"-----------------------------------------------------------------------------------------------------------------\n");
    // mvwprintw(gamewin,3*basic_padding+2*MaxHeightSubWindow,basic_padding+45 ,"------------------------------------------------------------------------------------------\n");
    Room * room=level->rooms[which];
    room->show=0;
    room->index=which;
    room->tries=0;
    room->rt=TREASURE;
    add_doors_to_room(room,which);
    add_pillars_to_room(room);
    add_windows_to_room(room);
    add_golds_to_room(room);
    for(int i=0;i<room->golds_number;i++){
        if(!strcmp(settings->difficulty,"hard")){
            if(room->golds[i]->gtype){
                room->golds[i]->value=5;
            }else{
                room->golds[i]->value=1;
            }
        }
        else if(!strcmp(settings->difficulty,"medium")){
            if(room->golds[i]->gtype){
                room->golds[i]->value=10;
            }else{
                room->golds[i]->value=2;
            }
        }
        else if(!strcmp(settings->difficulty,"easy")){
            if(room->golds[i]->gtype){
                room->golds[i]->value=15;
            }else{
                room->golds[i]->value=3;
            }
        }
    }

    add_foods_to_room(room);
    add_traps_to_room(room);
    add_potions_to_room(room);
    add_weapons_to_room(room);
    // add_corridors_to_level(level,gamewin);
    add_staircase_to_level(level);
    add_lockshow_to_level(level);
    add_gen(level);
    level->show=0;
    level->showtrap=0;
    level->rooms[which]->shouldgen=0;
    level->wroomkey=0;
    level->rooms[which]->shouldkey=1;
    add_akey(level);
    level->akey->taken=1;
    for(int i=0;i<level->len_rooms;i++){
        add_enemies_to_room(level->rooms[i],level);
    }
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
        PrintRoom(gamewin,room);
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
        mvwprintw(gamewin,5,1,"scount : %d:%d ",player->pcof[SPEED],player->pcount[SPEED]);
        mvwprintw(gamewin,6,1,"hcount : %d:%d ",player->pcof[HEALTH],player->pcount[HEALTH]);
        mvwprintw(gamewin,7,1,"dcount : %d:%d ",player->pcof[DAMAGE],player->pcount[DAMAGE]);
        mvwprintw(gamewin,8,1,"mam:%d aala:%d",player->diffc[0],player->diffc[1]);
        mvwprintw(gamewin,9,1,"jadoo:%d fased:%d",player->diffc[2],player->diffc[3]);

        mvwprintw(gamewin,38+(which),3,"{h:%d w:%d y:%d x:%d}",room->height,room->width,room->start.y,room->start.x);
        mvwprintw(gamewin,34,1,"wcount:%d ",player->wandcount);
        mvwprintw(gamewin,35,1,"acount:%d ",player->arrowcount);
        mvwprintw(gamewin,36,1,"dcount:%d ",player->dagcount);
        mvwprintw(gamewin,37,1,"wktaken:d:%d %d %d %d %d "
        ,player->wktaken[0],player->wktaken[1],player->wktaken[2],player->wktaken[3],player->wktaken[4]);
        for(int i=0;i<room->enemies_number;i++){
            mvwprintw(gamewin,30+(which),3,"[y:%d x:%d,t:%s]"
            ,room->enemies[i]->loc.y,room->enemies[i]->loc.x,room->enemies[i]->code);
        }
        for(int i=0;i<room->door_number;i++){
            mvwprintw(gamewin,38+(which),25+11*i,"(%d,%d|%d)",room->doors[i]->loc.y,room->doors[i]->loc.x,room->doors[i]->kind); //doors
        }
        mvwprintw(gamewin,38+(which),47,"[%d]",room->rt);
        mvwprintw(gamewin,38+(which),53,"en:(%d)",room->enemies_number);
        for(int i=0;i<5;i++){
            mvwprintw(gamewin,20+i,1,"             ");
        }
        for(int i=0;i<room->enemies_number;i++){
            mvwprintw(gamewin,20+i,1,"%s:%d:%d",room->enemies[i]->code
            ,room->enemies[i]->health,room->enemies[i]->alive);
        }
        wrefresh(gamewin);
    }


    if(which_room(level,player->loc)!=NULL){
        if(which_room(level,player->loc)->shouldgen){
            mvwprintw(gamewin,4,1,"pass:[%.4d]",which_room(level,player->loc)->gen->generated?
             which_room(level,player->loc)->gen->password : 0);
        }
    }else{
        mvwprintw(gamewin,5,1,"hehe           ");
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

    if(level->rooms[level->wroomkey]->show || level->show){
        if(!level->akey->taken){
            init_pair(94,94,COLOR_BLACK);
            wattron(gamewin,COLOR_PAIR(94));
            mvwprintw(gamewin, level->akey->loc.y, level->akey->loc.x,"\u25B3"); // ancient key
            wattroff(gamewin,COLOR_PAIR(94));
        }
    }

    if(which_room(level,player->loc)==NULL){
        mvwprintw(gamewin,5,1,"           ");
    }
    for(int i=0;i<level->len_rooms;i++){
        Room *room=level->rooms[i];
        if(!room->show && !level->show){
            continue;
        }
        for(int i=0;i<room->enemies_number;i++){
            if(room->enemies[i]->alive){
                mvwprintw(gamewin,room->enemies[i]->loc.y,room->enemies[i]->loc.x,"%s",room->enemies[i]->code); // enemies
            }
        }
    }
    Room*room=which_room(level,player->loc);
    if(room!=NULL && !level->show){
        if(room->rt==NIGHTMARE){
            for(int i=room->start.x;i<room->start.x+room->width;i++){
                for(int j=room->start.y;j<room->start.y+room->height;j++){
                    if(-2 <= (player->loc.x - i) && (player->loc.x - i)<=2){
                        if(-2 <= (player->loc.y - j) && (player->loc.y - j)<=2){
                            continue;
                        }
                    }
                    mvwprintw(gamewin,j,i," ");
                }
            }
        }
    }
    PrintPlayer(gamewin,player,settings);
}




void win_window(){
    // pre configuration
    int height = 9;
    int width = 55;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win_win = newwin(height, width, starty, startx);
    keypad(win_win, TRUE); // enable keypad
    box(win_win, 0, 0);
    curs_set(0);
    const char *win_text = "You Won! Press any key to return to main menu...";
    mvwprintw(win_win, 1, (width - strlen(win_text)) / 2, "%s", win_text);
    mvwprintw(win_win, 3, 3, "your score : %d", player->golds * 5 + player->health * 3);
    mvwprintw(win_win, 5, 3, "your golds : %d", player->golds);
    mvwprintw(win_win, 7, 3, "your health : %d", player->health);
    wrefresh(win_win);
    wgetch(win_win);
    wclear(win_win);
    delwin(win_win);
    if(current_user!=NULL){
        current_user->golds+=player->golds;
        current_user->points+=player->golds*5; // winning points
        current_user->points+=player->health*3; // winning points
        current_user->games_finished+=1; // games finished
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
    mvwprintw(lose_win, 3, 3, "your score : %d", player->golds);
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
    int height = 20;
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
    int highlight=0;
    while(1){
        mvwprintw(food_window,1,(width-strlen(food_intro)-2) / 2,"%s%d",food_intro,LenFood(player));
        mvwprintw(food_window,1,1,"%d",highlight);
        mvwprintw(food_window,3 , 3 ,"normal food %s: %d ","\U0001F353",player->diffc[0]);
        mvwprintw(food_window,5 , 3 ,"super food %s: %d ","\U0001F355",player->diffc[1]);
        mvwprintw(food_window,7 , 3 ,"magical food %s: %d ","\U0001F35F",player->diffc[2]);
        mvwprintw(food_window,9 , 3 ,"rotten food %s: %d ","\U0001F353",player->diffc[3]);
        mvwprintw(food_window,3,25,"CONSUME FOOD");
        mvwprintw(food_window,5,25,"CONSUME FOOD");
        mvwprintw(food_window,7,25,"CONSUME FOOD");
        mvwprintw(food_window,9,25,"CONSUME FOOD");
        init_pair(34,34,COLOR_BLACK);
        init_pair(196,196,COLOR_BLACK);
        for(int i=0;i<4;i++){
            if(highlight!=i){
                continue;
            }
            if(player->diffc[i]){
                wattron(food_window,COLOR_PAIR(34));
            }else{
                wattron(food_window,COLOR_PAIR(196));
            }
            mvwprintw(food_window,3 + 2*i,25,"CONSUME FOOD");
            if(player->diffc[i]){
                wattroff(food_window,COLOR_PAIR(34));
            }else{
                wattroff(food_window,COLOR_PAIR(196));
            }
        }
        
        mvwprintw(food_window, 12, (width - 10) / 2, "Health: %d", player->health); // need some serious design
        mvwprintw(food_window, 14, (width - 14) / 2, "Saturation: %d", player->sat); // need some serious design

        wrefresh(food_window);
        c=wgetch(food_window);
        int should_temp=0;
        switch(c){
            case 10: // enter key
                switch(highlight){
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        if(player->diffc[highlight]>0){
                            player->sat+=FOOD_HEALTH;
                            if(player->sat > MAXSAT){
                                player->sat=MAXSAT;
                            }
                            player->diffc[highlight]-=1;
                            should_temp=1;
                        }
                        break;
                }
                if(should_temp){
                    switch(highlight){
                        case 0:
                            break;
                        case 1:
                            player->pcof[DAMAGE]=2;
                            player->pcount[DAMAGE]+=5;
                            break;
                        case 2:
                            player->pcof[SPEED]=2;
                            player->pcount[SPEED]+=5;
                            break;
                        case 3:
                            player->health-=20; // temp
                            break;
                    }
                    
                }
                
                break;
            case KEY_DOWN:
                if(highlight==3){
                    highlight=0;
                }else{
                    highlight++;
                }
                break;
            case KEY_UP:
                if(highlight==0){
                    highlight=3;
                }else{
                    highlight--;
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



void intToRoman(int num, char *roman) {
    struct Numeral {
        int value;
        const char *symbol;
    } numerals[] = {
        {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
        {100,  "C"}, {90,  "XC"}, {50,  "L"}, {40,  "XL"},
        {10,   "X"}, {9,   "IX"}, {5,   "V"}, {4,   "IV"},
        {1,    "I"}
    };
    int i = 0;
    roman[0] = '\0'; 
    while (num > 0) {
        while (num >= numerals[i].value) {
            strcat(roman, numerals[i].symbol);
            num -= numerals[i].value;
        }
        i++;
    }
}


// int main() {
//     int number = 3; // Example input
//     char romanNumeral[20]; // Buffer to store the Roman numeral string
//     intToRoman(number, romanNumeral);
//     printf("%d in Roman numerals is %s\n", number, romanNumeral);
//     return 0;
// }

void meleewin(Level *level,Player *player){
    // pre configuration
    int height = 25;
    int width = 100;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *melee_window = newwin(height, width, starty, startx);
    keypad(melee_window, TRUE); // enable keypad
    curs_set(0);    
    Weapon *wh[2];
    wh[0]=player->weapons[0];
    wh[1]=NULL;
    int ww=0;
    for(int i=0;i<player->weapons_count;i++){
        if(player->weapons[i]->weapon==SWORD){
            wh[1]=player->weapons[i];
        }
    }
    init_pair(34,34,COLOR_BLACK);
    init_pair(130,130,COLOR_BLACK);
    init_pair(214,214,COLOR_BLACK);
    init_pair(51,51,COLOR_BLACK);
    init_pair(129,129,COLOR_BLACK);

    char *knife[] = {
        "      /| ________________",
        "O|===|* >________________>",
        "      \\|",
        NULL
    };
    char *sword[] = {
        "         />_________________________________",
        "[########[]_________________________________>",
        "         \\>",
        NULL
    };
    char *select[] = {
        "          _           _   ",
        " ___  ___| | ___  ___| |_ ",
        "/ __|/ _ \\ |/ _ \\/ __| __|",
        "\\__ \\  __/ |  __/ (__| |_ ",
        "|___/\\___|_|\\___|\\___|\\__|",
        NULL
    };


    while(1){
        box(melee_window, 0, 0);
        if(wh[0]!=NULL){
            if(wh[0]==player->current_weapon){
                wattron(melee_window,WA_BLINK);
            }
            for(int i=0;knife[i]!=NULL;i++){
                mvwprintw(melee_window,3+i,10,"%s",knife[i]);
            }

            mvwprintw(melee_window,7,19,"mace  %s",wh[0]->code);
            mvwprintw(melee_window,9,18,"damage: %d",5);
            if(wh[0]==player->current_weapon){
                wattroff(melee_window,WA_BLINK);
            }





            if(ww==0){
                wattron(melee_window,COLOR_PAIR(34));
            }
            for(int i=0;select[i]!=NULL;i++){
                mvwprintw(melee_window,12+i,10,"%s",select[i]);
            }
            if(ww==0){
                wattroff(melee_window,COLOR_PAIR(34));
            }
        }

        if(wh[1]!=NULL){
            if(wh[1]==player->current_weapon){
                wattron(melee_window,WA_BLINK);
            }
            switch(player->swordcount-1){
                case 0:
                    break;
                case 1:
                    wattron(melee_window,COLOR_PAIR(130));
                    break;
                case 2:
                    wattron(melee_window,COLOR_PAIR(214));
                    break;
                case 3:
                    wattron(melee_window,COLOR_PAIR(51));
                    break;
                default:
                    wattron(melee_window,COLOR_PAIR(129));
                    break;
            }
            for(int i=0;sword[i]!=NULL;i++){
                mvwprintw(melee_window,3+i,48,"%s",sword[i]);
            }
            switch(player->swordcount-1){
                case 0:
                    break;
                case 1:
                    wattroff(melee_window,COLOR_PAIR(130));
                    break;
                case 2:
                    wattroff(melee_window,COLOR_PAIR(214));
                    break;
                case 3:
                    wattroff(melee_window,COLOR_PAIR(51));
                    break;
                default:
                    wattroff(melee_window,COLOR_PAIR(129));
                    break;
            }
            


            mvwprintw(melee_window,7,69,"sword  %s",wh[1]->code);
            mvwprintw(melee_window,9,60,"damage: %d",10 + (player->swordcount-1) * 5);
            if(player->swordcount==1){
                mvwprintw(melee_window,9,75,"sharpness: %d",player->swordcount-1);
            }else{
                char rn[20];
                intToRoman(player->swordcount-1,rn);
                mvwprintw(melee_window,9,75,"sharpness: %s",rn);
            }
            if(wh[1]==player->current_weapon){
                wattroff(melee_window,WA_BLINK);
            }



            if(ww==1){
                wattron(melee_window,COLOR_PAIR(34));
            }
            for(int i=0;select[i]!=NULL;i++){
                mvwprintw(melee_window,12+i,61,"%s",select[i]);
            }
            if(ww==1){
                wattroff(melee_window,COLOR_PAIR(34));
            }
        }
        wrefresh(melee_window);



        int c=wgetch(melee_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(melee_window);
                wrefresh(melee_window);
                delwin(melee_window);
                return;
            case KEY_LEFT:
            case KEY_RIGHT:
                if(ww==1){
                    if(wh[0]!=NULL){
                        ww=0;
                        break;
                    }
                }
                if(ww==0){
                    if(wh[1]!=NULL){
                        ww=1;
                        break;
                    }
                }
                break;
            case 10:
                player->current_weapon=wh[ww];
                break;
            default:
                break;
        }
    }

}
void projwin(Level *level,Player *player){
    // pre configuration
    int height = 20;
    int width = 100;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *proj_win = newwin(height, width, starty, startx);
    keypad(proj_win, TRUE); // enable keypad
    curs_set(0);    
    init_pair(34,34,COLOR_BLACK);
    Weapon *dag=NULL;
    Weapon *wand=NULL;
    Weapon *arrow=NULL;
    Weapon *highlight;
    Weapon *which[3];
    for(int i=0;i<player->weapons_count;i++){
        if(player->weapons[i]->weapon==DAGGER){
            dag=player->weapons[i];
        }
        if(player->weapons[i]->weapon==WAND){
            wand=player->weapons[i];
        }
        if(player->weapons[i]->weapon==ARROW){
            arrow=player->weapons[i];
        }
    }
    which[0]=dag;
    which[1]=wand;
    which[2]=arrow;
    int ww;
    highlight=dag;
    ww=0;
    if(highlight==NULL){
        highlight=wand;
        ww=1;
    }
    if(highlight==NULL){
        highlight=arrow;
        ww=2;
    }
    char *select[] = {
        "          _           _   ",
        " ___  ___| | ___  ___| |_ ",
        "/ __|/ _ \\ |/ _ \\/ __| __|",
        "\\__ \\  __/ |  __/ (__| |_ ",
        "|___/\\___|_|\\___|\\___|\\__|",
        NULL
    };

    init_pair(242,242,COLOR_BLACK);
    init_pair(196,196,COLOR_BLACK);
    init_pair(226,226,COLOR_BLACK);
    init_pair(40,40,COLOR_BLACK);

    while(1){
        box(proj_win, 0, 0);
        highlight=which[ww];
        mvwprintw(proj_win,1,1,"%d",ww);
        if(dag!=NULL){
            if(dag==player->current_weapon){
                wattron(proj_win,WA_BLINK);
            }
            
            mvwprintw(proj_win,3,12,"dagger %s",dag->code);
            mvwprintw(proj_win,5,3,"damage: %d",12);
            mvwprintw(proj_win,5,18,"range: %d",5);


            if(player->dagcount==0){
                wattron(proj_win,COLOR_PAIR(242));
            }else if(player->dagcount<5){
                wattron(proj_win,COLOR_PAIR(196));
            }else if(player->dagcount<10){
                wattron(proj_win,COLOR_PAIR(226));
            }else{
                wattron(proj_win,COLOR_PAIR(40));
            }
            mvwprintw(proj_win,7,10,"quantity : %d",player->dagcount);
            if(player->dagcount==0){
                wattroff(proj_win,COLOR_PAIR(242));
            }else if(player->dagcount<5){
                wattroff(proj_win,COLOR_PAIR(196));
            }else if(player->dagcount<10){
                wattroff(proj_win,COLOR_PAIR(226));
            }else{
                wattroff(proj_win,COLOR_PAIR(40));
            }
            
            
            
            if(dag==player->current_weapon){
                wattroff(proj_win,WA_BLINK);
            }
            if(dag==highlight){
                wattron(proj_win,COLOR_PAIR(34));
            }
            for(int i=0;select[i]!=NULL;i++){
                mvwprintw(proj_win,10+i,3,"%s",select[i]);
            }
            if(dag==highlight){
                wattroff(proj_win,COLOR_PAIR(34));
            }
        }
        if(wand!=NULL){
            if(wand==player->current_weapon){
                wattron(proj_win,WA_BLINK);
            }
            mvwprintw(proj_win,3,(width-4)/2,"wand %s",wand->code);
            mvwprintw(proj_win,5,(width-22)/2,"damage: %d",15);
            mvwprintw(proj_win,5,(width+8)/2-2,"range: %d",10);
            if(player->wandcount==0){
                wattron(proj_win,COLOR_PAIR(242));
            }else if(player->wandcount<5){
                wattron(proj_win,COLOR_PAIR(196));
            }else if(player->wandcount<10){
                wattron(proj_win,COLOR_PAIR(226));
            }else{
                wattron(proj_win,COLOR_PAIR(40));
            }
            mvwprintw(proj_win,7,(width-4)/2-3,"quantity : %d",player->wandcount);
            if(player->wandcount==0){
                wattroff(proj_win,COLOR_PAIR(242));
            }else if(player->wandcount<5){
                wattroff(proj_win,COLOR_PAIR(196));
            }else if(player->wandcount<10){
                wattroff(proj_win,COLOR_PAIR(226));
            }else{
                wattroff(proj_win,COLOR_PAIR(40));
            }
            if(wand==player->current_weapon){
                wattroff(proj_win,WA_BLINK);
            }
            if(wand==highlight){
                wattron(proj_win,COLOR_PAIR(34));
            }
            for(int i=0;select[i]!=NULL;i++){
                mvwprintw(proj_win,10+i,38,"%s",select[i]);
            }
            if(wand==highlight){
                wattroff(proj_win,COLOR_PAIR(34));
            }
        }
        if(arrow!=NULL){
            if(arrow==player->current_weapon){
                wattron(proj_win,WA_BLINK);
            }
            mvwprintw(proj_win,3,width-20,"arrow %s",arrow->code);
            mvwprintw(proj_win,5,width-27,"damage: %d",5);
            mvwprintw(proj_win,5,width-15,"range: %d",5);
            if(player->arrowcount==0){
                wattron(proj_win,COLOR_PAIR(242));
            }else if(player->arrowcount<5){
                wattron(proj_win,COLOR_PAIR(196));
            }else if(player->arrowcount<10){
                wattron(proj_win,COLOR_PAIR(226));
            }else{
                wattron(proj_win,COLOR_PAIR(40));
            }
            mvwprintw(proj_win,7,width-22,"quantity : %d",player->arrowcount);
            if(player->arrowcount==0){
                wattroff(proj_win,COLOR_PAIR(242));
            }else if(player->arrowcount<5){
                wattroff(proj_win,COLOR_PAIR(196));
            }else if(player->arrowcount<10){
                wattroff(proj_win,COLOR_PAIR(226));
            }else{
                wattroff(proj_win,COLOR_PAIR(40));
            }

            if(arrow==player->current_weapon){
                wattroff(proj_win,WA_BLINK);
            }
            if(arrow==highlight){
                wattron(proj_win,COLOR_PAIR(34));
            }
            for(int i=0;select[i]!=NULL;i++){
                mvwprintw(proj_win,10+i,71,"%s",select[i]);
            }
            if(arrow==highlight){
                wattroff(proj_win,COLOR_PAIR(34));
            }
        }
        
        wrefresh(proj_win);
        int c=wgetch(proj_win);
        switch(c){
            case KEY_BACKSPACE:
                wclear(proj_win);
                wrefresh(proj_win);
                delwin(proj_win);
                return;
            case KEY_LEFT:
                if(ww==0){
                    if(which[2]!=NULL){
                        ww=2;
                        break;
                    }if(which[1]!=NULL){
                        ww=1;
                        break;
                    }
                    break;
                }else if(ww==1){
                    if(which[0]!=NULL){
                        ww=0;
                        break;
                    }
                    if(which[2]!=NULL){
                        ww=2;
                        break;
                    }
                    break;
                }else if(ww==2){
                    if(which[1]!=NULL){
                        ww=1;
                        break;
                    }if(which[0]!=NULL){
                        ww=0;
                        break;
                    }
                    break;
                }
                break;
            case KEY_RIGHT:
                if(ww==0){
                    if(which[1]!=NULL){
                        ww=1;
                        break;
                    }if(which[2]!=NULL){
                        ww=2;
                        break;
                    }
                    break;
                }else if(ww==1){
                    if(which[2]!=NULL){
                        ww=2;
                        break;
                    }
                    if(which[0]!=NULL){
                        ww=0;
                        break;
                    }
                    break;
                }else if(ww==2){
                    if(which[0]!=NULL){
                        ww=0;
                        break;
                    }if(which[1]!=NULL){
                        ww=1;
                        break;
                    }
                    break;
                }
                break;

            case 10:
                player->current_weapon=which[ww];
                break;
            default:
                break;
        }
    }


}






void weapon_window(Level *level,Player *player){
    // pre configuration
    int height = 27;
    int width = 70;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *weapon_window = newwin(height, width, starty, startx);
    keypad(weapon_window, TRUE); // enable keypad
    curs_set(0);
    const char *weapon_intro = "number of weapons collected: ";
    int c;
    int highlight=0;

    char *axe[] = {
        "  ,  /\\  .",
        " //`-||-'\\\\",
        "(| -=||=- |)",
        " \\\\,-||-.//",
        "  `  ||  '",
        "     ||",
        "     ||",
        "     ||",
        "     ||",
        "     ||",
        "     ||",
        "     ()",
        NULL
    };

    char *arrow[] = {
    "  4$$-.",
    "  4   \".",
    "  4    ^.",
    "  4      $",
    "  4      \"b.",
    "  4        $",
    "  4        $r",
    "-$b========444=====->",
    "  4        $r",
    "  4        $",
    "  4      \"b.",
    "  4      $",
    "  4    ,,",
    "  J.  ,\"",
    " '$$-'",
    NULL
};

    init_pair(34,34,COLOR_BLACK);
    init_pair(196,196,COLOR_BLACK);

    while(1){
        box(weapon_window, 0, 0);
        mvwprintw(weapon_window, 1, (width - strlen(weapon_intro)) / 2, "%s%d/%d",
         weapon_intro,player->weapons_count,MAX_WEAPON_COUNT);
        wattron(weapon_window,WA_BLINK);
        for (int i = 0; axe[i] != NULL; i++) {
            mvwprintw(weapon_window,6+i,10, "%s", axe[i]);
        }
        for (int i = 0; arrow[i] != NULL; i++) {
            mvwprintw(weapon_window, 4+i, 42, "%s", arrow[i]);
        }
        wattroff(weapon_window,WA_BLINK);
        int x=(player->wktaken[WAND] || player->wktaken[ARROW] || player->wktaken[DAGGER]);

        mvwprintw(weapon_window,22,10,"MELEE WEAPONS");
        mvwprintw(weapon_window,22,38,"PROJECTILE WEAPONS");
        if(highlight){
            if(x){
                wattron(weapon_window,COLOR_PAIR(34));
                mvwprintw(weapon_window,22,38,"PROJECTILE WEAPONS");
                wattroff(weapon_window,COLOR_PAIR(34));
            }else{
                wattron(weapon_window,COLOR_PAIR(196));
                mvwprintw(weapon_window,22,38,"PROJECTILE WEAPONS");
                wattroff(weapon_window,COLOR_PAIR(196));
            }
        }else{
            wattron(weapon_window,COLOR_PAIR(34));
            mvwprintw(weapon_window,22,10,"MELEE WEAPONS");
            wattroff(weapon_window,COLOR_PAIR(34));
        }
        wrefresh(weapon_window);
        c=wgetch(weapon_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(weapon_window);
                delwin(weapon_window);
                PrintLevel(level);
                return;

            case KEY_RIGHT:
            case KEY_LEFT:
                highlight=(highlight+1) % 2;
                break;
            case 10: // enter
                if(highlight){
                    if(x){
                        wclear(weapon_window);
                        wrefresh(weapon_window);
                        projwin(level,player);
                    }
                }else{
                    wclear(weapon_window);
                    wrefresh(weapon_window);
                    meleewin(level,player);
                }
                break;
            default:
                break;
        }
    }


}


void potion_window(Level *level,Player *player){
    // pre configuration
    int height = 25;
    int width = 55;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *potion_window = newwin(height, width, starty, startx);
    keypad(potion_window, TRUE); // enable keypad
    box(potion_window, 0, 0);
    curs_set(0);
    const char *potion_intro = "number of potions collected: ";
    int c;
    wrefresh(potion_window);
    mvwprintw(potion_window, 1, (width - strlen(potion_intro)) / 2,
     "%s%d/%d", potion_intro,player->potions_count,MAX_POTION_COUNT);
    

    char *skel[] = {
        "    .-.",
        "   (o.o)",
        "    |=|",
        "   __|__",
        " //.=|=.\\\\",
        "// .=|=. \\\\",
        "\\\\ .=|=. //",
        " \\\\(_=_)//",
        "  (:| |:)",
        "   || ||",
        "   () ()",
        "   || ||",
        "   || ||",
        "  ==' '==",
        NULL
    };


    int highlight=0;
    init_pair(34,34,COLOR_BLACK);
    init_pair(196,196,COLOR_BLACK);

    init_pair(168,168,COLOR_BLACK); // r
    init_pair(83,83,COLOR_BLACK); //g
    init_pair(146,146,COLOR_BLACK); // fake b lol
    while(1){
        mvwprintw(potion_window,4,5,"speed potion %s : %d","\U0001F37E",player->diffp[SPEED]);
        mvwprintw(potion_window,6,5,"damage potion %s : %d","\U0001F37C",player->diffp[DAMAGE]);
        mvwprintw(potion_window,8,5,"health potion %s : %d","\U0001F377",player->diffp[HEALTH]);
        mvwprintw(potion_window,4,35,"CONSUME POTION");
        mvwprintw(potion_window,6,35,"CONSUME POTION");
        mvwprintw(potion_window,8,35,"CONSUME POTION");


        wattron(potion_window,WA_BLINK);
        if(player->pcof[SPEED]!=1){
            wattron(potion_window,COLOR_PAIR(83));
        }
        if(player->pcof[DAMAGE]!=1){
            wattron(potion_window,COLOR_PAIR(146));
        }
        if(player->pcof[HEALTH]!=1){
            wattron(potion_window,COLOR_PAIR(168));
        }


        for(int i=0;skel[i]!=NULL;i++){
            mvwprintw(potion_window,10+i,22,"%s",skel[i]);
        }


        if(player->pcof[SPEED]!=1){
            wattroff(potion_window,COLOR_PAIR(83));
        }
        if(player->pcof[DAMAGE]!=1){
            wattroff(potion_window,COLOR_PAIR(146));
        }
        if(player->pcof[HEALTH]!=1){
            wattroff(potion_window,COLOR_PAIR(168));
        }
        wattroff(potion_window,WA_BLINK);






        if(player->diffp[highlight]>0){
            wattron(potion_window,COLOR_PAIR(34));
        }else{
            wattron(potion_window,COLOR_PAIR(196));
        }
        mvwprintw(potion_window,4+2*highlight,35,"CONSUME POTION");
        if(player->diffp[highlight]>0){
            wattroff(potion_window,COLOR_PAIR(34));
        }else{
            wattroff(potion_window,COLOR_PAIR(196));
        }

        wrefresh(potion_window);
        c=wgetch(potion_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(potion_window);
                delwin(potion_window);
                PrintLevel(level);
                return;
            case KEY_DOWN:
                if(highlight<2){
                    highlight++;
                }else{
                    highlight=0;
                }
                break;
            case KEY_UP:
                if(highlight>0){
                    highlight--;
                }else{
                    highlight=2;
                }
                break;
            case 10:
                if(player->diffp[highlight]){
                    player->diffp[highlight]-=1;
                    player->pcount[highlight]+=10;
                    player->pcof[highlight]=2;
                }
                break;
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
    int random= rand() % 2; // int random= rand() % 5; for 20% chance of being reversed
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





int importpasswin(Level *level, Player *player, int howmany){
    // pre configuration
    int height = 25;
    int width = 80;
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
    int highlight=1;
    char texts[3][50];
    strcpy(texts[0],"unlock with usable key");
    strcpy(texts[1],"use password");
    strcpy(texts[2],"combine two broken keys");
    

    int valid_to_unlock=0, valid_to_combine=0;


    init_pair(82,82,COLOR_BLACK);
    init_pair(124,124,COLOR_BLACK);
    init_pair(31,31,COLOR_BLACK);
    while(1){
        valid_to_combine=0;
        valid_to_unlock=0;
        sort_keys(player,1);
        for(int i=0;i<player->akey_count;i++){
            if(!player->akeys[i]->broken){
                valid_to_unlock=1;
                break;
            }
        }
        for(int i=0;i<player->akey_count;i++){
            if(player->akeys[i]->broken){
                valid_to_combine+=1;
            }
        }
        if(valid_to_combine>=2){
            valid_to_combine=1;
        }else{
            valid_to_combine=0;
        }


        mvwprintw(password_window, 8, (width - strlen(texts[0])) * 1 / 4 - 7,"%s", texts[0]);
        mvwprintw(password_window, 8, (width - strlen(texts[1])) * 2 / 4,"%s", texts[1]);
        mvwprintw(password_window, 8, (width - strlen(texts[2])) * 3 / 4 + 9,"%s", texts[2]);
        switch(highlight){
            case 0:
                if(valid_to_unlock){
                    wattron(password_window,COLOR_PAIR(82));
                    mvwprintw(password_window, 8, (width - strlen(texts[0])) * 1 / 4 - 7,"%s", texts[0]);
                    wattroff(password_window,COLOR_PAIR(82));
                }else{
                    wattron(password_window,COLOR_PAIR(124));
                    mvwprintw(password_window, 8, (width - strlen(texts[0])) * 1 / 4 - 7,"%s", texts[0]);
                    wattroff(password_window,COLOR_PAIR(124));
                }
                break;
            case 1:
                wattron(password_window,COLOR_PAIR(31));
                mvwprintw(password_window, 8, (width - strlen(texts[1])) * 2 / 4,"%s", texts[1]);
                wattroff(password_window,COLOR_PAIR(31));
                break;
            case 2:
                if(valid_to_combine){
                    wattron(password_window,COLOR_PAIR(82));
                    mvwprintw(password_window, 8, (width - strlen(texts[2])) * 3 / 4 + 9,"%s", texts[2]);
                    wattroff(password_window,COLOR_PAIR(82));
                }else{
                    wattron(password_window,COLOR_PAIR(124));
                    mvwprintw(password_window, 8, (width - strlen(texts[2])) * 3 / 4 + 9,"%s", texts[2]);
                    wattroff(password_window,COLOR_PAIR(124));
                }
                break;
        }
        mvwprintw(password_window,1,1,"%d",player->akey_count);
        mvwprintw(password_window,3,1,"%d",valid_to_unlock);
        mvwprintw(password_window,5,1,"%d",valid_to_combine);
        for(int i=0;i<MAX_AKEY_COUNT;i++){
            mvwprintw(password_window, 10+2*i, 3, "                          ");
        }
        for(int i=0;i<player->akey_count;i++){
            mvwprintw(password_window, 10+2*i, 3, "Key[%d]: %s",i,!player->akeys[i]->broken?"Usable":"Broken");
        }

        mvwprintw(password_window, 4, (width - 4) / 2, "    ");
        mvwprintw(password_window, 6, (width - 4) / 2, "    ");
        wrefresh(password_window);

        c=wgetch(password_window);
        switch(c){
            case KEY_BACKSPACE:
                wclear(password_window);
                delwin(password_window);
                return -1;
            case 10:
                switch(highlight){
                    case 0:
                        if(valid_to_unlock){
                            sort_keys(player,-1);
                            player->akey_count-=1;
                            wclear(password_window);
                            delwin(password_window);
                            return which_room(level,player->loc)->gen->password;
                        }
                        break;
                    case 1:
                        mvwprintw(password_window, 8, (width - strlen(texts[1])) * 2 / 4,"%s", texts[1]);
                        init_pair(193,193,COLOR_BLACK);
                        wattron(password_window,COLOR_PAIR(193));
                        mvwprintw(password_window, 5, (width - 4) / 2, "----");

                        echo();
                        int takenpass=0;
                        mvwscanw(password_window,4,(width-4)/2,"%d",&takenpass);
                        noecho();
                        wattroff(password_window,COLOR_PAIR(193));
                        wclear(password_window);
                        delwin(password_window);
                        return takenpass;
                        break;
                    case 2:
                        if(valid_to_combine){
                            int x = player->akey_count-1;
                            while(x>=0 && player->akeys[x]->broken){
                                x--;    
                            }
                            player->akeys[x+1]->broken=0;
                            player->akey_count -= 1;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case KEY_RIGHT:
                if(highlight<2){
                    highlight++;
                }else{
                    highlight=0;
                }
                break;
            case KEY_LEFT:
                if(highlight>0){
                    highlight--;
                }else{
                    highlight=2;
                }
                break;

            default:
                break;
        }

        
    }
    return -1;
}


void pause_window(Level *level,Player *player){
    // pre configuration
    int height = 25;
    int width = 30;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *pausewin = newwin(height, width, starty, startx);
    keypad(pausewin, TRUE);
    box(pausewin, 0, 0);
    char texts[3][100];
    strcpy(texts[0],"resume");
    strcpy(texts[1],"quit");
    strcpy(texts[2],"save and quit");
    const char *pause_intro = "here is the password: ";
    int highlight=0;
    while(1){
        mvwprintw(pausewin,1,(width-strlen(pause_intro))/2,"%s",pause_intro);
        for(int i=0;i<3;i++){
            if(highlight==i){
                wattron(pausewin,A_REVERSE);
            }
            mvwprintw(pausewin,3+2*i,(width-strlen(texts[i]))/2,"%s",texts[i]);
            if(highlight==i){
                wattroff(pausewin,A_REVERSE);
            }
        }

        int c=wgetch(pausewin);
        switch(c){
            case KEY_BACKSPACE:
                wclear(pausewin);
                wrefresh(pausewin);
                delwin(pausewin);
                return;
            case KEY_DOWN:
                highlight=(highlight+1) % 3;
                break;
            case KEY_UP:
                highlight=highlight?highlight-1:2;
                break;
            case 10:
                switch(highlight){
                    case 0:
                        wclear(pausewin);
                        wrefresh(pausewin);
                        delwin(pausewin);
                        return;
                    case 1:
                        wclear(pausewin);
                        wrefresh(pausewin);
                        delwin(pausewin);
                        show_main_menu();
                        break;
                    case 2:
                        wclear(pausewin);
                        wrefresh(pausewin);
                        delwin(pausewin);
                        if(current_user!=NULL){
                            current_user->golds+=player->golds;
                            current_user->points+=player->golds*5;
                            save_user_data(current_user); // temp
                        }
                        show_main_menu();
                            break;
                }
                break;
            default:
                break;
        }
    }
    curs_set(0);
}



void init_player(){
    player=(Player *)malloc(sizeof(Player));
    player->golds=0;
    player->fastmove=0;
    for(int i=0;i<4;i++){
        player->takenfoods[i]=(TakenFood **)malloc(MAX_TAKEN_FOOD_COUNT*sizeof(TakenFood *));
        for(int j=0;j<MAX_FOOD_COUNT;j++){
            player->takenfoods[i][j]=(TakenFood *)malloc(sizeof(TakenFood));
        }
    }
    for(int i=0;i<4;i++){
        player->diffc[i]=0;
    }


    if(!strcmp(settings->difficulty,"hard")){
        player->health=200;
    }
    else if(!strcmp(settings->difficulty,"medium")){
        player->health=300;
    }
    else if(!strcmp(settings->difficulty,"easy")){
        player->health=400;
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


    player->potions_count=0;
    player->potions=(Potion **)malloc(MAX_POTION_COUNT*sizeof(Potion *));
    for(int i=0;i<MAX_POTION_COUNT;i++){
        player->potions[i]=(Potion *)malloc(sizeof(Potion));
    }

    player->akeys=(aKey **)malloc(MAX_AKEY_COUNT*sizeof(aKey *));
    for(int i=0;i<MAX_AKEY_COUNT;i++){
        player->akeys[i]=(aKey *)malloc(sizeof(aKey));
    }
    player->should_pass=0;
    player->akey_count=0;
    player->passive=0;
    player->sat=MAXSAT;
    for(int i=0;i<3;i++){
        player->diffp[i]=0;
        player->pcof[i]=1;
        player->pcount[i]=0;
    }
    player->onspeed=0;
    player->damage=0;
    player->dagcount=0;
    player->arrowcount=0;
    player->wandcount=0;
    player->swordcount=0;
    player->wktaken[0]=1;
    player->wktaken[1]=0;
    player->wktaken[2]=0;
    player->wktaken[3]=0;
    player->wktaken[4]=0;
    player->lasthit=0;
    player->changelevel=0;
}