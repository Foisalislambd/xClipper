# LixClipper

### A Project by **MD Foisal Islam**

**LixClipper** is a lightweight, modern clipboard history manager designed for Linux. It runs quietly in the background, keeping track of your copied text so you never lose an important snippet again.

---

## 📖 User Guide

### How it Works
1.  **Copy as usual**: Whenever you press `Ctrl+C` or copy text from any application, LixClipper saves it.
2.  **Access History**: Click the **Tray Icon** (computer icon) in your system bar to open the history window.
3.  **Paste**: Click on any item in the list to copy it back to your clipboard. You can then paste it (`Ctrl+V`) anywhere.
4.  **Clear**: Use the "Clear all" button to wipe your history.

### Features
*   **Automatic Tracking**: Instantly saves copied text.
*   **Modern UI**: Dark mode interface inspired by modern design standards.
*   **Tray Integration**: Stays out of your way until you need it.
*   **Privacy Focused**: Runs locally on your machine.
*   **Smart History**: Removes duplicates and keeps the most recent items at the top (stores up to 20 items).

---

## 🛠️ Installation & Setup

Follow these simple steps to get LixClipper running on your Linux machine.

### 1. Install Requirements
You need a C++ compiler and Qt libraries. Open your terminal and run the command for your OS:

**Ubuntu / Debian / Linux Mint:**
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev
```
*(If you prefer Qt5, you can install `qtbase5-dev` instead)*

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake qt6-base
```

### 2. Build the Project
Open a terminal in the project folder and run:

```bash
# Create a build folder
mkdir build
cd build

# Configure the project
cmake ..

# Compile
make -j$(nproc)
```

### 3. Run LixClipper
Once built, you can start the app directly:

```bash
./LixClipper
```

*Tip: You can add this command to your "Startup Applications" to have LixClipper start automatically when you log in.*

---

## ❓ Troubleshooting

*   **"I don't see the icon!"**: Check your system tray area. On some desktop environments (like GNOME), you might need an extension like "AppIndicator" to see tray icons.
*   **Window closes too fast**: The window is designed to close when you click outside of it or switch windows. This is normal behavior for a popup tool.

---

**Developed with ❤️ by Foisal Islam**
