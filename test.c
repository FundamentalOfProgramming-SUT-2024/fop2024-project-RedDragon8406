#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "save.h"
#include "utils.h"
#include "game.h"
#include "auth.h"

int main(){
    FILE *f=fopen("saves/admin1","rb");
    Level **l=(Level **)malloc(5*sizeof(Level *));
    l[0]=(Level *)malloc(sizeof(Level));

    int x;
    fread(&x,sizeof(int),1,f);
    fread(l[0],sizeof(Level),1,f);
    fclose(f);
    printf("cl: %d\n",x);
    printf("wrk: %d\n",l[0]->wroomkey);
}