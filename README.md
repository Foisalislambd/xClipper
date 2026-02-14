# xClipper - Linux Clipboard History

A simple, human-friendly clipboard history manager for Linux, built with C++ and Qt.

## Features
- **Clipboard Monitoring**: Automatically records copied text.
- **History**: Stores the last 20 items.
- **Tray Icon**: Runs in the background with a system tray icon.
- **Quick Paste**: Click an item in the history list to copy it back to the clipboard.

## Prerequisites

You need a C++ compiler, CMake, and Qt development libraries.

### Ubuntu / Debian
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev
# OR for Qt5
# sudo apt install build-essential cmake qtbase5-dev
```

### Fedora
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel cmake qt6-base
```

## Build & Run

1.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

2.  Configure with CMake:
    ```bash
    cmake ..
    ```

3.  Build:
    ```bash
    make
    ```

4.  Run:
    ```bash
    ./xClipper
    ```

## Usage

-   The application runs in the background. Look for the computer icon in your system tray.
-   Click the tray icon to toggle the history window.
-   Click any item in the list to copy it to your clipboard.
-   The window hides automatically when you click outside of it.


