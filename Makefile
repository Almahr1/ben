# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    CC = gcc
    LDFLAGS = -lpdcurses
    TARGET = ben.exe
    TEST_TARGET = ben_tests.exe
else ifeq ($(UNAME_S),Linux)
    CC = gcc
    LDFLAGS = -lcurses
    TARGET = ben
    TEST_TARGET = ben_tests
else ifeq ($(UNAME_S),Darwin)
    CC = gcc
    LDFLAGS = -lcurses
    TARGET = ben
    TEST_TARGET = ben_tests
else
    $(error Unsupported OS: $(UNAME_S))
endif

# Compilation flags
CFLAGS = -Wall -Wextra -std=c11 -g -Isrc -Itests

# Source files
SRCS = src/bin.c src/color_config.c src/file_operations.c src/text_editor_functions.c src/gap_buffer.c
OBJS = $(SRCS:.c=.o)

# Test source files
TEST_SRCS = tests/test_framework.c tests/test_gap_buffer.c tests/test_data_structures.c tests/test_file_operations.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Install directory
INSTALL_DIR = /usr/local/bin

.PHONY: all clean install deps test

# Default target
all: $(TARGET)

# Build main application
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Build test application
$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(CC) $(OBJS) $(TEST_OBJS) -o $(TEST_TARGET)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)

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