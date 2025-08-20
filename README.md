# Ben: A Vim-like Text Editor

A minimalist, Vim-inspired text editor built in C with ncurses. Features modal editing, undo/redo, search functionality, and line wrapping.

## Installation

### Dependencies
Install ncurses development libraries:

**Ubuntu/Debian:**
```bash
sudo apt update && sudo apt install libncurses-dev
```

**Fedora:**
```bash
sudo dnf install ncurses-devel
```

**Arch Linux:**
```bash
sudo pacman -S ncurses
```

### Build & Install
```bash
git clone https://github.com/Spunchkin/ben.git
cd ben
make
sudo make install
```

## Usage

### Starting
```bash
ben [filename]    # Open file or create new
```

### Normal Mode
| Command | Action |
|---------|--------|
| `h j k l` | Move left/down/up/right |
| `i` | Insert mode at cursor |
| `a` | Insert mode after cursor |
| `A` | Insert mode at end of line |
| `o` | Insert new line below |
| `O` | Insert new line above |
| `x` | Delete character under cursor |
| `X` | Delete character before cursor |
| `u` | Undo |
| `Ctrl+R` | Redo |
| `w` | Toggle line wrapping |
| `/` | Search forward |
| `?` | Search backward |
| `n` | Next search result |
| `N` | Previous search result |
| `:` | Command mode |
| `Esc` | Return to normal mode |

### Insert Mode
- Type normally to insert text
- `Enter` splits line
- `Backspace`/`Delete` removes characters
- Arrow keys move cursor
- `Esc` returns to normal mode

### Command Mode
| Command | Action |
|---------|--------|
| `:w` | Save file |
| `:w filename` | Save as filename |
| `:q` | Quit |
| `:wq` | Save and quit |
| `:wrap` | Enable line wrapping |
| `:nowrap` | Disable line wrapping |
| `:nohl` | Clear search highlighting |
| `:set ic` | Case insensitive search |
| `:set noic` | Case sensitive search |

## Features

- **Modal editing** (Normal/Insert/Command modes)
- **File operations** (open, save, save as)
- **Undo/Redo system** with operation tracking
- **Search functionality** with highlighting and navigation
- **Line wrapping** with toggle support
- **Gap buffer** text storage for efficient editing
- **Color-coded interface** with customizable themes
- **Status bar** showing file info and cursor position

## Architecture

Ben uses a doubly-linked list of lines, each containing a gap buffer for efficient text manipulation. The editor maintains separate state for cursor position, display viewport, and search results.

## License

MIT License - see LICENSE file for details.
