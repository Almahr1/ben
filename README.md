# Ben: A Vim-like File Editor

Ben is a minimalist, Vim-like file editor built using C. Designed for speed and simplicity, it currently features basic text editing capabilities with an emphasis on low-level control.

---

## Features

* **Insert Mode**: Type and edit text.
* **Normal Mode**: Navigate through the file.
* **Basic Commands**:
    * `Escape` to toggle between Normal and Special (command) modes.
    * `:w` to save the current file.
    * `:q` to quit the editor.

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
    git clone [https://github.com/Spunchkin/ben](https://github.com/Spunchkin/ben.git)
    cd ben
    ```

2.  **Compile Ben:**
    Ben uses a `Makefile` for easy compilation.
    ```bash
    make
    ```

3.  **Run Ben:**
    ```bash
    ./ben [filename]
    ```
    Replace `[filename]` with the path to the file you want to edit. If no filename is provided, Ben will open an empty buffer.

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

## Contributing

Contributions are welcome! If you find a bug, have a feature request, or want to contribute code, please feel free to open an issue or submit a pull request on the GitHub repository.

---

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.
