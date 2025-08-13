# Detect OS
UNAME_S := $(shell uname -s)
OS :=

ifeq ($(UNAME_S),Darwin)
    OS = Darwin
    LDFLAGS = -lncurses
else ifeq ($(UNAME_S),Linux)
    OS = Linux
    LDFLAGS = -lncurses
else
    # Assuming Windows if not Linux or Darwin
    OS = Windows_NT
    LDFLAGS = -lpdcurses
endif

CC = gcc
TARGET = ben
TEST_TARGET = ben_tests

# Compilation flags
CFLAGS = -Wall -Wextra -std=c11 -g -Isrc -Itests

# Source files for the main application
SRCS_MAIN = src/bin.c src/color_config.c src/file_operations.c src/text_editor_functions.c src/gap_buffer.c
OBJS_MAIN = $(SRCS_MAIN:.c=.o)

# Source files for the test suite
TEST_SRCS = tests/test_runner.c tests/test_framework.c tests/test_gap_buffer.c tests/test_data_structures.c tests/test_file_operations.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

# All object files for the test executable
TEST_EXEC_OBJS = $(OBJS_MAIN) $(TEST_OBJS)

# Install directory
INSTALL_DIR = /usr/local/bin

.PHONY: all clean install deps test

# Default target
all: $(TARGET)

# Build main application
$(TARGET): $(OBJS_MAIN)
	$(CC) $(OBJS_MAIN) -o $(TARGET) $(LDFLAGS)

# Build test application
$(TEST_TARGET): $(TEST_EXEC_OBJS)
	$(CC) $(TEST_EXEC_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS_MAIN) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)

# Install
install: $(TARGET)
	sudo cp $(TARGET) $(INSTALL_DIR)
	@echo "Ben has been installed to $(INSTALL_DIR). You can now run 'ben' from anywhere."

# Dependencies
deps:
ifeq ($(OS),Linux)
	sudo apt-get update
	sudo apt-get install libncurses5-dev libncursesw5-dev
endif
ifeq ($(OS),Darwin)
	brew install ncurses
endif

# Header dependencies for the main application
src/bin.o: src/bin.c src/text_editor_functions.h src/data_structures.h src/color_config.h
src/file_operations.o: src/file_operations.c src/data_structures.h src/gap_buffer.h
src/text_editor_functions.o: src/text_editor_functions.c src/text_editor_functions.h src/color_config.h src/gap_buffer.h
src/color_config.o: src/color_config.c src/color_config.h
src/gap_buffer.o: src/gap_buffer.c src/gap_buffer.h