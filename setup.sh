#!/bin/bash

# Compile the program
gcc -Wall -Wextra -pedantic -std=c99 src/bin.c src/file_operations.c src/text_editor_functions.c -lncurses -o ben

# Move the executable to a directory in PATH
sudo mv ben /usr/local/bin

echo "Setup complete. You can now run 'ben' from anywhere in your terminal."
