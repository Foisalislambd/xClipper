#!/bin/bash

# Exit on error
set -e

echo "🚀 Starting LixClipper AppImage Build..."

# Define directories
# Get the directory where this script is located, then go up one level to find the project root
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
APP_DIR="$PROJECT_ROOT/AppDir"
RELEASE_DIR="$PROJECT_ROOT/release/AppImage"

# Clean up previous builds (Only for AppImage)
rm -rf "$APP_DIR" "$RELEASE_DIR"
mkdir -p "$APP_DIR" "$RELEASE_DIR"

# 1. Build the Application
echo "🛠️  Compiling LixClipper..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
cd "$PROJECT_ROOT"

# 2. Prepare AppDir Structure
echo "📂 Setting up AppDir..."
mkdir -p "$APP_DIR/usr/bin"
mkdir -p "$APP_DIR/usr/share/applications"
mkdir -p "$APP_DIR/usr/share/icons/hicolor/256x256/apps"

# 3. Copy Files
echo "COPYing binary and assets..."
cp "$BUILD_DIR/LixClipper" "$APP_DIR/usr/bin/"
cp "$PROJECT_ROOT/LixClipper.desktop" "$APP_DIR/usr/share/applications/"
cp "$PROJECT_ROOT/assets/icon.png" "$APP_DIR/usr/share/icons/hicolor/256x256/apps/LixClipper.png"

# 4. Download LinuxDeploy and Qt Plugin
echo "⬇️  Downloading LinuxDeploy tools..."
cd "$RELEASE_DIR"

if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x linuxdeploy-x86_64.AppImage
fi

if [ ! -f "linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
fi

# 5. Generate AppImage
echo "📦 Generating AppImage..."
# Explicitly use qmake6 if found, otherwise fall back
export QMAKE=$(which qmake6 || which qmake-qt5 || which qmake)
echo "ℹ️  Using qmake at: $QMAKE"

# IMPORTANT: Backslashes allow the command to span multiple lines.
./linuxdeploy-x86_64.AppImage \
    --appdir "$APP_DIR" \
    --plugin qt \
    --output appimage \
    --icon-file "$PROJECT_ROOT/assets/icon.png" \
    --desktop-file "$PROJECT_ROOT/LixClipper.desktop"

# Move final AppImage to release folder
mv LixClipper*.AppImage "$RELEASE_DIR/"

echo ""
echo "✅ Build Complete!"
echo "🎉 Your AppImage is located in: $RELEASE_DIR"
echo "You can give this file to anyone using Linux!"
