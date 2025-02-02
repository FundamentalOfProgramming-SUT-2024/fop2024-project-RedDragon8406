#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include "game.h"
#include "settings.h"
#include "auth.h"
#include "save.h"
#include "design.h"
/*
        |   |          |   |
        |   |          |   |        
--------------------------------
---------------------------------
        |   |          |   |
        |   |          |   |

20 for width of each
10 for height of each
10 for both x,y of blank space between each subwindow

*/
int RHLG(){
    int y;
    y = (rand() % (MaxHeightRoom - MinHeightRoom)) + MinHeightRoom + 5;
    return y;
}
int RWLG(){
    int x;
    x = (rand() % (MaxWidthRoom - MinWidthRoom)) + MinWidthRoom ;
    x = x * 3 / 2;
    return x;
}
int RHRG(){
    int y;
    y = (rand() % (MaxHeightRoom - MinHeightRoom)) + MinHeightRoom;
    return y;
}
int RWRG(){
    int x;
    x = (rand() % (MaxWidthRoom - MinWidthRoom)) + MinWidthRoom;
    return x;
}

int RYRG(int y_subwindow,int height_room){
    int y;
    y = (rand() % (MaxHeightSubWindow-height_room-1)) + y_subwindow;
    return y;
}
int RXRG(int x_subwindow,int widthroom){
    int x;
    x = (rand() % (MaxWidthSubwindow-widthroom-1)) + x_subwindow;
    return x;
}


void add_player_to_level(Level* level,Player* player){
    player->loc.x=level->bstaircase->loc.x;
    player->loc.y=level->bstaircase->loc.y;
}

Room * which_room(Level *level,Point loc){
    for(int i=0;i<level->len_rooms;i++){
        Room *room=level->rooms[i];
        if(room->start.x < loc.x && loc.x < room->start.x+room->width-1 && 
        room->start.y < loc.y && loc.y < room->start.y+room->height-1){
            return level->rooms[i];
        }
    }
    return NULL;
}
Door * is_door(Level*level, Point loc){
    Room *room;
    for(int j=0;j<level->len_rooms;j++){
        room=level->rooms[j];
        for(int i=0;i<room->door_number;i++){
            if(room->doors[i]->loc.x == loc.x && room->doors[i]->loc.y == loc.y){
                return room->doors[i];
            }
        }
    }
    return NULL;
}

Window * is_window(Level *level, Point loc){
    Room *room;
    for(int j=0;j<level->len_rooms;j++){
        room=level->rooms[j];
        for(int i=0;i<room->windows_number;i++){
            if(room->windows[i]->loc.x == loc.x && room->windows[i]->loc.y == loc.y){
                return room->windows[i];
            }
        }
    }
    return NULL;
}

int check_wall_collide(Level* level,Room* room,Point next_move){
    if (which_room(level,next_move)==NULL){
        if(is_door(level,next_move)!=NULL){
            return 1;
        }
        return 0;
    }
    for(int i=0;i<room->pillars_number;i++){
        if(next_move.x==room->pillars[i]->loc.x && next_move.y==room->pillars[i]->loc.y){
            return 0;
        }
    }
    return 1;
}

Corridor * in_corridor(Level *level,Point loc){
    for(int i=0;i<level->corrs_number;i++){
        for(int j=0;j<level->corrs[i]->locs_count;j++){
            if(loc.x==level->corrs[i]->locs[j].x && loc.y==level->corrs[i]->locs[j].y){
                return level->corrs[i];
            }
        }
    }
    return NULL;
}

int in_staircase(Level *level,Point loc){
    if(loc.x==level->staircase->loc.x && loc.y==level->staircase->loc.y){
        return 1;
    }
    return 0;
}
int in_bstaircase(Level *level,Point loc){
    if(loc.x==level->bstaircase->loc.x && loc.y==level->bstaircase->loc.y){
        return 1;
    }
    return 0;
}



void unlockdoor(Level *level, Player *player, Door *first){
    if(which_room(level,player->loc) != NULL){
        Door *second;
        for(int i=0;i<level->corrs_number;i++){
            if(level->corrs[i]->node1==first){
                second = level->corrs[i]->node2;
                break;
            }
        }
        first->kind=UNLOCKED;
        second->kind=UNLOCKED;
    }
}

int generatepassword(){
    int pass= (rand() % 9000) + 1000;
    return pass;
}


int handlegeneration(Level *level, Player *player){
    Room *room=which_room(level,player->loc);
    if(room != NULL){
        if(room->shouldgen){
            if(room->gen->loc.x==player->loc.x && room->gen->loc.y==player->loc.y){
                if(!room->gen->generated){
                    room->gen->password=generatepassword();
                    room->gen->generated=1;
                    FirstTime=time(NULL);
                    player->should_pass=1;
                }
                return 1;
            }
            if(!player->should_pass){
                room->gen->generated=0;
            }
        }
    }
    return 0;
}


void handleRegen(Player *player){
    int x = player->sat / (MAXSAT / 4) - 1;
    player->health+= x * player->pcof[HEALTH];
    if(player->health>MAXHEALTH){
        player->health=MAXHEALTH;
    }
}


void handleRot(Player *player){ 
    time_t second=time(NULL);
    double difference;
    for(int w=0;w<4;w++){
        for(int i=0;i<player->diffc[w];i++){
            TakenFood *f=player->takenfoods[w][i];
            difference=difftime(second,f->ttaken);
            if(difference>30){
                f->ttaken=second;
                switch(w){
                    case MAM:
                        player->takenfoods[FASED][player->diffc[FASED]++]=f;
                        player->diffc[w]-=1;
                        break;
                    case AALA:
                    case JADOO:
                        player->takenfoods[MAM][player->diffc[MAM]++]=f;
                        player->diffc[w]-=1;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}


void sort_keys(Player *player, int reversed){
    for(int i=0;i<player->akey_count;i++){
        for(int j=i;j<player->akey_count;j++){
            if(reversed==1){
                if(player->akeys[j]->broken<player->akeys[i]->broken){
                    aKey *temp=player->akeys[i];
                    player->akeys[i]=player->akeys[j];
                    player->akeys[j]=temp;
                }
            }else{
                if(player->akeys[j]->broken>player->akeys[i]->broken){
                    aKey *temp=player->akeys[i];
                    player->akeys[i]=player->akeys[j];
                    player->akeys[j]=temp;
                }
            }
        }
    }
}

int handlePlayermove(Level *level,int ch,Player *player,WINDOW *gamewin){
    Point np;
    int nx;
    int ny;
    switch (ch)
    {
    case 'w':
        nx=player->loc.x;
        ny=player->loc.y-1;
        break;
    case 'a':
        nx=player->loc.x-1;
        ny=player->loc.y;
        break;
    case 's':
        nx=player->loc.x;
        ny=player->loc.y+1;
        break;
    case 'd':
        nx=player->loc.x+1;
        ny=player->loc.y;
        break;
    case KEY_LEFT:
        nx=player->loc.x-1;
        ny=player->loc.y-1;
        break;
    case KEY_UP:
        nx=player->loc.x+1;
        ny=player->loc.y-1;
        break;
    case KEY_RIGHT:
        nx=player->loc.x+1;
        ny=player->loc.y+1;
        break;
    case KEY_DOWN:
        nx=player->loc.x-1;
        ny=player->loc.y+1;
        break;
    default:
        return 0;
    }
    np.x=nx;
    np.y=ny;
    Room *room=which_room(level,player->loc);
    if(is_door(level,np)!=NULL && !is_door(level,np)->show){
        is_door(level,np)->show=1;
        return 0;
    }
    if(is_window(level,np)!=NULL){
        Window *win=is_window(level,np);
        Point loc;
        loc=np;
        if(win->side){
            while(loc.x<win_width){
                loc.x+=1;
                if(which_room(level,loc)!=NULL){
                    which_room(level,loc)->show=1;
                    return 0;
                }
            }
        }else{
            while(loc.x>0){
                loc.x-=1;
                if(which_room(level,loc) !=NULL){
                    which_room(level,loc)->show=1;
                    return 0;
                }
            }
        }
        return 0;
    }
    if(is_door(level,np)!=NULL && is_door(level,np)->kind==PASS){
        if(room==NULL){
            return 0;
        }
        for(;room->tries<3;room->tries++){
            if(in_corridor(level,player->loc)==NULL){
                int takenpass=importpasswin(level,player, room->tries);
                if(takenpass==-1){
                    return 0;
                }
                if(room->gen->password==takenpass){
                    unlockdoor(level,player,(Door *)is_door(level,np));   
                    return 0;
                }
            }
        }
        return 0; // locker room
    }


    if(is_door(level,player->loc)!=NULL && (ch=='w' || ch=='s')){
        player->loc.x=np.x;
        player->loc.y=np.y;
        player->sat--;
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
        if(room!=NULL){
            if(room->rt==ENCHANT){
                player->sat-=4;
            }
        }
        return 1;
    }
    else if(check_wall_collide(level,room,np) || in_corridor(level,np)!=NULL){
        player->loc.x=np.x;
        player->loc.y=np.y;
        player->sat--;
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
        if(room!=NULL){
            if(room->rt==ENCHANT){
                player->sat-=4;
            }
        }
        return 1;
    }
    return 0;
}


void handleEnemyDeath(Level *level, Player *player){
    Room *room=which_room(level,player->loc);
    if(room == NULL){
        return;
    }
    for(int i=0;i<room->enemies_number;i++){
        if(room->enemies[i]->health<=0){
            room->enemies[i]->health=0;
            room->enemies[i]->alive=0;
        }
    }
}


int handleTrajectorymove(Level *level,Weapon * weapon,Point wloc,WINDOW *gamewin,
int wway,int x,Player *player, WINDOW *chatwin ){
    Point np;
    int nx,ny;
    char xx;
    char yy;
    switch (wway)
    {
    case 'w':
        nx=wloc.x;
        ny=wloc.y-1-x;
        xx = '^';
        yy = '|';
        break;
    case 'a':
        nx=wloc.x-1-x;
        ny=wloc.y;
        xx = '<';
        yy = '-';
        break;
    case 's':
        nx=wloc.x;
        ny=wloc.y+1+x;
        xx = 'v';
        yy = '|';
        break;
    case 'd':
        nx=wloc.x+1+x;
        ny=wloc.y;
        xx = '>';
        yy = '-';
        break;
    default:
        break;
    }
    np.x=nx;
    np.y=ny;
    
    Room *room=which_room(level,np);
    if(room == NULL){
        return 0;
    }
    if(check_wall_collide(level,room,np) || in_corridor(level,np)!=NULL){
        wrefresh(gamewin);
        for(int i=0;i<room->enemies_number;i++){
            if(np.x==room->enemies[i]->loc.x && np.y==room->enemies[i]->loc.y && room->enemies[i]->alive){
                mvwprintw(gamewin,np.y,np.x,"%c",xx);
                wrefresh(gamewin);
                usleep(50000);
                int damage=0;
                switch(weapon->weapon){
                    case ARROW:
                        damage=5;
                        break;
                    case WAND:
                        damage=15;
                        room->enemies[i]->crippled=1;
                        break;
                    case DAGGER:
                        damage=12;
                        break;
                    default:
                        break;
                }
                room->enemies[i]->health-= damage * player->pcof[DAMAGE];
                chat_hit_enemy(chatwin,player,room->enemies[i]);
                return 2;
            }
        }
        mvwprintw(gamewin,np.y,np.x,"%c",yy);
        wrefresh(gamewin);
        usleep(30000);
        return 1;
    }
    return 0;

}


void spawnNewWeapon(Room *room,Player *player,int i, int wway,Weapon *wep){
    Point np;
    int nx,ny;
    switch (wway)
    {
    case 'w':
        nx=player->loc.x;
        ny=player->loc.y-i;
        break;
    case 'a':
        nx=player->loc.x-i;
        ny=player->loc.y;
        break;
    case 's':
        nx=player->loc.x;
        ny=player->loc.y+i;
        break;
    case 'd':
        nx=player->loc.x+i;
        ny=player->loc.y;
        break;
    default:
        break;
    }
    np.x=nx;
    np.y=ny;
    Weapon *newwep=(Weapon *)malloc(sizeof(Weapon));
    newwep->loc=np;
    newwep->weapon=wep->weapon;
    newwep->taken=0;
    switch(wep->weapon){
        case DAGGER:
            newwep->ifdn=1;
            strcpy(newwep->code,"\u2020");
            break;
        case WAND:
            newwep->ifwn=1;
            strcpy(newwep->code,"\u2628");
            break;
        case ARROW:
            newwep->ifan=1;
            strcpy(newwep->code,"\u2671");
            break;
        default:
            break;
    }
    room->weapons[room->weapons_number++]=newwep;
}

int handleDamage(Player *player,Level * level,WINDOW *gamewin, int lh,WINDOW *chatwin){
    Room * room=which_room(level,player->loc);
    if(room == NULL){
        return 0;
    }
    Weapon *wep=player->current_weapon;
    switch(wep->weapon){
        case MACE:
        case SWORD:{
            int hit=0;
            for(int i=player->loc.x-1 ; i <= player->loc.x+1 ; i++){
                for(int j=player->loc.y-1 ; j <= player->loc.y+1 ; j++){
                    for(int which=0;which<room->enemies_number;which++){
                        Enemy * e = room->enemies[which];
                        if(!e->alive){
                            continue;
                        }
                        if(e->loc.x == i && e->loc.y == j){
                            hit =1 ;
                            switch(wep->weapon){
                                case MACE:
                                    e->health -= 5 * player->pcof[DAMAGE];
                                    break;
                                case SWORD:{
                                    player->damage=10 + (player->swordcount-1) * 5;
                                    e->health -= player->damage * player->pcof[DAMAGE];
                                    break;
                                }
                                default:
                                    break;
                            }
                            chat_hit_enemy(chatwin,player,e);
                        }

                    }
                }
            }
            return hit? 1 : 0;
        }


        case WAND:
        case ARROW:
        case DAGGER:{
            int enough=1;
            int traj=0;
            int wway;
            if(lh){
                wway=lh;
            }else{
                wway=wgetch(gamewin);
            }
            player->lasthit=wway;
            switch(wep->weapon){
                case WAND:
                    traj=10;
                    if(player->wandcount<=0){
                        enough=0;
                        break;
                    }
                    player->wandcount-=1;
                    break;
                case DAGGER:
                    traj=5;
                    if(player->dagcount<=0){
                        enough=0;
                        break;
                    }
                    player->dagcount-=1;
                    break;
                case ARROW:
                    traj=5;
                    if(player->arrowcount<=0){
                        enough=0;
                        break;
                    }
                    player->arrowcount-=1;
                    break;
                default:
                    break;
            }
            
            if(!enough){
                return 0;
            }
            for(int i=0;i<traj;i++){
                int res=handleTrajectorymove(level,wep,player->loc,gamewin,wway,i,player,chatwin);
                if(res==0){
                    spawnNewWeapon(room,player,i,wway,wep);
                    return 0;
                }else if(res==1){
                    continue;
                }else if(res==2){
                    return 1;
                }
                wrefresh(gamewin);
            }
            
            spawnNewWeapon(room,player,traj,wway,wep);
            return 0;
        }
    }
    return 0;
}



int handleEnemymove(Level *level,Player *player,WINDOW *gamewin,WINDOW *chatwin){
    Room * room=which_room(level,player->loc);
    if(room==NULL){
        return 0;
    }
    for(int i=0;i<room->enemies_number;i++){
        Enemy *e=room->enemies[i];
        if(!e->alive || e->crippled){
            continue;
        }
        if(-1 <= player->loc.x - e->loc.x && player->loc.x - e->loc.x <= 1 &&
        -1 <= player->loc.y - e->loc.y && player->loc.y - e->loc.y <= 1){
            switch(e->en){
                case GIANT:
                case UNDEED:
                    e->trigerred=5;
                    break;
                default:
                    break;
            }
            switch(e->en){
                case DEAMON:
                    player->health-=5;
                    break;
                case FBM:
                    player->health-=10;
                    break;
                case GIANT:
                    player->health-=15;
                    break;
                case SNAKE:
                    player->health-=20;
                    break;
                case UNDEED:
                    player->health-=30;
                    break;
            }
            chat_enemy_hit(chatwin,e);
        }
        else if(e->trigerred){
            Point np;
            int nx=e->loc.x;
            int ny=e->loc.y;
            int which= rand() % 2;
            if(is_door(level,e->loc)!=NULL){
                which=1;
            }
            for(int k=0;k<1;k++){
                if(which){
                    nx=e->loc.x;
                    if(player->loc.y>e->loc.y){
                        ny=e->loc.y+1;
                    }else if(player->loc.y<e->loc.y){
                        ny=e->loc.y-1;
                    }else{
                        which=0;
                        k-=1;
                    }
                }else{
                    ny=e->loc.y;
                    if(player->loc.x>e->loc.x){
                        nx=e->loc.x+1;
                    }else if(player->loc.x<e->loc.x){
                        nx=e->loc.x-1;
                    }else{
                        which=1;
                        k-=1;
                    }
                }
            }
            np.x=nx;
            np.y=ny;
            if(check_wall_collide(level,room,np) || in_corridor(level,np)!=NULL){
                switch(e->en){
                    case GIANT:
                    case UNDEED:
                        e->trigerred-=1;
                        e->loc=np;
                        break;
                    case SNAKE:
                        e->loc=np;
                        break;
                    default:
                        break;
                }
                return 1;
            }
            return 0;
        }
    }
    return 0;



}


int ReverseNumber(int num){
    int reversed = 0;
    
    while (num != 0) {
        reversed = reversed * 10 + num % 10;
        num /= 10;
    }
    return reversed;
}

int corr_index(Point loc, Corridor * corr){
    for(int i=0;i<corr->locs_count;i++){
        if(loc.x==corr->locs[i].x && loc.y==corr->locs[i].y){
            return i;
        }
    }
    return -1;
}

int LenFood(Player *player){
    int result=0;
    for(int i=0;i<4;i++){
        result+=player->diffc[i];
    }
    return result;
}

void handleVision(Level* level,Player* player, WINDOW *gamewin, WINDOW *chatwin){
    Room* room=which_room(level,player->loc);
    if(!player->fastmove){
        if(room!=NULL){
            if(!room->show){
                room->show=1;
                chat_new_room(chatwin);
            }
            for(int i=0;i<room->traps_number;i++){
                if(player->loc.x==room->traps[i]->loc.x && player->loc.y==room->traps[i]->loc.y){
                    if(room->traps[i]->taken){
                        continue;
                    }
                    room->traps[i]->taken=1;
                    if(!strcmp(settings->difficulty,"hard")){
                        player->health-=50;
                    }
                    else if(!strcmp(settings->difficulty,"medium")){
                        player->health-=30;
                    }
                    else if(!strcmp(settings->difficulty,"easy")){
                        player->health-=20;
                    }
                    else{
                        player->health=0;
                    }


                    break;
                }
            }

            if(player->akey_count < MAX_AKEY_COUNT){
                if(player->loc.x==level->akey->loc.x && player->loc.y==level->akey->loc.y){
                    if(!level->akey->taken){
                        level->akey->taken=1;
                        player->akeys[player->akey_count++]=level->akey;
                        
                    }
                }
            }


            if(room->rt==NIGHTMARE || player->passive){
                return;
            }


            for(int i=0;i<room->golds_number;i++){
                if(player->loc.x==room->golds[i]->loc.x && player->loc.y==room->golds[i]->loc.y){
                    if(room->golds[i]->taken){
                        continue;
                    }
                    room->golds[i]->taken=1;
                    player->golds+=room->golds[i]->value;
                    chat_gold(chatwin,room->golds[i]);
                    break;
                }
            }

            if(LenFood(player)<MAX_FOOD_COUNT){
                for(int i=0;i<room->foods_number;i++){
                    if((player->loc.x==room->foods[i]->loc.x && player->loc.y==room->foods[i]->loc.y) ||
                    (player->loc.x-1==room->foods[i]->loc.x && player->loc.y==room->foods[i]->loc.y)){
                        if(room->foods[i]->taken){
                            continue;
                        }

                        room->foods[i]->taken=1;
                        player->takenfoods[room->foods[i]->kind][player->diffc[room->foods[i]->kind]]->ttaken=time(NULL);
                        player->diffc[room->foods[i]->kind]+=1;
                        break;
                    }
                }
            }
 

            for(int i=0;i<room->weapons_number;i++){
                if((player->loc.x==room->weapons[i]->loc.x && player->loc.y==room->weapons[i]->loc.y) ||
                (player->loc.x-1==room->weapons[i]->loc.x && player->loc.y==room->weapons[i]->loc.y)){
                    if(player->loc.x-1==room->weapons[i]->loc.x && player->loc.y==room->weapons[i]->loc.y){
                        switch(room->weapons[i]->weapon){
                            case DAGGER:
                                if(room->weapons[i]->ifdn==1){
                                    continue;
                                }
                                break;
                            case WAND:
                                if(room->weapons[i]->ifwn==1){
                                    continue;
                                }
                                break;
                            case ARROW:
                                if(room->weapons[i]->ifan==1){
                                    continue;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    if(room->weapons[i]->taken){
                        continue;
                    }
                    room->weapons[i]->taken=1;
                    if(!player->wktaken[room->weapons[i]->weapon]){
                        player->weapons[player->weapons_count++]=room->weapons[i];
                        player->wktaken[room->weapons[i]->weapon]=1;
                    }
                    switch(room->weapons[i]->weapon){
                        case DAGGER:
                            player->dagcount+=room->weapons[i]->ifdn;
                            break;
                        case WAND:
                            player->wandcount+=room->weapons[i]->ifwn;
                            break;
                        case ARROW:
                            player->arrowcount+=room->weapons[i]->ifan;
                            break;
                        case SWORD:
                            player->swordcount+=1;
                        default:
                            break;
                    }
                    break;
                }
            }
            

            if(player->potions_count<MAX_POTION_COUNT){
                for(int i=0;i<room->potions_number;i++){
                    if((player->loc.x==room->potions[i]->loc.x && player->loc.y==room->potions[i]->loc.y) ||
                    (player->loc.x-1==room->potions[i]->loc.x && player->loc.y==room->potions[i]->loc.y)){
                        if(room->potions[i]->taken){
                            continue;
                        }
                        room->potions[i]->taken=1;
                        player->potions[player->potions_count++]=room->potions[i];
                        player->diffp[room->potions[i]->potion]+=1;
                        break;
                    }
                }
            }

        }
    }
    if(room!=NULL){
        if(!room->show){
            room->show=1;
        }
    }
    Corridor * corr=in_corridor(level,player->loc);
    if(corr!=NULL){
        int index_corr=corr_index(player->loc,corr);
        for(int i=index_corr;i<=index_corr+5 && i<corr->locs_count;i++){
            corr->show[i]=1;
        }
    }

}



void initialize_doors(Room *room) {
    room->doors = malloc(room->door_number * sizeof(Door*));
    for (int i = 0; i < room->door_number; i++) {
        room->doors[i] = malloc(sizeof(Door));
    }
}

void add_doors_to_room(Room *room,int which) {
    if(which==0){
        room->door_number = 1;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2) ) + 1; // bottom
        room->doors[0]->loc.y = room->start.y + room->height-1;
    }
    else if(which==5){
        room->door_number = 1;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2)) + 1; // top
        room->doors[0]->loc.y = room->start.y;
    }
    else if(which==2 || which==3){
        room->door_number = 2;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2)) + 1; // top
        room->doors[0]->loc.y = room->start.y;
        room->doors[1]->loc.x = room->start.x + (rand() % (room->width-2) ) + 1; // bottom
        room->doors[1]->loc.y = room->start.y + room->height-1;
    }
    else if(which==1 || which==4){
        room->door_number = 2;
        initialize_doors(room);
        room->doors[0]->loc.x = room->start.x + (rand() % (room->width-2) ) + 1; // bottom
        room->doors[0]->loc.y = room->start.y + room->height-1;
        room->doors[1]->loc.x = room->start.x + (rand() %( room->width-2)) + 1; // top
        room->doors[1]->loc.y = room->start.y;
    }
}

void add_lockshow_to_level(Level *level){
    for(int i=0;i<level->len_rooms;i++){
        for(int j=0;j<level->rooms[i]->door_number;j++){
            if(level->rooms[i]->doors[j]->kind==PASS){
                level->rooms[i]->doors[j]->locked=1;
            }else{
                level->rooms[i]->doors[j]->locked=0;
            }

            if(level->rooms[i]->doors[j]->kind==HIDDEN){
                level->rooms[i]->doors[j]->show=0;
            }else{
                level->rooms[i]->doors[j]->show=1;
            }

        }

    }
}

void add_pillars_to_room(Room *room){
    room->pillars_number= rand() % ((room->height*room->width)/30);
    room->pillars=(Pillar **)malloc(room->pillars_number*sizeof(Pillar *));
    for(int i=0;i<room->pillars_number;i++){
        Point first_guess;
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                j=0;
                first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
                first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
            }
        }
        room->pillars[i]=(Pillar *)malloc(sizeof(Pillar));
        room->pillars[i]->loc.x=first_guess.x;
        room->pillars[i]->loc.y=first_guess.y;
    }
}

void add_golds_to_room(Room *room){
    room->golds_number = rand() %((room->height*room->width)/30);
    room->golds_number+=2;
    if(room->rt==ENCHANT){
        room->golds_number = rand() %((room->height*room->width)/50);
    }else if(room->rt == TREASURE){
        room->golds_number = rand() %((room->height*room->width)/20) + 5;
        room->golds_number*=2;
    }
    room->golds=(Gold **)malloc(room->golds_number*sizeof(Gold *));
    for(int i=0;i<room->golds_number;i++){
        int check=0;
        Point first_guess;        
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if(first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->pillars_number;j++){
            if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                check=1;
                break;
            }
        }
        if(check){
            i--;
            continue;
        }
        room->golds[i]=(Gold *)malloc(sizeof(Gold));
        room->golds[i]->loc=first_guess;
        room->golds[i]->taken=0;
        int z = rand() % 10;
        if(room->rt==TREASURE){
            z = rand() % 5;
        }
        if(z){
            z=GOLD;
        }else{
            z=BLACK;
        }
        room->golds[i]->gtype=z;
        if(room->golds[i]->gtype){
            strcpy(room->golds[i]->code,"\u2727");
        }else{
            strcpy(room->golds[i]->code,"\u25C6");
        }
    }
}

void add_foods_to_room(Room *room){
    room->foods_number = rand() %((room->height*room->width)/30);
    room->foods_number+=1;
    if(room->rt==ENCHANT){
        room->foods_number=1;
    }
    else if(room->rt==TREASURE){
        room->foods_number=0;
    }
    room->foods=(Food **)malloc(room->foods_number*sizeof(Food *));
    for(int i=0;i<room->foods_number;i++){
        int check=0;
        Point first_guess;        
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x+1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->pillars_number;j++){
            if((first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y) || 
            (first_guess.x+1==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y)){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if((first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y) || 
            (first_guess.x+1==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y)){
                check=1;
                break;
            }
        }
        if(check){
            i--;    
            continue;
        }
        room->foods[i]=(Food *)malloc(sizeof(Food));
        room->foods[i]->loc=first_guess;
        room->foods[i]->taken=0;
        int x = rand() % 4;
        room->foods[i]->kind=x;
        switch(room->foods[i]->kind){
            case MAM:
            case FASED:
                strcpy(room->foods[i]->code,"\U0001F353");
                break;
            case AALA:
                strcpy(room->foods[i]->code,"\U0001F355");
                break;
            case JADOO:
                strcpy(room->foods[i]->code,"\U0001F35F");
                break;
            default:
                break;
        }
    }
}

void add_traps_to_room(Room *room){
    room->traps_number = rand() %((room->height*room->width)/40);
    room->traps_number+=1;
    if(room->rt==TREASURE){
        room->traps_number= rand() %((room->height*room->width)/20) + 3;
        room->traps_number *= 2;
    }
    room->traps=(Trap **)malloc(room->traps_number*sizeof(Trap *));
    for(int i=0;i<room->traps_number;i++){
        int check=0;
        Point first_guess;        
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if(first_guess.x==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->pillars_number;j++){
            if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if(first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->foods_number;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        if(check){
            i--;    
            continue;
        }
        room->traps[i]=(Trap *)malloc(sizeof(Trap));
        room->traps[i]->loc=first_guess;
        room->traps[i]->taken=0;
    }
}


void add_potions_to_room(Room *room){
    room->potions_number = rand() %((room->height*room->width)/35);
    if(room->rt==ENCHANT){
        room->potions_number += 2;
        room->potions_number *= 3;
        room->potions_number /= 2;
    }else if(room->rt==TREASURE){
        room->potions_number=0;
    }
    room->potions=(Potion **)malloc(room->potions_number*sizeof(Potion *));
    for(int i=0;i<room->potions_number;i++){
        int check=0;
        Point first_guess;        
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if((first_guess.x==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
             (first_guess.x+1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
             (first_guess.x-1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->pillars_number;j++){
            if((first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y) ||
            (first_guess.x+1==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y)
            ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if((first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y )||
            (first_guess.x+1==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y)
            ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->foods_number;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x+1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->traps_number;j++){
            if((first_guess.x==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y) || 
            (first_guess.x+1==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        if(check){
            i--;
            continue;   
        }
        room->potions[i]=(Potion *)malloc(sizeof(Potion));
        room->potions[i]->loc=first_guess;
        room->potions[i]->taken=0;
        room->potions[i]->potion= rand () % 3;
        switch(room->potions[i]->potion){
            case SPEED:
                strcpy(room->potions[i]->code,"\U0001F37E");
                break;
            case HEALTH:
                strcpy(room->potions[i]->code,"\U0001F377");
                break;
            case DAMAGE:
                strcpy(room->potions[i]->code,"\U0001F37C");
                break;
            default:
                strcpy(room->potions[i]->code,"P");
                break;
        }
    }
}


void add_weapons_to_room(Room *room){
    room->weapons_number = rand() %((room->height*room->width)/35);
    if(room->rt==TREASURE){
        room->weapons_number=0;
    }else if(room->rt==ENCHANT){
        room->weapons_number+=2;
    }
    room->weapons=(Weapon **)malloc(MAX_WEAPON_IN_ROOM*sizeof(Weapon *));
    for(int i=0;i<room->weapons_number;i++){
        int check=0;
        Point first_guess;        
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if((first_guess.x==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
             (first_guess.x+1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
             (first_guess.x-1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->pillars_number;j++){
            if((first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y) ||
            (first_guess.x+1==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y)
            ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if((first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y )||
            (first_guess.x+1==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y)
            ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->foods_number;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x+1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->traps_number;j++){
            if((first_guess.x==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y) || 
            (first_guess.x+1==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->potions_number;j++){
            if((first_guess.x==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
             (first_guess.x+1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
             (first_guess.x-1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ){
                check=1;
                break;
            }
        }
        if(check){
            i--;
            continue;   
        }
        room->weapons[i]=(Weapon *)malloc(sizeof(Weapon));
        room->weapons[i]->loc=first_guess;
        room->weapons[i]->taken=0;
        room->weapons[i]->weapon= rand () % 4 + 1;
        switch(room->weapons[i]->weapon){
            case MACE:
                strcpy(room->weapons[i]->code,"\u2692");
                break;
            case DAGGER:
                strcpy(room->weapons[i]->code,"\U0001F5E1");
                room->weapons[i]->ifdn=10;
                break;
            case WAND:
                strcpy(room->weapons[i]->code,"\U0001FA84");
                room->weapons[i]->ifwn=8;
                break;
            case ARROW:
                strcpy(room->weapons[i]->code,"\u27B3");
                room->weapons[i]->ifan=5;
                break;
            case SWORD:
                strcpy(room->weapons[i]->code,"\u2694");
                break;
            default:
                strcpy(room->weapons[i]->code,"W");
                break;
        }

    }
}


void  add_staircase_to_level(Level *level){
    Room *room=level->rooms[level->len_rooms-1];
    Point first_guess;
    for(int i=0;i<1;i++){
        int check=0;     
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<room->pillars_number;j++){
            if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if(first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->foods_number;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->traps_number;j++){
            if(first_guess.x==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->potions_number;j++){
            if((first_guess.x==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
             (first_guess.x+1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
             (first_guess.x-1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->weapons_number;j++){
            if((first_guess.x==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
             (first_guess.x+1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
             (first_guess.x-1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ){
                check=1;
                break;
            }
        }
        if(check){
            i--;
            continue;   
        }
    }       
    level->staircase=(Staircase *)malloc(sizeof(Staircase));
    level->staircase->loc.x=first_guess.x;
    level->staircase->loc.y=first_guess.y;
    level->staircase->c='>';


    room=level->rooms[0];
    Point second_guess;
    for(int i=0;i<1;i++){
        int check=0;     
        second_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        second_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<room->pillars_number;j++){
            if(second_guess.x==room->pillars[j]->loc.x && second_guess.y==room->pillars[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if(second_guess.x==room->golds[j]->loc.x && second_guess.y==room->golds[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->foods_number;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->traps_number;j++){
            if(second_guess.x==room->traps[j]->loc.x && second_guess.y==room->traps[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->potions_number;j++){
            if((second_guess.x==room->potions[j]->loc.x && second_guess.y==room->potions[j]->loc.y) ||
             (second_guess.x+1==room->potions[j]->loc.x && second_guess.y==room->potions[j]->loc.y) ||
             (second_guess.x-1==room->potions[j]->loc.x && second_guess.y==room->potions[j]->loc.y) ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->weapons_number;j++){
            if((second_guess.x==room->weapons[j]->loc.x && second_guess.y==room->weapons[j]->loc.y) ||
             (second_guess.x+1==room->weapons[j]->loc.x && second_guess.y==room->weapons[j]->loc.y) ||
             (second_guess.x-1==room->weapons[j]->loc.x && second_guess.y==room->weapons[j]->loc.y) ){
                check=1;
                break;
            }
        }
        if(check){
            i--;
            continue;   
        }
    }       
    level->bstaircase=(Staircase *)malloc(sizeof(Staircase));
    level->bstaircase->loc.x=second_guess.x;
    level->bstaircase->loc.y=second_guess.y;
    level->bstaircase->c='<';



}


void add_gen(Level *level){
    for(int iter=0;iter<level->len_rooms;iter++){
        if(!level->rooms[iter]->shouldgen){
            continue;
        }

        Room *room=level->rooms[iter];
        Point first_guess;
        for(int i=0;i<1;i++){
            int check=0;     
            first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
            first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
            for(int j=0;j<room->pillars_number;j++){
                if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                    check=1;
                    break;
                }
            }
            for(int j=0;j<room->golds_number;j++){
                if(first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y){
                    check=1;
                    break;
                }
            }
            for(int j=0;j<room->foods_number;j++){
                if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
                (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                    check=1;
                    break;
                }
            }
            for(int j=0;j<room->traps_number;j++){
                if(first_guess.x==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y){
                    check=1;
                    break;
                }
            }
            for(int j=0;j<room->potions_number;j++){
                if((first_guess.x==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
                    (first_guess.x+1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
                    (first_guess.x-1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ){
                    check=1;
                    break;
                }
            }
            for(int j=0;j<room->weapons_number;j++){
                if((first_guess.x==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
                    (first_guess.x+1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
                    (first_guess.x-1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ){
                    check=1;
                    break;
                }
            }
            if(!iter){
                if(first_guess.x==level->bstaircase->loc.x && first_guess.y==level->bstaircase->loc.y){
                    check=1;
                }
            }
            if(check){
                i--;
                continue;   
            }
        }

        room->gen=(Gen *)malloc(sizeof(Gen));
        room->gen->loc=first_guess;
        room->gen->generated=0;
        room->gen->password=0;
    }
}


void add_akey(Level *level){
    Room *room=level->rooms[level->wroomkey];
    Point first_guess;
    for(int i=0;i<1;i++){
        int check=0;     
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<room->pillars_number;j++){
            if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if(first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->foods_number;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->traps_number;j++){
            if(first_guess.x==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->potions_number;j++){
            if((first_guess.x==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
                (first_guess.x+1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
                (first_guess.x-1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->weapons_number;j++){
            if((first_guess.x==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
                (first_guess.x+1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
                (first_guess.x-1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ){
                check=1;
                break;
            }
        }
        if(!level->wroomkey){
            if(first_guess.x==level->bstaircase->loc.x && first_guess.y==level->bstaircase->loc.y){
                check=1;
            }
        }
        if(level->wroomkey==level->len_rooms-1){
            if(first_guess.x==level->staircase->loc.x && first_guess.y==level->staircase->loc.y){
                check=1;
            }
        }
        if(room->shouldgen){
            if(first_guess.x==room->gen->loc.x && first_guess.y==room->gen->loc.y){
                check=1;
            }
        }
        if(check){
            i--;
            continue;   
        }
    }

    level->akey=(aKey *)malloc(sizeof(aKey));
    level->akey->loc=first_guess;
    level->akey->taken=0;
    int randomnum;
    randomnum = rand() % 5; //  randomnum = rand() % 10; for being broken by 10%
    if(!randomnum){
        level->akey->broken=1;
    }else{
        level->akey->broken=0;
    }
}    


void add_enemies_to_room(Room *room,Level * level){
    room->enemies_number = rand() %((room->height*room->width)/40);
    if(room->rt==ENCHANT){
        room->enemies_number=0;
    }
    room->enemies=(Enemy **)malloc(room->enemies_number*sizeof(Enemy *));
    for(int i=0;i<room->enemies_number;i++){
        Point first_guess;
        int check=0;     
        first_guess.x= (rand() % (room->width-4)) + 2 + room->start.x;
        first_guess.y= (rand() % (room->height-4)) + 2 + room->start.y;
        for(int j=0;j<i;j++){
            if(first_guess.x==room->enemies[j]->loc.x && first_guess.y==room->enemies[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->pillars_number;j++){
            if(first_guess.x==room->pillars[j]->loc.x && first_guess.y==room->pillars[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->golds_number;j++){
            if(first_guess.x==room->golds[j]->loc.x && first_guess.y==room->golds[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->foods_number;j++){
            if((first_guess.x==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y) || 
            (first_guess.x-1==room->foods[j]->loc.x && first_guess.y==room->foods[j]->loc.y)){ // temp
                check=1;
                break;
            }
        }
        for(int j=0;j<room->traps_number;j++){
            if(first_guess.x==room->traps[j]->loc.x && first_guess.y==room->traps[j]->loc.y){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->potions_number;j++){
            if((first_guess.x==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
                (first_guess.x+1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ||
                (first_guess.x-1==room->potions[j]->loc.x && first_guess.y==room->potions[j]->loc.y) ){
                check=1;
                break;
            }
        }
        for(int j=0;j<room->weapons_number;j++){
            if((first_guess.x==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
                (first_guess.x+1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ||
                (first_guess.x-1==room->weapons[j]->loc.x && first_guess.y==room->weapons[j]->loc.y) ){
                check=1;
                break;
            }
        }
        int iter;
        for(iter=0;iter<level->len_rooms;iter++){
            if(level->rooms[iter]==room){
                break;
            }
        }
        if(level->wroomkey == iter){
            if(first_guess.x==level->akey->loc.x && first_guess.y==level->akey->loc.y){
                check=1;
            }
        }
        if(iter==0){
            if(first_guess.x==level->bstaircase->loc.x && first_guess.y==level->bstaircase->loc.y){
                check=1;
            }
        }
        if(iter==level->len_rooms-1){
            if(first_guess.x==level->staircase->loc.x && first_guess.y==level->staircase->loc.y){
                check=1;
            }
        }
        if(room->shouldgen){
            if(first_guess.x==room->gen->loc.x && first_guess.y==room->gen->loc.y){
                check=1;
            }
        }
        if(check){
            i--;
            continue;   
        }
        room->enemies[i]=(Enemy *)malloc(sizeof(Enemy));
        room->enemies[i]->loc=first_guess;
        room->enemies[i]->alive=1;
        room->enemies[i]->crippled=0;
        room->enemies[i]->en = rand () % 5;
        switch(room->enemies[i]->en){
            case DEAMON:
                strcpy(room->enemies[i]->code,"D");
                room->enemies[i]->health=5;
                room->enemies[i]->trigerred=0;
                break;
            case FBM:
                strcpy(room->enemies[i]->code,"F");
                room->enemies[i]->health=10;
                room->enemies[i]->trigerred=0;
                break;
            case GIANT:
                strcpy(room->enemies[i]->code,"G");
                room->enemies[i]->health=15;
                room->enemies[i]->trigerred=0;
                break;
            case SNAKE:
                strcpy(room->enemies[i]->code,"S");
                room->enemies[i]->health=20;
                room->enemies[i]->trigerred=-1;
                break;
            case UNDEED:
                strcpy(room->enemies[i]->code,"U");
                room->enemies[i]->health=30;
                room->enemies[i]->trigerred=0;
                break;
            default:
                strcpy(room->enemies[i]->code,"E");
                room->enemies[i]->health=0;
                room->enemies[i]->trigerred=0;
                break;
        }
    }
}

void add_windows_to_room(Room *room){
    int how_many;
    // srand(time(NULL));
    how_many = (rand() % 3);
    int which;
    if(how_many==1){
        which= rand()%2;
    }
    room->windows_number=how_many;
    for(int i=0;i<how_many;i++){
        room->windows[i]=(Window *)malloc(sizeof(Window));
    }
    if(how_many==1){
        if(which){
            int y= (rand() % (room->height-2))+1;
            room->windows[0]->loc.x=room->start.x+room->width-1;
            room->windows[0]->loc.y=room->start.y+y;
        }else{
            int y= (rand() % (room->height-2))+1;
            room->windows[0]->loc.x=room->start.x;
            room->windows[0]->loc.y=room->start.y+y;
        }
        room->windows[0]->side=which;
    }
    else{
        for(int i=0;i<room->windows_number;i++){
            int y= (rand() % (room->height-2))+1;
            room->windows[i]->loc.x=room->start.x + (i?room->width-1:0);
            room->windows[i]->loc.y=room->start.y+y;
            room->windows[i]->side=i;
        }
    }
}



void defuse_traps(Level *level,Player *player){
    Room *room=which_room(level,player->loc);
    if(room==NULL){
        return;
    }
    for(int i=player->loc.x-1;i<=player->loc.x+1;i++){
        for(int j=player->loc.y;j<=player->loc.y+1;j++){
            for(int k=0;k<room->traps_number;k++){
                if(room->traps[k]->loc.x==i && room->traps[k]->loc.y==j){
                    room->traps[k]->taken=1;
                }
            }
        }
    }
}




int set_kind(){
    int kind = rand() % 12;
    if(kind>=4){
        kind=2;
    }else if(kind >= 2){
        kind=1;
    }else{
        kind=0;
    }
    return kind;
}
void add_corridors_to_level(Level *level,WINDOW *gamewin){
    for(int i=0;i<level->len_rooms;i++){
        level->rooms[i]->shouldgen=0;
    }
    int kind;
    Room **rooms=level->rooms;
    level->corrs_number=5;
    level->corrs=(Corridor **)malloc(level->corrs_number*sizeof(Corridor));
    for(int i=0;i<level->corrs_number;i++){
        level->corrs[i]=(Corridor *)malloc(sizeof(Corridor));
    }
    Corridor **corrs=level->corrs;
    int which,random_down,random_up,static_down,static_up,static_narrow,down;
    Point n1,n2;

    which=0;        // -------------------------------------------- first --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[(which?1:0)]; // bottom
    corrs[which]->node2=rooms[which+1]->doors[0]; // bottom
    random_down= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_down= (which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding) - n1.y+random_down;
    static_up=(which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding)- n2.y+random_down;
    static_narrow = n2.x - n1.x-1; // -1 is for not counting the first and last ones
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y+i+1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_down].x=n1.x+i+1;
        corrs[which]->locs[i+static_down].y=n1.y+static_down;
    }
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i+static_down+static_narrow].x=n1.x+static_narrow+1;
        corrs[which]->locs[i+static_down+static_narrow].y=n1.y+static_down-i;
    }
    
    kind = set_kind();
    corrs[which]->node1->kind=kind;
    corrs[which]->node2->kind=kind;
    if(kind==PASS){
        level->rooms[which]->shouldgen=1;        
    }
    else if(kind==HIDDEN){
        level->rooms[which+1]->rt=ENCHANT;
    }

    which++; // -------------------------------------------- second --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[1]; // top
    corrs[which]->node2=rooms[which+1]->doors[0]; // top        
    random_up= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_up= -(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding) +n1.y+random_up;
    static_down=-(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding)+n2.y+random_up;
    static_narrow = n2.x - n1.x-1; // -1 is for not counting the first and last ones
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y-i-1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_up].x=n1.x+i+1;
        corrs[which]->locs[i+static_up].y=n1.y-static_up;
    }
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i+static_up+static_narrow].y=n1.y-static_up+i;
        corrs[which]->locs[i+static_up+static_narrow].x=n1.x+static_narrow+1;
    }

    kind = set_kind();
    corrs[which]->node1->kind=kind;
    corrs[which]->node2->kind=kind;
    if(kind==PASS){
        level->rooms[which]->shouldgen=1;
    }
    else if(kind==HIDDEN){
        level->rooms[which+1]->rt=ENCHANT;
    }

    which++; // -------------------------------------------- third --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[1]; // bottom
    corrs[which]->node2=rooms[which+1]->doors[0]; // top        
    random_down= ( rand() % (2*basic_padding-1))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_down=MaxHeightSubWindow+basic_padding-n1.y + random_down;
    static_narrow=n2.x-n1.x;
    down=n2.y-n1.y-static_down;
    int way;
    if(static_narrow>0){
        static_narrow-=1;
        way=1;
    }
    else if(static_narrow<0){
        static_narrow=-static_narrow;
        static_narrow-=1;
        way=-1;
    }
    else{
        way=0;
    }
    corrs[which]->locs_count=static_down+static_narrow+down;
    corrs[which]->locs=(Point *)malloc((static_down+down+static_narrow)*sizeof(Point));
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y+i+1;
    }
    for(int i=0;i<static_narrow;i++){
        if(way>0){
            corrs[which]->locs[static_down+ i].x=n1.x+i+1;
            corrs[which]->locs[static_down + i].y=n1.y+static_down;
        }
        else if(way<0){
            corrs[which]->locs[static_down+ i].x=n1.x-i-1;
            corrs[which]->locs[static_down + i].y=n1.y+static_down;
        }
    }
    for(int i=0;i<down;i++){
        corrs[which]->locs[static_down+static_narrow+ i].x=n1.x+ way*(static_narrow+1);
        corrs[which]->locs[static_down+static_narrow+ i].y=n1.y+static_down+i;
    }


    kind = set_kind();
    corrs[which]->node1->kind=kind;
    corrs[which]->node2->kind=kind;
    if(kind==PASS){
        level->rooms[which]->shouldgen=1;

    }
    else if(kind==HIDDEN){
        level->rooms[which+1]->rt=ENCHANT;
    }

    which++; // -------------------------------------------- fourth --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[(which?1:0)]; // bottom
    corrs[which]->node2=rooms[which+1]->doors[0]; // bottom
    random_down= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_down= (which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding) - n1.y+random_down;
    static_up=(which?2*MaxHeightSubWindow+3*basic_padding:MaxHeightSubWindow+basic_padding)- n2.y+random_down;
    static_narrow = n1.x - n2.x-1; // -1 is for not counting the first and last ones
    way=-1;
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y+i+1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_down].x=n1.x-i-1;
        corrs[which]->locs[i+static_down].y=n1.y+static_down;
    }
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i+static_down+static_narrow].x=n1.x-static_narrow-1;
        corrs[which]->locs[i+static_down+static_narrow].y=n1.y+static_down-i;
    }

    kind = set_kind();
    corrs[which]->node1->kind=kind;
    corrs[which]->node2->kind=kind;
    if(kind==PASS){
        level->rooms[which]->shouldgen=1;
    }
    else if(kind==HIDDEN){
        level->rooms[which+1]->rt=ENCHANT;
    }

    which++; // -------------------------------------------- fifth --------------------------------------------
    corrs[which]->node1=(Door *)malloc(sizeof(Door));
    corrs[which]->node2=(Door *)malloc(sizeof(Door));
    corrs[which]->node1=rooms[which]->doors[1]; // top
    corrs[which]->node2=rooms[which+1]->doors[0]; // top        
    random_up= ( rand() % (basic_padding-2))+1;
    n1=corrs[which]->node1->loc;
    n2=corrs[which]->node2->loc;
    static_up= -(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding) +n1.y+random_up;
    static_down=-(which-1?MaxHeightSubWindow+3*basic_padding:basic_padding)+n2.y+random_up;
    static_narrow = n1.x - n2.x-1; // -1 is for not counting the first and last ones
    way=-1;
    corrs[which]->locs_count=static_down+static_narrow+static_up;
    corrs[which]->locs=(Point *)malloc((static_down+static_up+static_narrow)*sizeof(Point));
    for(int i=0;i<static_up;i++){
        corrs[which]->locs[i].x=n1.x;
        corrs[which]->locs[i].y=n1.y-i-1;
    }
    for(int i=0;i<static_narrow;i++){
        corrs[which]->locs[i+static_up].x=n1.x-i-1;
        corrs[which]->locs[i+static_up].y=n1.y-static_up;
    }
    for(int i=0;i<static_down;i++){
        corrs[which]->locs[i+static_up+static_narrow].y=n1.y-static_up+i;
        corrs[which]->locs[i+static_up+static_narrow].x=n1.x-static_narrow-1;
    }
    

    for(int i=0;i<level->corrs_number;i++){
        level->corrs[i]->show=(int *)malloc(level->corrs[i]->locs_count*sizeof(int));
        for(int j=0;j<level->corrs[i]->locs_count;j++){
            level->corrs[i]->show[j]=0;
        }
    }

    kind = set_kind();
    corrs[which]->node1->kind=kind;
    corrs[which]->node2->kind=kind;
    if(kind==PASS){
        level->rooms[which]->shouldgen=1;
    }
    else if(kind==HIDDEN){
        level->rooms[which+1]->rt=ENCHANT;
    }

}