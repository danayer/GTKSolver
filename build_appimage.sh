#!/bin/bash
set -e

# Define variables
APP_NAME="GTKSolver"
VERSION="0.1.0"
BUILD_DIR="$(pwd)/build"
APPDIR="$(pwd)/AppDir"
APPDIR_TRAINER="$(pwd)/AppDir-Trainer"
APPIMAGE_DIR="$(pwd)/appimage"
TORCH_PATH="$HOME/libtorch"

# Ensure required tools are installed
if ! command -v wget &> /dev/null; then
    echo "Installing wget..."
    sudo apt-get update && sudo apt-get install -y wget
fi

# Create directories
mkdir -p "$APPIMAGE_DIR"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/lib"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"
mkdir -p "$APPDIR/usr/share/metainfo"

# Create separate AppDir for trainer
mkdir -p "$APPDIR_TRAINER/usr/bin"
mkdir -p "$APPDIR_TRAINER/usr/lib"
mkdir -p "$APPDIR_TRAINER/usr/share/applications"
mkdir -p "$APPDIR_TRAINER/usr/share/icons/hicolor/256x256/apps"
mkdir -p "$APPDIR_TRAINER/usr/share/metainfo"

# Build the project if not already built
if [ ! -f "$BUILD_DIR/formula-teacher-gui" ] || [ ! -f "$BUILD_DIR/formula-teacher-trainer" ]; then
    echo "Building GTKSolver..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DCMAKE_PREFIX_PATH="$TORCH_PATH" -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc)
    cd ..
fi

# Check if both binaries exist
if [ ! -f "$BUILD_DIR/formula-teacher-gui" ]; then
    echo "Error: formula-teacher-gui binary not found!"
    exit 1
fi

if [ ! -f "$BUILD_DIR/formula-teacher-trainer" ]; then
    echo "Error: formula-teacher-trainer binary not found!"
    exit 1
fi

echo "Found both binaries, proceeding with AppImage creation"

# Copy GUI binary to AppDir
echo "Copying GUI binary to AppDir..."
cp "$BUILD_DIR/formula-teacher-gui" "$APPDIR/usr/bin/gtksolver-gui"

# Copy Trainer binary to Trainer AppDir
echo "Copying Trainer binary to Trainer AppDir..."
cp "$BUILD_DIR/formula-teacher-trainer" "$APPDIR_TRAINER/usr/bin/gtksolver-trainer"

# Copy LibTorch dependencies to both AppDirs
echo "Copying LibTorch dependencies..."
mkdir -p "$APPDIR/usr/lib/libtorch"
cp -r "$TORCH_PATH/lib/"*.so* "$APPDIR/usr/lib/" 2>/dev/null || true

mkdir -p "$APPDIR_TRAINER/usr/lib/libtorch"
cp -r "$TORCH_PATH/lib/"*.so* "$APPDIR_TRAINER/usr/lib/" 2>/dev/null || true

# Create desktop file for GUI - use RDNS style ID
cat > "$APPDIR/usr/share/applications/org.gtksolver.app.desktop" << EOF
[Desktop Entry]
Type=Application
Name=GTKSolver
Comment=Neural network for solving formula problems
Exec=gtksolver-gui
Icon=gtksolver
Terminal=false
Categories=Education;
X-AppStream-Ignore=false
EOF

# Create desktop file for Trainer - use RDNS style ID
cat > "$APPDIR_TRAINER/usr/share/applications/org.gtksolver.trainer.desktop" << EOF
[Desktop Entry]
Type=Application
Name=GTKSolver Trainer
Comment=Train neural network on text data
Exec=gtksolver-trainer
Icon=gtksolver-trainer
Terminal=false
Categories=Education;
X-AppStream-Ignore=false
EOF

# Create symlinks for desktop files in AppDirs
ln -sf usr/share/applications/org.gtksolver.app.desktop "$APPDIR/org.gtksolver.app.desktop" 
ln -sf usr/share/applications/org.gtksolver.trainer.desktop "$APPDIR_TRAINER/org.gtksolver.trainer.desktop"

# Create AppStream metadata for GUI
cat > "$APPDIR/usr/share/metainfo/org.gtksolver.app.appdata.xml" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<component type="desktop-application">
  <id>org.gtksolver.app</id>
  <metadata_license>MIT</metadata_license>
  <project_license>MIT</project_license>
  <name>GTKSolver</name>
  <summary>Neural network for solving formula problems</summary>
  <description>
    <p>
      GTKSolver is an intelligent system that uses neural networks to learn from textbook
      data containing mathematical formulas and subsequently solve problems on this topic.
    </p>
  </description>
  <launchable type="desktop-id">org.gtksolver.app.desktop</launchable>
  <url type="homepage">https://github.com/danayer/GTKSolver</url>
  <developer_name>Danayer</developer_name>
  <provides>
    <binary>gtksolver-gui</binary>
  </provides>
  <releases>
    <release version="0.1.0" date="2025-01-01">
      <description>
        <p>Initial release of GTKSolver</p>
      </description>
    </release>
  </releases>
  <content_rating type="oars-1.1" />
</component>
EOF

# Create AppStream metadata for Trainer
cat > "$APPDIR_TRAINER/usr/share/metainfo/org.gtksolver.trainer.appdata.xml" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<component type="desktop-application">
  <id>org.gtksolver.trainer</id>
  <metadata_license>MIT</metadata_license>
  <project_license>MIT</project_license>
  <name>GTKSolver Trainer</name>
  <summary>Train neural networks for formula solving</summary>
  <description>
    <p>
      GTKSolver Trainer allows you to train neural network models using textbook
      data containing mathematical formulas, which can then be used by GTKSolver
      to solve problems.
    </p>
  </description>
  <launchable type="desktop-id">org.gtksolver.trainer.desktop</launchable>
  <url type="homepage">https://github.com/danayer/GTKSolver</url>
  <developer_name>Danayer</developer_name>
  <provides>
    <binary>gtksolver-trainer</binary>
  </provides>
  <releases>
    <release version="0.1.0" date="2025-01-01">
      <description>
        <p>Initial release of GTKSolver Trainer</p>
      </description>
    </release>
  </releases>
  <content_rating type="oars-1.1" />
</component>
EOF

# Create icons for GUI
echo '<svg xmlns="http://www.w3.org/2000/svg" width="256" height="256"><circle cx="128" cy="128" r="120" fill="#3584e4"/><text x="128" y="128" font-size="70" text-anchor="middle" fill="white">GTK</text><text x="128" y="180" font-size="40" text-anchor="middle" fill="white">Solver</text></svg>' > "$APPDIR/usr/share/icons/hicolor/256x256/apps/gtksolver.svg"

# Create icons for Trainer
echo '<svg xmlns="http://www.w3.org/2000/svg" width="256" height="256"><circle cx="128" cy="128" r="120" fill="#33d17a"/><text x="128" y="128" font-size="70" text-anchor="middle" fill="white">GTK</text><text x="128" y="180" font-size="40" text-anchor="middle" fill="white">Train</text></svg>' > "$APPDIR_TRAINER/usr/share/icons/hicolor/256x256/apps/gtksolver-trainer.svg"

# Create AppRun script for GUI
cat > "$APPDIR/AppRun" << 'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"

echo "Starting GTKSolver GUI..."
exec "${HERE}/usr/bin/gtksolver-gui" "$@"
EOF
chmod +x "$APPDIR/AppRun"

# Create AppRun script for Trainer
cat > "$APPDIR_TRAINER/AppRun" << 'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"

echo "Starting GTKSolver Trainer..."
exec "${HERE}/usr/bin/gtksolver-trainer" "$@"
EOF
chmod +x "$APPDIR_TRAINER/AppRun"

# Download linuxdeploy if not exists
if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy..."
    wget -c "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod +x linuxdeploy-x86_64.AppImage
fi

# Build separate AppImages for GUI and Trainer
mkdir -p "$APPIMAGE_DIR"

echo "Building GTKSolver GUI AppImage..."
export APPIMAGE_EXTRACT_AND_RUN=1
./linuxdeploy-x86_64.AppImage --appdir="$APPDIR" --output appimage --desktop-file="$APPDIR/usr/share/applications/org.gtksolver.app.desktop"
mv ./*GTKSolver*.AppImage "$APPIMAGE_DIR/GTKSolver-GUI-x86_64.AppImage" 2>/dev/null || true

echo "Building GTKSolver Trainer AppImage..."
./linuxdeploy-x86_64.AppImage --appdir="$APPDIR_TRAINER" --output appimage --desktop-file="$APPDIR_TRAINER/usr/share/applications/org.gtksolver.trainer.desktop"
mv ./*GTKSolver*.AppImage "$APPIMAGE_DIR/GTKSolver-Trainer-x86_64.AppImage" 2>/dev/null || true

echo "AppImage creation complete!"
echo "You can find the AppImage files in the $APPIMAGE_DIR directory:"
ls -la "$APPIMAGE_DIR"
