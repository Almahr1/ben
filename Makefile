# Define the C compiler to use
CC = gcc

# Define compilation flags
# -Wall: Enable all common warnings
# -Wextra: Enable extra warnings
# -std=c11: Use the C11 standard
# -g: Include debugging information (for gdb)
# -Isrc: Look for header files in the src directory
CFLAGS = -Wall -Wextra -std=c11 -g -Isrc

# Define linker flags (for ncurses library)
# -lcurses or -lncurses: Link with the ncurses library
LDFLAGS = -lcurses

# Define the executable name
TARGET = ben

# Define source files inside the src directory
SRCS = src/bin.c src/file_operations.c src/text_editor_functions.c

# Automatically generate object file names from source files
OBJS = $(SRCS:.c=.o)

# Define installation directory
INSTALL_DIR = /usr/local/bin

.PHONY: all clean install

# Default target: builds the executable
all: $(TARGET)

# Rule to link the executable
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
	echo "Ben has been installed to $(INSTALL_DIR). You can now run 'ben' from anywhere."

