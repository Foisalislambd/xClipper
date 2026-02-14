#!/bin/bash

# Exit on error
set -e

APP_NAME="xClipper"
PKG_NAME="xclipper" # Package names must be lowercase
VERSION="1.0.0"
ARCH="amd64"
DEB_NAME="${PKG_NAME}_${VERSION}_${ARCH}"

# Get the directory where this script is located, then go up one level to find the project root
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
RELEASE_DIR="$PROJECT_ROOT/release"
DEB_DIR="$RELEASE_DIR/$DEB_NAME"

echo "🚀 Starting .deb build for $APP_NAME..."

# 1. Clean and Prepare Directories
rm -rf "$DEB_DIR"
mkdir -p "$DEB_DIR/usr/bin"
mkdir -p "$DEB_DIR/usr/share/applications"
mkdir -p "$DEB_DIR/usr/share/icons/hicolor/256x256/apps"
mkdir -p "$DEB_DIR/DEBIAN"

# 2. Build Project
echo "🛠️  Compiling..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ..
make -j$(nproc)
cd "$PROJECT_ROOT"

# 3. Copy Files to Package Structure
echo "📂 Copying files..."
cp "$BUILD_DIR/$APP_NAME" "$DEB_DIR/usr/bin/$PKG_NAME"
cp "$PROJECT_ROOT/xClipper.desktop" "$DEB_DIR/usr/share/applications/"
cp "$PROJECT_ROOT/assets/icon.png" "$DEB_DIR/usr/share/icons/hicolor/256x256/apps/xClipper.png"

# Update .desktop file to point to the lowercase binary name and correct icon
sed -i "s/Exec=xClipper/Exec=$PKG_NAME/g" "$DEB_DIR/usr/share/applications/xClipper.desktop"

# 4. Create Control File (Metadata)
echo "📝 Generating control file..."
# Note: We list generic Qt dependencies. 
cat > "$DEB_DIR/DEBIAN/control" <<EOF
Package: $PKG_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: $ARCH
Depends: libc6, libstdc++6, libqt6widgets6 | libqt5widgets5
Maintainer: Foisal Islam <foisal@example.com>
Description: A modern clipboard history manager.
 xClipper tracks your clipboard history and allows you to 
 access previous clippings easily from the system tray.
EOF

# 5. Build the .deb file
echo "📦 Building .deb package..."
dpkg-deb --build "$DEB_DIR"

echo ""
echo "✅ Build Complete!"
echo "🎉 .deb file is located at: $RELEASE_DIR/${DEB_NAME}.deb"
echo "To install, run: sudo apt install ./release/${DEB_NAME}.deb"
