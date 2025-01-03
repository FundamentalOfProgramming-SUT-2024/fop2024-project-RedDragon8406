#include <stdlib.h>
#include <string.h>
#include "game.h"


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