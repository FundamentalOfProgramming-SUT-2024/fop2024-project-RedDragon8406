#!/bin/bash


source_files=("main.c" "menu.c" "exit.c" "choices.c" "design.c" "auth.c" "scoreboard.c" "profile.c" "settings.c")


gcc -o rogue "${source_files[@]}" -lncurses


if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
    ./rogue
fi
