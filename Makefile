# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    CC = gcc
    LDFLAGS = -lpdcurses
    TARGET = ben.exe
else ifeq ($(UNAME_S),Linux)
    CC = gcc
    LDFLAGS = -lcurses
    TARGET = ben
else ifeq ($(UNAME_S),Darwin)
    CC = gcc
    LDFLAGS = -lcurses
    TARGET = ben
else
    $(error Unsupported OS: $(UNAME_S))
endif

# Compilation flags
CFLAGS = -Wall -Wextra -std=c11 -g -Isrc

# Source files
SRCS = src/bin.c src/color_config.c src/file_operations.c src/text_editor_functions.c src/gap_buffer.c
OBJS = $(SRCS:.c=.o)

# Install directory
INSTALL_DIR = /usr/local/bin

.PHONY: all clean install deps

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

# Install
install: $(TARGET)
	sudo cp $(TARGET) $(INSTALL_DIR)
	@echo "Ben has been installed to $(INSTALL_DIR). You can now run 'ben' from anywhere."

# Dependencies
deps:
ifeq ($(OS),Windows_NT)
	@echo "Please install PDCurses manually on Windows"
	@echo "You can download it from: https://github.com/wmcbrine/PDCurses"
else ifeq ($(UNAME_S),Linux)
	sudo apt-get update
	sudo apt-get install libncurses5-dev libncursesw5-dev
else ifeq ($(UNAME_S),Darwin)
	brew install ncurses
endif

# Header dependencies
src/bin.o: src/bin.c src/text_editor_functions.h src/data_structures.h src/color_config.h
src/file_operations.o: src/file_operations.c src/data_structures.h src/text_editor_functions.h src/gap_buffer.h
src/text_editor_functions.o: src/text_editor_functions.c src/text_editor_functions.h src/color_config.h src/gap_buffer.h
src/color_config.o: src/color_config.c src/color_config.h
src/gap_buffer.o: src/gap_buffer.c src/gap_buffer.h
