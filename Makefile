# Detect operating system and set compiler/linker options
ifeq ($(OS),Windows_NT)
    CC = gcc
    LDFLAGS = -lpdcurses
    TARGET = ben.exe
else
    # Linux/macOS settings
    CC = gcc
    LDFLAGS = -lcurses
    TARGET = ben
endif

# Define compilation flags
# -Wall: Enable all common warnings
# -Wextra: Enable extra warnings
# -std=c11: Use the C11 standard
# -g: Include debugging information (for gdb)
# -Isrc: Look for header files in the src directory
CFLAGS = -Wall -Wextra -std=c11 -g -Isrc

# Define source files inside the src directory (including new gap_buffer.c)
SRCS = src/bin.c src/color_config.c src/file_operations.c src/text_editor_functions.c src/gap_buffer.c

# Automatically generate object file names from source files
OBJS = $(SRCS:.c=.o)

# Define installation directory
INSTALL_DIR = /usr/local/bin

.PHONY: all clean install deps

# Default target: builds the executable
all: $(TARGET)

# Rule to link the main executable
$(TARGET): $(OBJS)
    $(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up compiled files
clean:
    rm -f $(OBJS) $(TARGET)

# Rule to install the executable
# This will copy the compiled 'ben' executable to the INSTALL_DIR
# Use 'sudo make install' to run this command
install: $(TARGET)
    sudo cp $(TARGET) $(INSTALL_DIR)
    @echo "Ben has been installed to $(INSTALL_DIR). You can now run 'ben' from anywhere."

# Install development dependencies
deps:
ifeq ($(OS),Windows_NT)
    @echo "Please install PDCurses manually on Windows"
    @echo "You can download it from: https://github.com/wmcbrine/PDCurses"
else
    ifeq ($(shell uname -s),Linux)
    sudo apt-get update
    sudo apt-get install libncurses5-dev libncursesw5-dev
    endif
    ifeq ($(shell uname -s),Darwin)
    brew install ncurses
    endif
endif

# Dependency rules for proper rebuilding
src/bin.o: src/bin.c src/text_editor_functions.h src/data_structures.h src/color_config.h
src/file_operations.o: src/file_operations.c src/data_structures.h src/text_editor_functions.h src/gap_buffer.h
src/text_editor_functions.o: src/text_editor_functions.c src/text_editor_functions.h src/color_config.h src/gap_buffer.h
src/color_config.o: src/color_config.c src/color_config.h
src/gap_buffer.o: src/gap_buffer.c src/gap_buffer.h
