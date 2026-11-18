#!/bin/bash
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

echo "=== Building AppImages for x86_64 and aarch64 ==="

# 1. Ensure appimagetool and runtimes are present
mkdir -p .appimage_tools
cd .appimage_tools

if [ ! -f "runtime-x86_64" ]; then
    echo "Downloading runtime-x86_64..."
    curl -sSL -o runtime-x86_64 https://github.com/AppImage/AppImageKit/releases/download/continuous/runtime-x86_64
fi

if [ ! -f "runtime-aarch64" ]; then
    echo "Downloading runtime-aarch64..."
    curl -sSL -o runtime-aarch64 https://github.com/AppImage/AppImageKit/releases/download/continuous/runtime-aarch64
fi

if [ ! -d "squashfs-root" ]; then
    echo "Downloading appimagetool..."
    curl -sSL -o appimagetool https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
    chmod +x appimagetool
    ./appimagetool --appimage-extract
fi

cd "${ROOT_DIR}"

# 2. Package x86_64 AppImage
echo "Packaging SysDVR-Client-x86_64.AppImage..."
rm -rf AppDir-x86_64
mkdir -p AppDir-x86_64/usr/bin AppDir-x86_64/usr/lib

cp -r published-x64/* AppDir-x86_64/usr/bin/
cp published-x64/cimgui.so AppDir-x86_64/usr/lib/
if [ -f "published-x64/libSDL2_image-2.0.so.0" ]; then
    cp -P published-x64/libSDL2_image* AppDir-x86_64/usr/lib/ || true
fi

cp Client/Platform/Linux/com.github.exelix11.sysdvr.desktop AppDir-x86_64/com.github.exelix11.sysdvr.desktop
cp Client/Platform/Linux/flatpak_icon.png AppDir-x86_64/com.github.exelix11.sysdvr.png
cp Client/Platform/Linux/flatpak_icon.png AppDir-x86_64/.DirIcon

cat << 'APP_RUN' > AppDir-x86_64/AppRun
#!/bin/sh
SELF=$(readlink -f "$0")
HERE=$(dirname "$SELF")
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${HERE}/usr/bin:${HERE}/usr/bin/runtimes/linux-x64/native:/usr/lib/x86_64-linux-gnu:/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}"
exec "${HERE}/usr/bin/SysDVR-Client" "$@"
APP_RUN
chmod +x AppDir-x86_64/AppRun

ARCH=x86_64 .appimage_tools/squashfs-root/usr/bin/appimagetool \
    --runtime-file .appimage_tools/runtime-x86_64 \
    --no-appstream AppDir-x86_64 SysDVR-Client-x86_64.AppImage

chmod +x SysDVR-Client-x86_64.AppImage
echo "Created SysDVR-Client-x86_64.AppImage successfully!"

# 3. Package aarch64 AppImage
echo "Packaging SysDVR-Client-aarch64.AppImage..."
rm -rf AppDir-aarch64
mkdir -p AppDir-aarch64/usr/bin AppDir-aarch64/usr/lib

cp -r published-arm64/* AppDir-aarch64/usr/bin/
cp published-arm64/cimgui.so AppDir-aarch64/usr/lib/

cp Client/Platform/Linux/com.github.exelix11.sysdvr.desktop AppDir-aarch64/com.github.exelix11.sysdvr.desktop
cp Client/Platform/Linux/flatpak_icon.png AppDir-aarch64/com.github.exelix11.sysdvr.png
cp Client/Platform/Linux/flatpak_icon.png AppDir-aarch64/.DirIcon

cat << 'APP_RUN' > AppDir-aarch64/AppRun
#!/bin/sh
SELF=$(readlink -f "$0")
HERE=$(dirname "$SELF")
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${HERE}/usr/bin:${HERE}/usr/bin/runtimes/linux-arm64/native:/usr/lib/aarch64-linux-gnu:/lib/aarch64-linux-gnu:${LD_LIBRARY_PATH}"
exec "${HERE}/usr/bin/SysDVR-Client" "$@"
APP_RUN
chmod +x AppDir-aarch64/AppRun

ARCH=arm_aarch64 .appimage_tools/squashfs-root/usr/bin/appimagetool \
    --runtime-file .appimage_tools/runtime-aarch64 \
    --no-appstream AppDir-aarch64 SysDVR-Client-aarch64.AppImage

chmod +x SysDVR-Client-aarch64.AppImage
echo "Created SysDVR-Client-aarch64.AppImage successfully!"

echo "=== All AppImages created successfully! ==="
ls -lh *.AppImage
