#!/bin/bash


source_files=("main.c" "menu.c" "exit.c" "choices.c" "design.c" "auth.c" "scoreboard.c" "profile.c" "settings.c" "game.c" "utils.c")

# -lSDL2 -lSDL2_mixer
# -fsanitize=address -g
gcc -o rogue "${source_files[@]}" -lncursesw -Wall 


if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
    ./rogue
fi
