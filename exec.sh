#!/bin/bash

# Define an array of source files
source_files=("main.c" "menu.c" "exit.c" "choices.c" "design.c")

# Compile the program
gcc -o rogue "${source_files[@]}" -lncurses

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
fi

# Run the program
./rogue
