# Windows Setup Guide

### Prerequisites
1. Install [MSYS2](https://www.msys2.org/) (provides Unix-like environment)
2. Install [Git for Windows](https://git-scm.com/download/win)

### Installation Steps
```bash
# 1. Open MSYS2 terminal
# 2. Install dependencies
pacman -Syu
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-pdcurses

# 3. Clone repository
git clone https://github.com/yourusername/ben-text-editor.git
cd ben-text-editor

# 4. Build the editor
make

# 5. Run the editor
./ben [filename]
