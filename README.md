
# Ben: A Vim-like File Editor

Ben is a minimalist, Vim-like file editor built using C. Designed for speed and simplicity, it currently features basic text editing capabilities with an emphasis on low-level control.

---

## Features

* **Insert Mode**: Type and edit text.
* **Normal Mode**: Navigate through the file.
* **Basic Commands**:
    * `Escape` to toggle between Normal and Special (command) modes.
    * `w` to save the current file.
    * `q` to quit the editor.
    * `wq` save the current file and exit ben

---

## Getting Dependencies

Ben relies on the `ncurses` library for terminal user interface functionality. Here's how to install it on various Linux distributions:

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install libncurses-dev
```

### Fedora

```bash
sudo dnf install ncurses-devel
```

### Arch Linux

```bash
sudo pacman -S ncurses
```

### openSUSE

```bash
sudo zypper install ncurses-devel
```

---

## Building from Source

To compile and run Ben, you'll need `gcc` (or another C compiler) and the `ncurses` development libraries installed.

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/Spunchkin/ben.git](https://github.com/Spunchkin/ben.git)
    cd ben
    ```

2.  **Compile and Install Ben:**
    Ben uses a `Makefile` for easy compilation and installation. Run the following commands:
    ```bash
    make
    sudo make install
    ```
    The `make` command compiles the project, and `sudo make install` moves the compiled `ben` executable to `/usr/local/bin`, making it accessible from anywhere in your terminal.

---

## Usage

### Normal Mode

* **Arrow Keys**: Move the cursor (or `h`, `j`, `k`, `l` once implemented).
* **`Escape`**: Switch to Special (command) Mode.

### Insert Mode

* **Type**: Insert characters directly.
* **`Enter`**: Insert a new line.
* **`Backspace`**: Delete the character before the cursor.
* **`Delete`**: Delete the character under the cursor.
* **`Escape`**: Switch to Normal Mode.

### Special (Command) Mode

* Press `Escape` from Normal Mode to enter Special Mode.
* Type your command at the bottom of the screen.
* **`w`**: Save the file. You will be prompted for a filename if you haven't saved before.
* **`q`**: Quit the editor.
* Press `Enter` to execute the command.
* Press `Escape` to return to Normal Mode without executing a command.

---

### Core Editor Functionality

* **File I/O:**
    * [x] Load a file from a command-line argument.
    * [x] Create a new, empty buffer if no file is provided.
    * [x] Save the current buffer to a file with the command `:w`.
    * [x] Save the current buffer to a specified filename with the command `:w filename.txt`.
* **Text Manipulation:**
    * [x] Insert characters at the cursor position.
    * [x] Delete characters with `Backspace`.
    * [x] Delete characters with `Delete` (`KEY_DC`).
    * [x] Handle line breaks when the `Enter` key is pressed.
    * [x] Merge lines correctly when `Backspace` is used at the beginning of a line.
* **Navigation:**
    * [x] Move the cursor up and down using `KEY_UP` and `KEY_DOWN`.
    * [x] Move the cursor left and right using `KEY_LEFT` and `KEY_RIGHT`.
    * [x] Adjust the cursor column when moving between lines of different lengths.

---

### User Interface & Experience

* **Display:**
    * [x] Draw the text content on the screen.
    * [x] Draw line numbers in a separate column.
    * [x] Scroll the view up and down to keep the cursor visible.
* **Status Bar:**
    * [x] Display the filename or `[No Name]`.
    * [x] Display the current cursor position (line and column numbers).
    * [x] Display the command input area when in special mode.
    * [ ] Provide feedback messages (e.g., "File saved") for user actions.
* **Modes & Controls:**
    * [x] A normal mode for text editing.
    * [x] A command mode (activated by `:`) for special commands.
    * [x] Exit the editor with the command `:q`.
    * [x] Save and exit with the command `:wq`.

---

### Planned Enhancements (Not Done)

* **Undo/Redo System:**
    * [ ] Implement a stack-based undo/redo mechanism to track changes.
* **Search Functionality:**
    * [ ] Add a search command (e.g., `/`) to find text within the buffer.
* **Copy & Paste:**
    * [ ] Implement a yank/paste buffer for copying and pasting text.
* **Navigation & Usability:**
    * [ ] Implement `KEY_HOME` and `KEY_END` to jump to the beginning/end of a line.
    * [ ] Add a command like `:N` to jump to a specific line number.
* **Performance:**
    * [ ] Optimize memory management to reduce frequent `realloc` calls.
    * [ ] Consider alternative data structures for large files, like a gap buffer, to improve performance.

---

## Contributing

Contributions are welcome! If you find a bug, have a feature request, or want to contribute code, please feel free to open an issue or submit a pull request on the GitHub repository.

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.

