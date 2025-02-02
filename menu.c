#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "menu.h"
#include "exit.h"
#include "choices.h"
#include "design.h" 
#include "main.h" 
#include "auth.h"
#include "scoreboard.h"  
#include "profile.h" 
#include "settings.h" 
#include "game.h" // last include
void print_menu(WINDOW *menu_win, int highlight, int n_choices);
void handle_logged_in_choice(WINDOW *menu_win, int choice, int highlight, WINDOW *design_win);
void handle_logged_out_choice(WINDOW *menu_win, int choice, int highlight, WINDOW *design_win);

void design_menu_win(WINDOW *design_win);
void kill_design_win(WINDOW *design_win);
void show_main_menu() {
    clear();
    initscr();
    WINDOW *design_win;
    int dheight = 46;
    int dwidth = 134;
    int dstarty = (LINES - dheight) / 2;
    int dstartx = (COLS - dwidth) / 2 + 15;

    design_win=newwin(dheight,dwidth,dstarty,dstartx);



    n_choices = (current_user) ? NUM_CHOICES_LOGGED_IN : NUM_CHOICES_LOGGED_OUT;
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;


    noecho();
    cbreak();
    curs_set(0);

    init_colors();

    // update which choices should be shown
    update_choices();

    int height = 12;
    int width = 30;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2 ;

    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);

    const char *welcome_text = "Welcome to My Rogue Game!";
    attron(COLOR_PAIR(5)); 
    mvprintw(0, (COLS - strlen(welcome_text)) / 2, "%s", welcome_text);
    attroff(COLOR_PAIR(5)); 



    refresh();
    print_menu(menu_win, highlight, n_choices);
    design_menu_win(design_win);
    while (1) {
        print_menu(menu_win, highlight, n_choices);
        wrefresh(menu_win);
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            default:
                break;
        }
        if (current_user) {
            handle_logged_in_choice(menu_win, choice, highlight,design_win);
        } else {
            handle_logged_out_choice(menu_win, choice, highlight,design_win);
        }
    }
    endwin();
}

void print_menu(WINDOW *menu_win, int highlight, int n_choices) {
    init_pair(180,180,COLOR_BLACK);
    wattron(menu_win,A_ITALIC);
    wattron(menu_win,COLOR_PAIR(180));
    mvwprintw(menu_win,1,2,"welcome to the main menu !");
    box(menu_win,0,0);
    wattroff(menu_win,COLOR_PAIR(180));
    wattroff(menu_win,A_ITALIC);
    init_pair(180,180,COLOR_BLACK);
    int y, i;
    y = 3; //  make some space for the welcome message
    init_pair(240,240,COLOR_BLACK);
    for (i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) { // highlight the current choice
                wattron(menu_win, COLOR_PAIR(180));
                mvwprintw(menu_win, y+i, (15-strlen(choices[i])/2), "%s", choices[i]);
                wattroff(menu_win, COLOR_PAIR(180));
        } else
            mvwprintw(menu_win, y+i, (15-strlen(choices[i])/2), "%s", choices[i]);
    }
    if(current_user!=NULL){
        char path[100];
        snprintf(path,sizeof(path),"saves/%s",current_user->username);
        FILE *f=fopen(path,"rb");
        if(f!=NULL){
            fclose(f);
        }else{
            wattron(menu_win,COLOR_PAIR(240));
            mvwprintw(menu_win, y+1, (15-strlen(choices[1])/2), "%s", choices[1]);
            wattroff(menu_win,COLOR_PAIR(240));
        }
    }
    wrefresh(menu_win);
}

void handle_logged_in_choice(WINDOW *menu_win, int choice, int highlight, WINDOW *design_win) {
    switch (choice) {
        case 1: // Start Game
            kill_design_win(design_win);
            StartGame(0);
            break;
        case 2:{ // Load Game
            char path[100];
            snprintf(path,sizeof(path),"saves/%s",current_user->username);
            FILE *f=fopen(path,"rb");
            if(f!=NULL){
                kill_design_win(design_win);
                fclose(f);
                StartGame(1);
            }
            break;
        }
        case 3: // Profile
            kill_design_win(design_win);
            show_profile();
            break;
        case 4: // Scoreboard;
            kill_design_win(design_win);
            show_scoreboard();
            break;
        case 5: // Settings
            kill_design_win(design_win);
            settings_window();
            break;
        case 6: // Logout
            kill_design_win(design_win);
            logout_user();
            show_main_menu();
            break;
        case 7: // Exit
            kill_design_win(design_win);
            show_exit_confirmation(menu_win, highlight);
            break;
        default:
            break;
    }
}

void handle_logged_out_choice(WINDOW *menu_win, int choice, int highlight, WINDOW *design_win) {
    
    switch (choice) {
        case 1: // Start Game
            kill_design_win(design_win);
            StartGame(0);
            break;
        case 2: // Login/Signup
            kill_design_win(design_win);
            authentication_window();
            break;
        case 3: // Scoreboard
            kill_design_win(design_win);
            show_scoreboard();
            break;
        case 4: // Settings
            kill_design_win(design_win);
            settings_window();
            break;
        case 5: // Exit
            kill_design_win(design_win);
            show_exit_confirmation(menu_win, highlight);
            break;
        default:
            break;
    }
}


void design_menu_win(WINDOW *design_win){
    // curs_set(0);

    init_pair(180,180,COLOR_BLACK);
    // box(design_win,0,0);
 
    char *art1[] = {
        "            _..--\"\"---.",
        "           /           \".",
        "           `            l",
        "           |'._  ,._ l/\"\\",
        "           |  _J<__/.v._/",
        "            \\( ,~._,,,,-)",
        "             `-\\' `,j|",
        "                \\_,____J",
        "           .--.__)--(__.--.",
        "          /  `-----..--'. j",
        "          '.- '`--` `--' \\",
        "         //  '`---'`  `-' \\",
        "        //   '`----'`.-.-' \\",
        "      _//     `--- -'   \\'.| \\________",
        "     |  |         ) (      `.__.---- -'\\",
        "      \\7          \\`-(               ''\\\\",
        "      ||       _  /`-(               l|//7__",
        "      |l    ('  `-)-/_.--.    ",
        "      |\\     l\\_  `-'    .' ",
        "      llJ   _ _)J--._.-('    ",
        "      |||( ( '_)_  .l   \". _",
        "      ^\\\\\\||`'   \"   '\"-. \" )",
        "           \\ |           ) /",
        "           l l          / / ",
        "            \" \\        / /  ",
        "            | |       / / ",
        "            | |      / /  ",
        "            J  \\  /\"  (",
        "            | ().'`-()/ ",
        "           _.-\"_.____/",
        "       _.-\"_.+\"|   ",
        " /\"\\.-\"_.-\"  | | ",
        " \\_   \"      | |   ",
        "   |ll       | |     ",
        "   \\\\\\\\      |-\\ ",
        "  __\\\\\\\\    ( .-\\",
        "`'''''''`----\\\\\\\\.....--'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''",
        "              \\\\\\\\''",
        NULL
    };

    char *art2[] = {
        "                              _.--\"\"-._",
        "  .                         .\"         \".",
        " / \\    ,^.         /(     Y             |      )\\",
        "/   `---. |--'\\    (  \\__..'--   -   -- -'\"\"-.-'  )",
        "|        :|    `>   '.     l_..-------.._l      .'",
        "|      __l;__ .'      \"-.__.||_.-'v'-._||`\"----\"",
        " \\  .-' | |  `              l._       _.'",
        "  \\/    | |                   l`^^'^^'j",
        "        | |                _   \\_____/     _",
        "        j |               l `--__)--'(__.--' |",
        "        | |               | /`---``-----'\"1 |  ,-----.",
        "        | |               )/  `--' '---'   \\'-'  ___  `-.",
        "        | |              //  `-'  '`----'  /  ,-'   I`.  \\",
        "      _ L |_            //  `-.-.'`-----' /  /  |   |  `. \\",
        "     '._' / \\         _/(   `/   )- ---' ;  /__.J   L.__.\\ :",
        "      `._;/-(-.......'  /        ) (     |  |            | |",
        "      `._;l _'--------_/        )-'/     :  |___.    _._./ ;",
        "        | |                 .__ )-'\\  __  \\  \\  I   1   / /",
        "        `-'                /   `-\\-(-'   \\ \\  `.|   | ,' /",
        "                           \\__  `-'    __/  `-. `---'',-'",
        "                              )-._.-- (        `-----'",
        "                             )(  l\\ o ('..-.",
        "                       _..--' _'-' '--'.-. |",
        "                __,,-'' _,,-''            \\ \\",
        "               f'. _,,-'                   \\ \\",
        "              ()--  |                       \\ \\",
        "                \\.  |                       /  \\",
        "                  \\ \\                      |._  |",
        "                   \\ \\                     |  ()|",
        "                    \\ \\                     \\  /",
        "                     ) `-.                   | |",
        "                    // .__)                  | |",
        "                 _.//7'                      | |",
        "               '---'                         j_| `",
        "                                            (| |",
        "                                             |  \\",
        "                                             |lllj",
        "                                             ||||| ",
        NULL
    };
    wattron(design_win,COLOR_PAIR(180));

    for(int i=0;art2[i]!=NULL;i++){
        mvwprintw(design_win,1+i,53,"%s",art2[i]);
    }
    for(int i=0;art1[i]!=NULL;i++){
        mvwprintw(design_win,1+i,1,"%s",art1[i]);
    }
    wrefresh(design_win);
    // wgetch(design_win);
    wattroff(design_win,COLOR_PAIR(180));
}


void kill_design_win(WINDOW *design_win){
    wclear(design_win);
    wrefresh(design_win);
    delwin(design_win);
}