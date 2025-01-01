// menu.c
#include <ncurses.h>
#include <unistd.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

char *choices[] = {
    "New Game",
    "Load Game",
    "Profile",
    "Settings",
    "Scoreboard",
    "Exit",
};

void print_menu(WINDOW *menu_win, int highlight, int n_choices) {
    int x, y, i;
    x = 2;
    y = 2;
    box(menu_win, 0, 0);
    for (i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) { // Highlight the present choice
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}

void menu() {
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;
    int n_choices = ARRAY_SIZE(choices);

    initscr();
    clear();
    noecho();
    cbreak(); // Line buffering disabled. Pass on everything
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);

    int height = 10;
    int width = 30;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);
    mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
    refresh();
    print_menu(menu_win, highlight, n_choices);
    while (1) {
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
                mvprintw(24, 0, "Character pressed is = %3d Hopefully it can be printed as '%c'", c, c);
                refresh();
                break;
        }
        print_menu(menu_win, highlight, n_choices);
        if (choice != 0) // User made a choice, come out of the infinite loop
            break;
    }
    mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
    clrtoeol();
    refresh();
    getch();
    endwin();
}
