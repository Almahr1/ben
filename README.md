
# Ben: A Vim-like File Editor

Ben is a minimalist, Vim-like file editor built using C. Designed for speed and simplicity, it currently features basic text editing capabilities with an emphasis on low-level control.

---


## Features

* **Three Editing Modes**:
    * **Normal Mode**: Navigate and manipulate text
    * **Insert Mode**: Type and edit content
    * **Command Mode**: Execute editor commands
* **Enhanced Navigation**:
    * Arrow keys and Vim-style `h`, `j`, `k`, `l` movement
    * Line wrapping support (toggle with `:wrap`/`:nowrap`)
* **File Operations**:
    * `:w` - Save file
    * `:w filename` - Save as new file
    * `:q` - Quit editor
    * `:wq` - Save and quit
* **Text Manipulation**:
    * Insert/delete characters/lines
    * Line merging/splitting
    * `x`/`X` to delete characters under/before cursor
* **Visual Enhancements**:
    * Line numbers with current line indicator
    * Color-coded mode indicators
    * Status bar with file info and cursor position
    * Customizable color schemes

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
* **Movement**: `h`(left), `j`(down), `k`(up), `l`(right) or arrow keys
* **Commands**:
    * `i` - Enter Insert mode at cursor
    * `a` - Enter Insert mode after cursor
    * `A` - Enter Insert mode at end of line
    * `o`/`O` - Insert new line below/above
    * `x` - Delete character under cursor
    * `X` - Delete character before cursor
    * `:` - Enter Command mode
    * `w` - Toggle line wrapping
* `Escape` returns to Normal Mode

### Insert Mode
* **Typing**: Insert characters directly
* **Special Keys**:
    * `Enter` - Split line
    * `Backspace` - Delete previous character/merge lines
    * `Delete` - Delete next character/merge with next line
    * Arrow keys - Move cursor
* `Escape` returns to Normal Mode

### Command Mode
* Press `:` from Normal Mode to enter
* **Commands**:
    * `w` - Save file
    * `w filename` - Save as
    * `q` - Quit
    * `wq` - Save and quit
    * `wrap`/`nowrap` - Toggle line wrapping
* Press `Enter` to execute or `Escape` to cancel

---

### Core Editor Functionality

* **File I/O**:
    * [x] Load files from command-line
    * [x] Create new buffers
    * [x] Save files (`:w`, `:w filename`)
* **Text Manipulation**:
    * [x] Character insertion/deletion
    * [x] Line splitting/merging
    * [x] Backspace/Delete handling
* **Navigation**:
    * [x] Arrow key movement
    * [x] Vim-style `h`/`j`/`k`/`l` movement
    * [x] Cross-line column adjustment
* **User Interface**:
    * [x] Line numbers with current line indicator
    * [x] Color-coded mode display
    * [x] Status bar with file info
    * [x] Command input area
    * [x] Customizable color schemes
* **Advanced Features**:
    * [x] Line wrapping with toggle
    * [x] Scrolling viewport
    * [x] Cursor position tracking

---

### Planned Enhancements

* **Editing Operations**:
    * [ ] Undo/Redo system
    * [ ] Copy/Paste functionality
* **Navigation**:
    * [ ] `HOME`/`END` key support
    * [ ] `:N` jump to line number
* **Search & Replace**:
    * [ ] Text search (`/` command)
    * [ ] Replace functionality
* **Performance**:
    * [ ] Optimized memory management
    * [ ] Gap buffer implementation
* **User Experience**:
    * [ ] Action feedback messages
    * [ ] Syntax highlighting
    * [ ] Multiple buffer support

---

## Contributing

Contributions are welcome! If you find a bug, have a feature request, or want to contribute code, please feel free to open an issue or submit a pull request on the GitHub repository.

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.


## Daily Message
 14/8/2025: couldnt work alot today due to hospital stuff
